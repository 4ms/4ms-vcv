#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/PEG_info.hh"

#include "peg/PEGChannel.h"

#include <alpaca/alpaca.h>
#include <array>
#include <algorithm>

namespace MetaModule
{

class PEGCore : public SmartCoreProcessor<PEGInfo> {
public:
	using Info = PEGInfo;
	using ThisCore = PEGCore;
	using enum Info::Elem;

	template<Info::Elem EL>
	void setOutput(auto val) {
		return SmartCoreProcessor<Info>::setOutput<EL>(val);
	}

	template<Info::Elem EL>
	auto getInput() {
		return SmartCoreProcessor<Info>::getInput<EL>();
	}

	template<Info::Elem EL, typename VAL>
	void setLED(const VAL &value) {
		return SmartCoreProcessor<Info>::setLED<EL>(value);
	}

	template<Info::Elem EL>
	auto getState() {
		return SmartCoreProcessor<Info>::getState<EL>();
	}

private:

	private:
	struct MappingA
	{
		const static Info::Elem PingDivMultKnob = PingDivMultRedKnob;
		const static Info::Elem ScaleKnob    	= ScaleRedKnob;
		const static Info::Elem SkewKnob		= SkewRedKnob;
		const static Info::Elem CurveKnob		= CurveRedKnob;
		const static Info::Elem PingButton   	= PingRedButton;
		const static Info::Elem CycleButton  	= CycleRedButton;
		const static Info::Elem BiNPolarButton	= BiNPolarRedButton;
		const static Info::Elem PingJackIn		= PingRedJackIn;
		const static Info::Elem QntIn 			= QntRedIn;
		const static Info::Elem AsyncIn			= AsyncRedIn;
		const static Info::Elem DivJackIn		= DivRedJackIn;
		const static Info::Elem SkewJackIn		= SkewRedJackIn;
		const static Info::Elem CurveJackIn 	= CurveRedJackIn;
		const static Info::Elem EnvOut       	= EnvRedOut;
		const static Info::Elem _5VEnvOut    	= P5VEnvRedOut;
		const static Info::Elem EoFOut       	= EofRedOut;
		const static Info::Elem SecondaryOut    = EorOut;
		const static Info::Elem EnvOutLight  	= EnvredlightLight;
		const static Info::Elem EofLight     	= EofredlightLight;
		const static Info::Elem SecondaryLight  = EorredlightLight;

		const static Info::Elem AsyncModeAltParam = AsyncRedModeAltParam;
		const static Info::Elem FreeNRunningPingAltParam = FreeNRunningPingRedAltParam;
		const static Info::Elem SkewLimitAltParam = SkewLimitRedAltParam;
	};

	struct MappingB
	{		
		const static Info::Elem PingDivMultKnob = PingDivMultBlueKnob;
		const static Info::Elem ScaleKnob    	= ScaleBlueKnob;
		const static Info::Elem SkewKnob		= SkewBlueKnob;
		const static Info::Elem CurveKnob		= CurveBlueKnob;
		const static Info::Elem PingButton   	= PingBlueButton;
		const static Info::Elem CycleButton  	= CycleBlueButton;
		const static Info::Elem BiNPolarButton	= BiNPolarBlueButton;
		const static Info::Elem PingJackIn		= PingBlueJackIn;
		const static Info::Elem QntIn 			= QntBlueIn;
		const static Info::Elem AsyncIn			= AsyncBlueIn;
		const static Info::Elem DivJackIn		= DivBlueJackIn;
		const static Info::Elem SkewJackIn		= SkewBlueJackIn;
		const static Info::Elem CurveJackIn 	= CurveBlueJackIn;
		const static Info::Elem EnvOut       	= EnvBlueOut;
		const static Info::Elem _5VEnvOut    	= P5VEnvBlueOut;
		const static Info::Elem EoFOut       	= EofBlueOut;
		const static Info::Elem SecondaryOut    = HalfROut;
		const static Info::Elem EnvOutLight  	= EnvbluelightLight;
		const static Info::Elem EofLight     	= EofblueLight;
		const static Info::Elem SecondaryLight	= HalfriseblueLight;

		const static Info::Elem AsyncModeAltParam = AsyncBlueModeAltParam;
		const static Info::Elem FreeNRunningPingAltParam = FreeNRunningPingBlueAltParam;
		const static Info::Elem SkewLimitAltParam = SkewLimitBlueAltParam;
	};

	PEGChannel<PEGCore,MappingA> channelA;
	PEGChannel<PEGCore,MappingB> channelB;

	friend PEGChannel<PEGCore,MappingA>;
	friend PEGChannel<PEGCore,MappingB>;

	public : 
	PEGCore()
		: channelA(this)
		, channelB(this)
		, timerPhase(0)
        , timerPhaseIncrement(1.0f)
		{
		}

	void update() override {

		timerPhase += timerPhaseIncrement;
        while (timerPhase > 1.0f) {
            channelA.doDACUpdate();
			channelB.doDACUpdate();
            timerPhase -= 1.0f;
        }

		channelA.update();
		channelB.update();

		if(isPatched<ToggleIn>())
		{
			auto toggleIn = getInput<ToggleIn>();
			channelA.toggleInput(toggleIn);
			channelB.toggleInput(toggleIn);
		};
		
		switch (getState<EofRedModeAltParam>())
		{
			case 0: channelA.setMainMode(channelA.MainMode::EOF_GATE); break;
			case 1: channelA.setMainMode(channelA.MainMode::EOF_TRIG); break;
			case 2: channelA.setMainMode(channelA.MainMode::TAP_GATE); break;
			case 3: channelA.setMainMode(channelA.MainMode::TAP_TRIG); break;
		}

		switch (getState<EofBlueModeAltParam>())
		{
			case 0: channelB.setMainMode(channelB.MainMode::EOF_GATE); break;
			case 1: channelB.setMainMode(channelB.MainMode::EOF_TRIG); break;
			case 2: channelB.setMainMode(channelB.MainMode::TAP_GATE); break;
			case 3: channelB.setMainMode(channelB.MainMode::TAP_TRIG); break;
			default: return;
		}

		switch (getState<EorRedModeAltParam>())
		{
			case 0: channelA.setSecondaryMode(channelA.SecondaryMode::EOR_GATE); break;
			case 1: channelA.setSecondaryMode(channelA.SecondaryMode::EOR_TRIG); break;
			case 2: channelA.setSecondaryMode(channelA.SecondaryMode::HR_GATE); break;
			case 3: channelA.setSecondaryMode(channelA.SecondaryMode::HR_TRIG); break;
			default: return;
		}

		switch (getState<HalfNRBlueModeAltParam>())
		{
			case 0: channelB.setSecondaryMode(channelB.SecondaryMode::HR_GATE); break;
			case 1: channelB.setSecondaryMode(channelB.SecondaryMode::HR_TRIG); break;
			case 2: channelB.setSecondaryMode(channelB.SecondaryMode::EOR_GATE); break;
			case 3: channelB.setSecondaryMode(channelB.SecondaryMode::EOR_TRIG); break;
			default: return;
		}

		setOutput<OrOut>(std::max(getOutput<EnvBlueOut>(), getOutput<EnvRedOut>()));
	}

	void set_samplerate(float sr) override {
		// DAC update needs to happen at fixed rate, independent of sample rate
		timerPhaseIncrement = float(PEG::PEGBase::kDacSampleRate) / sr;
	}

	struct SaveState_t {
		bool cyclingA;
		bool cyclingB;
		uint32_t clk_timeA;
		uint32_t clk_timeB;
	};
	SaveState_t saveState;

	void load_state(std::string_view state_data) override 
	{
		auto raw_data = decode(state_data);

		std::error_code ec;
		auto newSaveState = alpaca::deserialize<alpaca::options::with_version, SaveState_t>(raw_data, ec);
		if (!ec)
		{
			saveState = newSaveState;

			channelA.peg.settings.start_clk_time = saveState.clk_timeA;
			channelA.peg.settings.start_cycle_on = saveState.cyclingA;
			channelA.peg.apply_settings();

			channelB.peg.settings.start_clk_time = saveState.clk_timeB;
			channelB.peg.settings.start_cycle_on = saveState.cyclingB;
			channelB.peg.apply_settings();
		}
	}

	std::string save_state() override 
	{
		saveState.cyclingA = channelA.peg.settings.start_cycle_on;
		saveState.clk_timeA = channelA.peg.settings.start_clk_time;

		saveState.cyclingB = channelB.peg.settings.start_cycle_on;
		saveState.clk_timeB = channelB.peg.settings.start_clk_time;

		std::vector<uint8_t> bytes;
		alpaca::serialize<alpaca::options::with_version>(saveState, bytes);
		return encode({bytes.data(), bytes.size()});
	}

private:
    float timerPhase;
    float timerPhaseIncrement;

public:
	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on
};

} // namespace MetaModule
