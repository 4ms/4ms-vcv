#include "CoreModules/4ms/info/DLD_info.hh"
//
#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "looping-delay/DLDChannel.hh"
#include <alpaca/alpaca.h>

namespace MetaModule
{

class DLDCore : public SmartCoreProcessor<DLDInfo> {
public:
	using Info = DLDInfo;
	using ThisCore = DLDCore;
	using enum Info::Elem;

	template<Info::Elem EL>
	void setOutput(auto val)
	{
		return SmartCoreProcessor<Info>::setOutput<EL>(val);
	}

	template<Info::Elem EL>
	auto getInput()
	{
		return SmartCoreProcessor<Info>::getInput<EL>();
	}

	template<Info::Elem EL, typename VAL>
	void setLED(const VAL &value)
	{
		return SmartCoreProcessor<Info>::setLED<EL>(value);
	}

	template<Info::Elem EL>
	auto getState()
	{
		return SmartCoreProcessor<Info>::getState<EL>();
	}

private:
	struct MappingA
	{
		const static Info::Elem TimeKnob              = TimeAKnob;
		const static Info::Elem FeedbackKnob          = FeedbackAKnob;
		const static Info::Elem DelayFeedKnob         = DelayFeedAKnob;
		const static Info::Elem MixKnob               = MixAKnob;
		const static Info::Elem ReverseButton         = ReverseAButton;
		const static Info::Elem HoldButton            = HoldAButton;
		const static Info::Elem ReturnInput           = ReturnAIn;
		const static Info::Elem SendOutput            = SendAOut;
		const static Info::Elem ReverseInput          = ReverseAJackIn;
		const static Info::Elem HoldInput             = HoldAJackIn;
		const static Info::Elem TimeInput             = TimeAJackIn;
		const static Info::Elem FeedbackInput         = FeedbackAJackIn;
		const static Info::Elem DelayFeedInput        = DelayFeedAJackIn;
		const static Info::Elem LoopOutput            = LoopAOut;
		const static Info::Elem TimeLight             = TimeALedLight;
		const static Info::Elem TimeModSwitch         = TimeMultASwitch;
		const static Info::Elem SoftClipEnabledAlt    = SoftClipAAltParam;
		const static Info::Elem AutoMuteEnabledAlt    = AutomuteAAltParam;
		const static Info::Elem CrossFadeTimeAlt      = CrossfadeAAltParam;
		const static Info::Elem DelayFeedTaperModeAlt = DelayfeedATaperAltParam;
		const static Info::Elem HoldInputModeAlt      = HoldAAltParam;
		const static Info::Elem ReverseInputModeAlt   = Rev_AAltParam;
		const static Info::Elem TimeUnquantizedAlt    = TimeAAutoNUnqAltParam;
		const static Info::Elem StereoModeAlt         = StereoModeAAltParam;
		const static Info::Elem PingMethodAlt         = PingMethodAltParam;
	};

	struct MappingB
	{
		const static Info::Elem TimeKnob              = TimeBKnob;
		const static Info::Elem FeedbackKnob          = FeedbackBKnob;
		const static Info::Elem DelayFeedKnob         = DelayFeedBKnob;
		const static Info::Elem MixKnob               = MixBKnob;
		const static Info::Elem ReverseButton         = ReverseBButton;
		const static Info::Elem HoldButton            = HoldBButton;

		const static Info::Elem ReturnInput           = ReturnBIn;
		const static Info::Elem SendOutput            = SendBOut;
		const static Info::Elem ReverseInput          = ReverseBJackIn;
		const static Info::Elem HoldInput             = HoldBJackIn;
		const static Info::Elem TimeInput             = TimeBJackIn;
		const static Info::Elem FeedbackInput         = FeedbackBJackIn;
		const static Info::Elem DelayFeedInput        = DelayFeedBJackIn;
		const static Info::Elem LoopOutput            = LoopBOut;
		const static Info::Elem TimeLight             = TimeBLedLight;
		const static Info::Elem TimeModSwitch         = TimeMultBSwitch;
		const static Info::Elem SoftClipEnabledAlt    = SoftClipBAltParam;
		const static Info::Elem AutoMuteEnabledAlt    = AutomuteBAltParam;
		const static Info::Elem CrossFadeTimeAlt      = CrossfadeBAltParam;
		const static Info::Elem DelayFeedTaperModeAlt = DelayfeedBTaperAltParam;
		const static Info::Elem HoldInputModeAlt      = HoldBAltParam;
		const static Info::Elem ReverseInputModeAlt   = Rev_BAltParam;
		const static Info::Elem TimeUnquantizedAlt    = TimeBAutoNUnqAltParam;
		const static Info::Elem StereoModeAlt         = StereoModeBAltParam;
		const static Info::Elem PingMethodAlt         = PingMethodAltParam;
	};	

	DLDChannel<DLDCore, MappingA> channelA;
	DLDChannel<DLDCore, MappingB> channelB;

	friend DLDChannel<DLDCore, MappingA>;
	friend DLDChannel<DLDCore, MappingB>;

public:
	DLDCore()
		: channelA(this), channelB(this)
	{
	}

	void update() override {

		auto pingButton = getState<PingButton>() == MomentaryButton::State_t::PRESSED;
		auto pingJack = getInput<PingJackIn>().value_or(0) > TriggerThresholdInVolt;

		channelA.io.pingButtonIn = pingButton;
		channelA.io.pingJackIn = pingJack;

		channelB.io.pingButtonIn = pingButton;
		channelB.io.pingJackIn = pingJack;

		// Input normalization
		channelA.io.audioIn = getInput<InAIn>().value_or(0);
		channelB.io.audioIn = isPatched<InBIn>() ? getInput<InBIn>().value_or(0) : channelA.io.audioIn; 

		channelA.update();
		channelB.update();

		// Output normalization
		setOutput<OutBOut>(channelB.io.audioOut);
		setOutput<OutAOut>(isPatched<OutBOut>() ? channelA.io.audioOut : (channelA.io.audioOut + channelB.io.audioOut) / 2);

		// only map channel B to common controls
		setLED<PingButton>(channelB.io.pingOut);
		setOutput<ClockOut>(channelB.io.clockOut ? ClockOuputFullScaleInVolt : 0.0f);
	}

	void set_samplerate(float sr) override {
		channelA.set_samplerate(sr);
		channelB.set_samplerate(sr);
	}

	struct SaveState_t
	{
		uint32_t ping_time;
	};
	SaveState_t saveState;

	void load_state(std::string_view state_data) override 
	{
		auto raw_data = decode(state_data);

		std::error_code ec;
		auto newSaveState = alpaca::deserialize<alpaca::options::with_version, SaveState_t>(raw_data, ec);
		if (!ec)
		{
			// store current state so it can be applied later
			saveState = newSaveState;
			applySaveState();
		}
	}

	std::string save_state() override 
	{
		populateSaveState();

		std::vector<uint8_t> bytes;
		alpaca::serialize<alpaca::options::with_version>(saveState, bytes);

		return encode({bytes.data(), bytes.size()});
	}

	void applySaveState() 
	{
		channelA.set_ping_time(saveState.ping_time);
		channelB.set_ping_time(saveState.ping_time);
	}

	void populateSaveState()
	{
		saveState.ping_time = channelA.get_ping_time();
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	static constexpr float TriggerThresholdInVolt    = 0.1f;
	static constexpr float ClockOuputFullScaleInVolt = 5.0f;
};

} // namespace MetaModule
