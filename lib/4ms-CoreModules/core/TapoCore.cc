#include "info/Tapo_info.hh"
#include "CoreModules/4ms/4ms_element_state_conversions.hh"
#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"

#include "tapo/multitap_delay.hh"
#include "tapo/ui.hh"

#include <alpaca/alpaca.h>

// #define PRINTS

#ifdef PRINTS
#include <cstdio>
#endif

namespace MetaModule
{

class TapoCore : public SmartCoreProcessor<TapoInfo> {
	using Info = TapoInfo;
	using ThisCore = TapoCore;
	using enum Info::Elem;

public:
	TapoCore() : audioBufferFillCount(0), gateOutCounter(0), uiSampleCounter(0)
	{
		#ifdef PRINTS
		printf("Constructor TapoCore\n");
		#endif

		delay.Init((short*)buffer.data(), buffer.size()/sizeof(short) / 2, DefaultSampleRateInHz);

		currentSampleRateInHz = DefaultSampleRateInHz;

		delay.tap_modulo_observable_.set_observer([this]
		{
			onTapDetected();
		});

		ui.Init(&delay, &parameters);

		audioBufferFillCount = 0;
		std::fill(audioBufferTX.begin(), audioBufferTX.end(), ::TapoDelay::ShortFrame{0,0});

		ui.getADCDriver().set(TapoDelay::ADC_SCALE_CV,      0.5f);
		ui.getADCDriver().set(TapoDelay::ADC_FEEDBACK_CV,   0.5f);
		ui.getADCDriver().set(TapoDelay::ADC_MODULATION_CV, 0.5f);
		ui.getADCDriver().set(TapoDelay::ADC_DRYWET_CV,     0.5f);
		ui.getADCDriver().set(TapoDelay::ADC_VEL_CV,     	0.0f);
		ui.getADCDriver().set(TapoDelay::ADC_TAPTRIG_CV,   	0.0f);
		ui.getADCDriver().set(TapoDelay::ADC_CLOCK_CV,   	0.0f);

		for (std::size_t i=0; i<TapoDelay::kNumSlots; i++)
		{
			ui.getPersistentStorage().ResetSlot(i);
		}
	}

	void set_samplerate(float sr) override {

		auto new_samplerate = uint32_t(std::round(sr));

		if (new_samplerate != currentSampleRateInHz)
		{
			populateSaveState();

			#ifdef PRINTS
			printf("Change sample rate %uHz -> %uHz\n", currentSampleRateInHz, new_samplerate);
			#endif

			currentSampleRateInHz = new_samplerate;
			
			delay.changeSampleRate(currentSampleRateInHz);

			applySaveState();	
		}
	}

	void update() override {
		if (uiSampleCounter++ >= (float(currentSampleRateInHz)/TickFreqHz)) {
			uiSampleCounter = 0;

			sideloadDrivers();
			pollAltParameters();

			ui.Tick();
			ui.Poll();
			ui.DoEvents();
		}

		if (audioBufferFillCount == BlockSize)
		{
			ui.ReadParameters();
			delay.Poll();
			delay.Process(&parameters, audioBufferRX.data(), audioBufferTX.data());

			audioBufferFillCount = 0;
		}

		packUnpackBlockBuffers();
		audioBufferFillCount++;

		updateGateOut();
	}

	//////////////////////////////////

	// This struct is serialized and deserialized on save/load
	// To preserve backwards compatibility, only add fields
	// (do not reorder or remove)
	struct SaveState_t
	{
		bool repeat = false;
		int8_t current_slot = -1;
		bool sync = false;
		std::vector<TapoDelay::Persistent::SavedSlot> slots;
	};

	SaveState_t saveState{};

	void load_state(std::string_view state_data) override 
	{
		auto raw_data = decode(state_data);

		std::error_code ec;
		auto newSaveState = alpaca::deserialize<alpaca::options::with_version, SaveState_t>(raw_data, ec);
		if (!ec)
		{
			// store current state so it can be applied later
			saveState = newSaveState;

			#ifdef PRINTS
			printf("Loaded: Repeat %u, Current Slot %d, Sync %u, NumbSlots %u\n", saveState.repeat, saveState.current_slot, saveState.sync, saveState.slots.size());
			#endif

			applySaveState();
		}
		else
		{
			// Deserialization error
			// just ignore
			#ifdef PRINTS
			printf("Deserialization Error: %s\n", ec.message().c_str());
			#endif
		}
	}

	std::string save_state() override 
	{
		populateSaveState();

		#ifdef PRINTS
		printf("Save: Repeat %u, Current Slot %d, Sync %u, NumbSlots %u\n", saveState.repeat, saveState.current_slot, saveState.sync, saveState.slots.size());
		#endif

		std::vector<uint8_t> bytes;
		alpaca::serialize<alpaca::options::with_version>(saveState, bytes);

		return encode({bytes.data(), bytes.size()});
	}

	void applySaveState()
	{
		#ifdef PRINTS
		printf("Apply: Repeat %u, Current Slot %d, Sync %u, NumbSlots %u\n", saveState.repeat, saveState.current_slot, saveState.sync, saveState.slots.size());
		#endif

		// at this point we assume all called elements are initialized
		ui.getPersistentStorage().load_custom_slots(std::span{saveState.slots.data(), saveState.slots.size()});

		// only restore if current slot is valid
		if (saveState.current_slot > 0)
		{
			ui.set_current_slot(saveState.current_slot);
		}
		delay.set_sync(saveState.sync);
		delay.set_repeat(saveState.repeat ? 1.0f : 0.0f);

		ui.ReloadCurrentSlot();
	}

	void populateSaveState()
	{
		// set current state so it will be applied correctly on sample rate change
		saveState = SaveState_t
		{
			.repeat = delay.repeat() > 0.0f,
			.current_slot = ui.current_slot(),
			.sync = delay.sync(),
			.slots = ui.getPersistentStorage().get_custom_slots()
		};

		#ifdef PRINTS
		printf("Populate: Repeat %u, Current Slot %d, Sync %u, NumbSlots %u\n", saveState.repeat, saveState.current_slot, saveState.sync, saveState.slots.size());
		#endif
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	void packUnpackBlockBuffers()
	{
		auto clamp = [](auto val, auto max_amplitude)
		{
			return std::min(std::max(val, -max_amplitude), max_amplitude);
		};

		auto monoInput = int16_t(clamp(getInput<AudioIn>().value_or(0) / AudioInputFullScaleInVolt, 1.0f) * 32767.0 );
		audioBufferRX[audioBufferFillCount] = {monoInput, monoInput};

		if (isPatched<AudioOut2Out>())
		{
			setOutput<AudioOut1Out>(float(audioBufferTX[audioBufferFillCount].l) / 32768.0 * AudioOutputFullScaleInVolt);
			setOutput<AudioOut2Out>(float(audioBufferTX[audioBufferFillCount].r) / 32768.0 * AudioOutputFullScaleInVolt);
		}
		else
		{
			setOutput<AudioOut1Out>((float(audioBufferTX[audioBufferFillCount].l) + float(audioBufferTX[audioBufferFillCount].r)) / 2 / 32768.0 * AudioOutputFullScaleInVolt);
		}
	}

	void sideloadDrivers()
	{
		using namespace ::TapoDelay;

		auto& adc      = ui.getADCDriver();
		auto& buttons  = ui.getButtonDriver();
		auto& leds     = ui.getLEDDriver();
		auto& switches = ui.getSwitchDriver();
		auto& gate     = ui.getGateInputDriver();

		//
		// Knobs
		//
		adc.set(ADC_SCALE_POT,      getState<TimeKnob>());
		adc.set(ADC_FEEDBACK_POT,   getState<FeedbackKnob>());
		adc.set(ADC_MODULATION_POT, getState<ModulationKnob>());
		adc.set(ADC_DRYWET_POT, 	getState<DryWetKnob>());
		adc.set(ADC_MORPH_POT, 		getState<MorphKnob>());
		adc.set(ADC_GAIN_POT, 		getState<LevelKnob>());

		//
		// Biploar CVs
		//
		auto BipolarCVInputFunc = [](float voltage)
		{
			return std::clamp(voltage / (2 * BiploarCVInputFullScaleInVolt) + 0.5f, 0.0f, 1.0f);
		};
		if (auto val=getInput<TimeJackIn>(); val)       adc.set(ADC_SCALE_CV,      BipolarCVInputFunc(*val));
		if (auto val=getInput<FeedbackJackIn>(); val)   adc.set(ADC_FEEDBACK_CV,   BipolarCVInputFunc(*val));
		if (auto val=getInput<ModulationJackIn>(); val) adc.set(ADC_MODULATION_CV, BipolarCVInputFunc(*val));
		if (auto val=getInput<DryWetJackIn>(); val)     adc.set(ADC_DRYWET_CV,     BipolarCVInputFunc(*val));

		//
		// Velocity CV
		//
		auto VelocityCVInputFunc = [](float voltage)
		{
			return std::clamp(voltage / VelocityCVInputFullScaleInVolt, 0.0f, 1.0f);
		};
		if (auto val=getInput<VelocityIn>(); val)       adc.set(ADC_VEL_CV, VelocityCVInputFunc(*val));

		//
		// Gate and trigger inputs
		//
		if (auto val=getInput<RepeatJackIn>(); val)  gate.set(GATE_INPUT_REPEAT, *val > GateInputThresholdInVolt);
		if (auto val=getInput<TapIn>(); val)         adc.set(ADC_TAPTRIG_CV,     *val > GateInputThresholdInVolt);		
		if (auto val=getInput<ExtClockIn>(); val)    adc.set(ADC_CLOCK_CV,       *val > GateInputThresholdInVolt);		

		//
		// Tap Button
		//
		adc.set(ADC_FSR_CV, getState<TapSensorButton>() == MomentaryButton::State_t::PRESSED ? TapSensorPressedIntensity : 0.0);

		//
		// Button
		//
		buttons.set(BUTTON_1,      getState<Button1Button>() == MomentaryButton::State_t::RELEASED);
		buttons.set(BUTTON_2,      getState<Button2Button>() == MomentaryButton::State_t::RELEASED);
		buttons.set(BUTTON_3,      getState<Button3Button>() == MomentaryButton::State_t::RELEASED);
		buttons.set(BUTTON_4,      getState<Button4Button>() == MomentaryButton::State_t::RELEASED);
		buttons.set(BUTTON_5,      getState<Button5Button>() == MomentaryButton::State_t::RELEASED);
		buttons.set(BUTTON_6,      getState<Button6Button>() == MomentaryButton::State_t::RELEASED);
		buttons.set(BUTTON_REPEAT, getState<RepeatButton>()  == MomentaryButton::State_t::RELEASED);
		buttons.set(BUTTON_DELETE, getState<DeleteButton>()  == MomentaryButton::State_t::RELEASED);

		//
		// Three-Way Switches
		//
		auto SwitchStateMappingEdit = [](auto val) -> uint8_t
		{
			// map position VCV -> firmware
			switch(val)
			{
				case 0:  return 1; // overdub
				case 1:  return 0; // off
				case 2:  return 2; // normal
				default: return 0;
			}
		};

		auto SwitchStateMappingVelo = [](auto val) -> uint8_t
		{
			// map position VCV -> firmware
			switch(val)
			{
				case 0:  return 1; // lp
				case 1:  return 0; // amp 
				case 2:  return 2; // bp
				default: return 0;
			}
		};
		switches.set(SWITCH_EDIT, SwitchStateMappingEdit(getState<AddOffInsSwitch>()));
		switches.set(SWITCH_VELO, SwitchStateMappingVelo(getState<ResAmpLpfSwitch>()));

		//
		// LEDs
		//
		setLED<Button1Button>(std::array{leds.get(LED_BUT1_R), leds.get(LED_BUT1_G), leds.get(LED_BUT1_B)});
		setLED<Button2Button>(std::array{leds.get(LED_BUT2_R), leds.get(LED_BUT2_G), leds.get(LED_BUT2_B)});
		setLED<Button3Button>(std::array{leds.get(LED_BUT3_R), leds.get(LED_BUT3_G), leds.get(LED_BUT3_B)});
		setLED<Button4Button>(std::array{leds.get(LED_BUT4_R), leds.get(LED_BUT4_G), leds.get(LED_BUT4_B)});
		setLED<Button5Button>(std::array{leds.get(LED_BUT5_R), leds.get(LED_BUT5_G), leds.get(LED_BUT5_B)});
		setLED<Button6Button>(std::array{leds.get(LED_BUT6_R), leds.get(LED_BUT6_G), leds.get(LED_BUT6_B)});

		setLED<RepeatButton>(std::array{leds.get(LED_REPEAT_R), leds.get(LED_REPEAT_G), leds.get(LED_REPEAT_B)});
		setLED<DeleteButton>(std::array{leds.get(LED_DELETE_R), leds.get(LED_DELETE_G), leds.get(LED_DELETE_B)});

		setLED<TapLedLight>(leds.get(LED_TAP));
	}

	void pollAltParameters()
	{
		auto ProcessAltParam = [this](std::size_t index, unsigned newVal) -> std::optional<unsigned>
		{
			auto& thisOldValue = altParamsOldValues[index];

			// check if the alt parameter has been changed since the last time
			if (newVal != thisOldValue)
			{
				thisOldValue = newVal;

				ui.setSettingsItem(int(index), int(newVal));
			}
			else
			{
				// check the setting has been changed internally
				// and alt param needs to be updated
				auto internalValue = unsigned(ui.getSettingsItem(int(index)));
				if (thisOldValue and internalValue != thisOldValue)
				{
					return internalValue;
				}
			}

			return std::nullopt;
		};

		if (auto newVal = ProcessAltParam(TapoDelay::SettingsPages::PAGE_VELOCITY_PARAMETER, getState<VelocityAltParam>()); newVal)
		{
			// set alt param if this is supported
		}
		if (auto newVal = ProcessAltParam(TapoDelay::SettingsPages::PAGE_BANK, getState<BankAltParam>()); newVal)
		{
			// set alt param if this is supported
		}
		if (auto newVal = ProcessAltParam(TapoDelay::SettingsPages::PAGE_PANNING_MODE, getState<PanAltParam>()); newVal)
		{
			// set alt param if this is supported
		}
		if (auto newVal = ProcessAltParam(TapoDelay::SettingsPages::PAGE_SEQUENCER, getState<ModeAltParam>()); newVal)
		{
			// set alt param if this is supported
		}

	}

	void onTapDetected()
	{
		pingGateOut();
		ui.PingGateLed();
	}

private:
	void updateGateOut()
	{
		if (gateOutCounter != 0 and --gateOutCounter == 0)
		{
			setOutput<GateOut>(0);
		}
	}

	void pingGateOut()
	{
		gateOutCounter = GateOutLengthInS * float(currentSampleRateInHz);
		setOutput<GateOut>(GateOutVoltageInVolt);
	}	

private:
	static constexpr uint32_t DefaultSampleRateInHz   = 48000;

	static constexpr uint32_t TickFreqHz = 1000;

	static constexpr float GateOutLengthInS           = 4.0e-3f;
	static constexpr float GateOutVoltageInVolt       = 8.0f;	
	static constexpr float TapSensorPressedIntensity  = 0.5f;
	static constexpr float GateInputThresholdInVolt   = 0.5f;
	static constexpr float AudioInputFullScaleInVolt  = 22.0f;
	static constexpr float AudioOutputFullScaleInVolt = 17.0f;
	
	static constexpr float BiploarCVInputFullScaleInVolt  = 5.0f;
	static constexpr float VelocityCVInputFullScaleInVolt = 8.0f;

private:
	uint32_t currentSampleRateInHz = DefaultSampleRateInHz;

private:
	static constexpr std::size_t BufferSizeInBytes = 0x02000000;
	using Buffer_t = std::array<uint8_t,BufferSizeInBytes>;

	Buffer_t buffer{};

	::TapoDelay::MultitapDelay delay{};
	::TapoDelay::Parameters parameters{};
	::TapoDelay::Ui ui{};
	
	static constexpr std::size_t BlockSize = ::TapoDelay::kBlockSize;
	std::array<::TapoDelay::ShortFrame,BlockSize> audioBufferRX;
	std::array<::TapoDelay::ShortFrame,BlockSize> audioBufferTX;
	std::size_t audioBufferFillCount;

	uint32_t gateOutCounter{};

	uint32_t uiSampleCounter{};

	std::array<std::optional<unsigned>,4> altParamsOldValues{};
};

} // namespace MetaModule

