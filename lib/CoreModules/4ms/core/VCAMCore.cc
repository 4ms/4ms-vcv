#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"
#include "info/VCAM_info.hh"
#include "vcam/vcam_channel.h"

namespace MetaModule
{

class VCAMCore : public SmartCoreProcessor<VCAMInfo> {
	using Info = VCAMInfo;
	using ThisCore = VCAMCore;
	using enum Info::Elem;

public:
	VCAMCore() = default;

	void update() override {
		channelA1.pot(getState<A1LevelKnob>());
		channelA2.pot(getState<A2LevelKnob>());
		channelA3.pot(getState<A3LevelKnob>());
		channelA4.pot(getState<A4LevelKnob>());

		channelB1.pot(getState<B1LevelKnob>());
		channelB2.pot(getState<B2LevelKnob>());
		channelB3.pot(getState<B3LevelKnob>());
		channelB4.pot(getState<B4LevelKnob>());

		channelC1.pot(getState<C1LevelKnob>());
		channelC2.pot(getState<C2LevelKnob>());
		channelC3.pot(getState<C3LevelKnob>());
		channelC4.pot(getState<C4LevelKnob>());

		channelD1.pot(getState<D1LevelKnob>());
		channelD2.pot(getState<D2LevelKnob>());
		channelD3.pot(getState<D3LevelKnob>());
		channelD4.pot(getState<D4LevelKnob>());

		if (auto control = getInput<A1JackIn>(); control) {
			channelA1.control(*control);
		} else {
			channelA1.control(5.f);
		}
		if (auto control = getInput<A2JackIn>(); control) {
			channelA2.control(*control);
		} else {
			channelA2.control(5.f);
		}
		if (auto control = getInput<A3JackIn>(); control) {
			channelA3.control(*control);
		} else {
			channelA3.control(5.f);
		}
		if (auto control = getInput<A4JackIn>(); control) {
			channelA4.control(*control);
		} else {
			channelA4.control(5.f);
		}

		if (auto control = getInput<B1JackIn>(); control) {
			channelB1.control(*control);
		} else {
			channelB1.control(5.f);
		}
		if (auto control = getInput<B2JackIn>(); control) {
			channelB2.control(*control);
		} else {
			channelB2.control(5.f);
		}
		if (auto control = getInput<B3JackIn>(); control) {
			channelB3.control(*control);
		} else {
			channelB3.control(5.f);
		}
		if (auto control = getInput<B4JackIn>(); control) {
			channelB4.control(*control);
		} else {
			channelB4.control(5.f);
		}

		if (auto control = getInput<C1JackIn>(); control) {
			channelC1.control(*control);
		} else {
			channelC1.control(5.f);
		}
		if (auto control = getInput<C2JackIn>(); control) {
			channelC2.control(*control);
		} else {
			channelC2.control(5.f);
		}
		if (auto control = getInput<C3JackIn>(); control) {
			channelC3.control(*control);
		} else {
			channelC3.control(5.f);
		}
		if (auto control = getInput<C4JackIn>(); control) {
			channelC4.control(*control);
		} else {
			channelC4.control(5.f);
		}

		if (auto control = getInput<D1JackIn>(); control) {
			channelD1.control(*control);
		} else {
			channelD1.control(5.f);
		}
		if (auto control = getInput<D2JackIn>(); control) {
			channelD2.control(*control);
		} else {
			channelD2.control(5.f);
		}
		if (auto control = getInput<D3JackIn>(); control) {
			channelD3.control(*control);
		} else {
			channelD3.control(5.f);
		}
		if (auto control = getInput<D4JackIn>(); control) {
			channelD4.control(*control);
		} else {
			channelD4.control(5.f);
		}

		channelA1.mute(getState<A1Button>() == LatchingButton::State_t::UP);
		channelA2.mute(getState<A2Button>() == LatchingButton::State_t::UP);
		channelA3.mute(getState<A3Button>() == LatchingButton::State_t::UP);
		channelA4.mute(getState<A4Button>() == LatchingButton::State_t::UP);

		channelB1.mute(getState<B1Button>() == LatchingButton::State_t::UP);
		channelB2.mute(getState<B2Button>() == LatchingButton::State_t::UP);
		channelB3.mute(getState<B3Button>() == LatchingButton::State_t::UP);
		channelB4.mute(getState<B4Button>() == LatchingButton::State_t::UP);

		channelC1.mute(getState<C1Button>() == LatchingButton::State_t::UP);
		channelC2.mute(getState<C2Button>() == LatchingButton::State_t::UP);
		channelC3.mute(getState<C3Button>() == LatchingButton::State_t::UP);
		channelC4.mute(getState<C4Button>() == LatchingButton::State_t::UP);

		channelD1.mute(getState<D1Button>() == LatchingButton::State_t::UP);
		channelD2.mute(getState<D2Button>() == LatchingButton::State_t::UP);
		channelD3.mute(getState<D3Button>() == LatchingButton::State_t::UP);
		channelD4.mute(getState<D4Button>() == LatchingButton::State_t::UP);

		setLED<A1Button>(channelA1.getLEDbrightness());
		setLED<A2Button>(channelA2.getLEDbrightness());
		setLED<A3Button>(channelA3.getLEDbrightness());
		setLED<A4Button>(channelA4.getLEDbrightness());

		setLED<B1Button>(channelB1.getLEDbrightness());
		setLED<B2Button>(channelB2.getLEDbrightness());
		setLED<B3Button>(channelB3.getLEDbrightness());
		setLED<B4Button>(channelB4.getLEDbrightness());

		setLED<C1Button>(channelC1.getLEDbrightness());
		setLED<C2Button>(channelC2.getLEDbrightness());
		setLED<C3Button>(channelC3.getLEDbrightness());
		setLED<C4Button>(channelC4.getLEDbrightness());

		setLED<D1Button>(channelD1.getLEDbrightness());
		setLED<D2Button>(channelD2.getLEDbrightness());
		setLED<D3Button>(channelD3.getLEDbrightness());
		setLED<D4Button>(channelD4.getLEDbrightness());

		float output1 = 0.f;
		float output2 = 0.f;
		float output3 = 0.f;
		float output4 = 0.f;

		if (auto input = getInput<InAIn>(); input) {
			channelA1.input(*input);
			output1 += channelA1.output();

			channelA2.input(*input);
			output2 += channelA2.output();

			channelA3.input(*input);
			output3 += channelA3.output();

			channelA4.input(*input);
			output4 += channelA4.output();
		}

		if (auto input = getInput<InBIn>(); input) {
			channelB1.input(*input);
			output1 += channelB1.output();

			channelB2.input(*input);
			output2 += channelB2.output();

			channelB3.input(*input);
			output3 += channelB3.output();

			channelB4.input(*input);
			output4 += channelB4.output();
		}

		if (auto input = getInput<InCIn>(); input) {
			channelC1.input(*input);
			output1 += channelC1.output();

			channelC2.input(*input);
			output2 += channelC2.output();

			channelC3.input(*input);
			output3 += channelC3.output();

			channelC4.input(*input);
			output4 += channelC4.output();
		}

		if (auto input = getInput<InDIn>(); input) {
			channelD1.input(*input);
			output1 += channelD1.output();

			channelD2.input(*input);
			output2 += channelD2.output();

			channelD3.input(*input);
			output3 += channelD3.output();

			channelD4.input(*input);
			output4 += channelD4.output();
		}

		setOutput<Out1Out>(std::clamp(output1, -10.f, 10.f));
		setOutput<Out2Out>(std::clamp(output2, -10.f, 10.f));
		setOutput<Out3Out>(std::clamp(output3, -10.f, 10.f));
		setOutput<Out4Out>(std::clamp(output4, -10.f, 10.f));
	}

	void set_samplerate(float sr) override {
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	Channel channelA1;
	Channel channelA2;
	Channel channelA3;
	Channel channelA4;

	Channel channelB1;
	Channel channelB2;
	Channel channelB3;
	Channel channelB4;

	Channel channelC1;
	Channel channelC2;
	Channel channelC3;
	Channel channelC4;

	Channel channelD1;
	Channel channelD2;
	Channel channelD3;
	Channel channelD4;
};

} // namespace MetaModule
