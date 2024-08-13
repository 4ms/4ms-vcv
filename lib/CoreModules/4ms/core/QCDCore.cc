#include "CoreModules/4ms/info/QCD_info.hh"
#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"

#include "CoreModules/4ms/core/helpers/EdgeDetector.h"
#include "CoreModules/4ms/core/helpers/FlipFlop.h"

namespace MetaModule
{


template <class Parent, class Mapping>
class QCDChannel
{
private:
	template<typename Parent::Info::Elem EL>
	void setOutput(auto val)
	{
		return parent->template setOutput<EL>(val);
	}

	template<typename Parent::Info::Elem EL>
	auto getInput()
	{
		return parent->template getInput<EL>();
	}

	template<typename Parent::Info::Elem EL, typename VAL>
	void setLED(const VAL &value)
	{
		return parent->template setLED<EL>(value);
	}

	template<typename Parent::Info::Elem EL>
	auto getState()
	{
		return parent->template getState<EL>();
	}

private: 	
	enum operation_t {MULT, DIV};

	struct factorType_t
	{
		operation_t operation;
		uint32_t factor;
		uint32_t index;

		inline bool operator!=(factorType_t a) {
		if (a.operation == operation && a.factor == factor)
			return false;
		else
			return true;
		}
	};

	enum invMode_t {DELAY, INVERTED, SHUFFLE};
	enum edge_t {FALLING, RISING};
	enum outputState_t {LOW, HIGH};

private:
	Parent* parent;

public:
	QCDChannel(Parent* parent_)
		: parent(parent_), ticks(0), clockDivCounter({0}), invMode(DELAY), clockOutState(LOW), phase(0.0f), processSyncPulse(false), clockOutRisingEdgeCounter(0), triggerDetectorClock(1.0f, 2.0f), triggerDetectorReset(1.0f, 2.0f) {
			set_samplerate(48000.f);
	}

	void update(auto clockInput) {
		auto now = ++ticks;

		if (auto newFactor = readFactorCV(); newFactor != factor) {
			factor = newFactor;
			calculateClockOutPeriod(factor);
		}

		pulsewidth = readPulsewidthCV();
		triggerLengthInTicks = calculateTriggerlength(pulsewidth);
		invMode = readInvMode();

		if (clockInput) {
			if (triggerEdgeDetectorClock(triggerDetectorClock(*clockInput))) {
				calculateClockInPeriod(now);
				calculateClockOutPeriod(now, factor);
			}
		} else {
			resetClocks(now);
		}		

		if(clockIn.lastEventInTicks && clockIn.periodInTicks) {
			if(auto newPhase = processResetIn(now); newPhase) {
				phase = *newPhase;
			}

			uint32_t phaseOffset = uint32_t(std::round(*clockIn.periodInTicks * phase));

			if (now == *clockIn.lastEventInTicks + phaseOffset) {

				for (uint32_t index = 0; index < clockDivCounter.size(); index++) {
					
					clockDivCounter[index]++;

					if(clockDivCounter[index] >= clockFactor[index].factor) {
						clockDivCounter[index] = 0;
					}
				}

				if(factor.operation == DIV) {

					if(clockDivCounter[factor.index] == 0)
					{
						processSyncPulse = true;
					}
				} else {
					processSyncPulse = true;
				}
			}
		}

		updateClockOut(now, factor);
		updateInvOut(now);
	}

	void set_samplerate(float sr) {
		timeStepInS = 1.f /sr;

		triggerLength5msInTicks = uint32_t(std::round(0.005f / timeStepInS));
		triggerLengthMinimumInTicks = 2;
	}

private:
	factorType_t readFactorCV() {
		auto selector = getState<Mapping::DivMultKnob>();

		if(auto selectorCV = getInput<Mapping::DivMultCvJackIn>(); selectorCV) {
			selector += (getState<Mapping::DivMultCvKnob>() * 2.0f - 1.0f) * *selectorCV / 10.0f;
			selector = std::clamp(selector, 0.0f, 1.0f);
		}

		return clockFactor[uint32_t(std::round(selector * (clockFactor.size() - 1)))];
	}

	float readPulsewidthCV() {
		auto pulsewidth = getState<Mapping::GatePwKnob>();

		if(auto pulsewidthCV = getInput<Mapping::GatePwCvIn>(); pulsewidthCV) {
			pulsewidth += (getState<Mapping::GatePwAttKnob>() * 2.0f - 1.0f) * *pulsewidthCV / 10.0f;
			pulsewidth = std::clamp(pulsewidth, 0.0f, 1.0f);
		}

		return pulsewidth;
	}

	invMode_t readInvMode() {
		auto invMode = getState<Mapping::InvModeSwitch>();

		if(invMode == Toggle3posHoriz::State_t::LEFT) {
			return DELAY;
		} else if(invMode == Toggle3posHoriz::State_t::CENTER) {
			return INVERTED;
		} else {
			return SHUFFLE;
		}
	}

	uint32_t calculateTriggerlength(float pulsewidth) {
		if(clockOut.periodInTicks) {
			if (1/(timeStepInS * *clockOut.periodInTicks) < 100) {
				return triggerLength5msInTicks + uint32_t(std::round(pulsewidth * (*clockOut.periodInTicks - 2 * triggerLength5msInTicks)));
			} else {
				return triggerLengthMinimumInTicks + uint32_t(std::round(pulsewidth * (*clockOut.periodInTicks - 2 * triggerLengthMinimumInTicks)));
			}
		} else {
			return triggerLengthMinimumInTicks;
		}
	}

	void calculateClockInPeriod(uint32_t timestampInTicks) {
		if(clockIn.lastEventInTicks) {
			if(clockIn.lastEventInTicks < timestampInTicks) {
				clockIn.periodInTicks = timestampInTicks - *clockIn.lastEventInTicks;
			} else { //overflow
				clockIn.periodInTicks = std::numeric_limits<decltype(ticks)>::max() - *clockIn.lastEventInTicks;
				clockIn.periodInTicks = *clockIn.periodInTicks + timestampInTicks;
			}
		}

		clockIn.lastEventInTicks = timestampInTicks;
	}

	void calculateClockOutPeriod(factorType_t factor) {
		if(clockIn.periodInTicks) {
			if(factor.operation == MULT) {
				clockOut.periodInTicks = *clockIn.periodInTicks / factor.factor;
			} else if (factor.operation == DIV){
				clockOut.periodInTicks = *clockIn.periodInTicks * factor.factor;
			} else {
				clockOut.periodInTicks.reset();
			}
		}
	}

	void calculateClockOutPeriod(uint32_t timestampInTicks, factorType_t factor) {
		if(clockIn.periodInTicks) {
			if(factor.operation == MULT) {
				clockOut.periodInTicks = *clockIn.periodInTicks / factor.factor;
			} else if (factor.operation == DIV){
				clockOut.periodInTicks = *clockIn.periodInTicks * factor.factor;
			} else {
				clockOut.periodInTicks.reset();
			}

			if (!clockOut.lastEventInTicks) {
				clockOut.lastEventInTicks = timestampInTicks;
			}
		}
	}

	void updateClockOut(uint32_t timestampInTicks, factorType_t factor) {
		if(clockOut.periodInTicks) {
			if(clockOutIsLow()) {
				if (processSyncPulse == true) {
					clockOutRisingEdgeCounter = 0;

					setClockOutRisingEdge(timestampInTicks);

					processSyncPulse = false;
				} else if (factor.operation == MULT) {
					if(timestampInTicks >= (*clockOut.lastEventInTicks + *clockOut.periodInTicks) && clockOutRisingEdgeCounter < factor.factor) {
						setClockOutRisingEdge(timestampInTicks);
					}
				}
			} else {
				if (timestampInTicks >= (*clockOut.lastEventInTicks + triggerLengthInTicks)) {
					setClockOutFallingEdge(timestampInTicks);
				}
			}
		}
	}

	bool clockOutIsLow() {
		return clockOutState == LOW;
	}

	void setClockOutRisingEdge(uint32_t timestampInTicks) {
		if(clockOutState == LOW) {
			setOutput<Mapping::OutOut>(outputHighVoltageLevel);
			setLED<Mapping::OutLight>(1.f);
			updateInvOut(timestampInTicks, RISING);

			clockOut.lastEventInTicks = timestampInTicks;

			clockOutRisingEdgeCounter++;
			clockOutState = HIGH;
		}
	}

	void setClockOutFallingEdge(uint32_t timestampInTicks) {
		if(clockOutState == HIGH) {
			setOutput<Mapping::OutOut>(0.f);
			setLED<Mapping::OutLight>(0.f);
			updateInvOut(timestampInTicks, FALLING);

			clockOutState = LOW;
		}
	}

	void updateInvOut(uint32_t timestamp) {
		if ((invMode == DELAY) || (invMode == SHUFFLE)) {
			if(clockInvOut.lastEventInTicks) {
				auto invOutTriggerLength = triggerLengthMinimumInTicks;

				if (1/(timeStepInS * *clockOut.periodInTicks) < 100) {
					invOutTriggerLength = triggerLength5msInTicks;
				} else {
					invOutTriggerLength = uint32_t(std::round(*clockOut.periodInTicks * 0.3f));
				}

				if(invOutTriggerLength < triggerLengthMinimumInTicks) {
					invOutTriggerLength = triggerLengthMinimumInTicks;
				}

				if(timestamp >= (*clockInvOut.lastEventInTicks + invOutTriggerLength)) {
					setOutput<Mapping::InvOutOut>(0.f);
					setLED<Mapping::InvLight>(0.f);
					clockInvOut.lastEventInTicks.reset();
				}
			}
		}
	}

	void updateInvOut(uint32_t timestamp, edge_t edgeType) {
		if(invMode == INVERTED) {
			if(edgeType == RISING) {
				setOutput<Mapping::InvOutOut>(0.f);
				setLED<Mapping::InvLight>(0.f);
				clockInvOut.lastEventInTicks.reset();
			} else {
				setOutput<Mapping::InvOutOut>(outputHighVoltageLevel);
				setLED<Mapping::InvLight>(1.f);
				clockInvOut.lastEventInTicks = timestamp;
			}
		} else if(invMode == DELAY) {
			if(edgeType == FALLING) {
				setOutput<Mapping::InvOutOut>(outputHighVoltageLevel);
				setLED<Mapping::InvLight>(1.f);
				clockInvOut.lastEventInTicks = timestamp;
			}
		} else if(invMode == SHUFFLE) {
			setOutput<Mapping::InvOutOut>(outputHighVoltageLevel);
			setLED<Mapping::InvLight>(1.f);
			clockInvOut.lastEventInTicks = timestamp;
		}
	}

	void resetClocks(uint32_t timestampInTicks) {
		clockIn.lastEventInTicks.reset();
		clockIn.periodInTicks.reset();

		clockOut.lastEventInTicks.reset();
		clockOut.periodInTicks.reset();

		if(clockOutState == HIGH) {
			setOutput<Mapping::OutOut>(0.f);
			setLED<Mapping::OutLight>(0.f);
			updateInvOut(timestampInTicks, FALLING);
			clockOutState = LOW;
		}
	}

	std::optional<float> processResetIn(uint32_t timestamp) {
		if (auto input = getInput<Mapping::ResetIn>(); input) {
			if (triggerEdgeDetectorReset(triggerDetectorReset(*input))) {
				if(clockIn.periodInTicks) {
					uint32_t ticksSinceLastEvent = timestamp - *clockIn.lastEventInTicks;
					float phase = float(ticksSinceLastEvent) / float(*clockIn.periodInTicks);

					return phase;
				}
			}
		}

		return std::nullopt;
	}

private:
	static constexpr float outputHighVoltageLevel = 5.0f;
	static constexpr float triggerLengthMinimumInS = 0.005f;

	static constexpr uint32_t maxIndexDivisions = 9;
	static constexpr std::array<factorType_t,21> clockFactor = {{
		{DIV, 32, 0},
		{DIV, 32, 1},
		{DIV, 16, 2},
		{DIV, 8, 3},
		{DIV, 7, 4},
		{DIV, 6, 5},
		{DIV, 5, 6},
		{DIV, 4, 7},
		{DIV, 3, 8},
		{DIV, 2, 9},
		{MULT, 1, 10},
		{MULT, 2, 11},
		{MULT, 3, 12},
		{MULT, 4, 13},
		{MULT, 5, 14},
		{MULT, 6, 15},
		{MULT, 7, 16},
		{MULT, 8, 17},
		{MULT, 12, 18},
		{MULT, 16, 19},
		{MULT, 16, 20}
	}};

private:
	float timeStepInS;
	uint32_t triggerLengthMinimumInTicks;
	uint32_t triggerLength5msInTicks;
	uint32_t triggerLengthInTicks;
	uint32_t ticks;
	std::array<uint32_t, maxIndexDivisions + 1> clockDivCounter;
	factorType_t factor;
	float pulsewidth;
	invMode_t invMode;
	outputState_t clockOutState;
	float phase;
	bool processSyncPulse;
	uint32_t clockOutRisingEdgeCounter;

	struct clockMeasures
	{
		std::optional<uint32_t> lastEventInTicks;
		std::optional<uint32_t> periodInTicks;
	};

	clockMeasures clockIn;
	clockMeasures clockOut;
	clockMeasures clockInvOut;

private:
	FlipFlop triggerDetectorClock;
	EdgeDetector triggerEdgeDetectorClock;

	FlipFlop triggerDetectorReset;
	EdgeDetector triggerEdgeDetectorReset;
};


//////////////////////////////////////////////////////////////

class QCDCore : public SmartCoreProcessor<QCDInfo> {
	using ThisCore = QCDCore;

public:
	using Info = QCDInfo;
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
		const static Info::Elem GatePwAttKnob = GatePwAtt1Knob;
		const static Info::Elem GatePwKnob = GatePw1Knob;
		const static Info::Elem DivMultCvKnob = DivMultCv1Knob;
		const static Info::Elem DivMultKnob = DivMult1Knob;
		const static Info::Elem InvModeSwitch = InvMode1Switch;
		const static Info::Elem GatePwCvIn = GatePwCv1In;
		const static Info::Elem DivMultCvJackIn = DivMultCv1JackIn;
		const static Info::Elem ClkInIn = ClkIn1In;
		const static Info::Elem ResetIn = Reset1In;
		const static Info::Elem InvOutOut = InvOut1Out;
		const static Info::Elem OutOut = Out1Out;
		const static Info::Elem InvLight = Inv1Light;
		const static Info::Elem OutLight = Out1Light;
	};

	struct MappingB
	{
		const static Info::Elem GatePwAttKnob = GatePwAtt2Knob;
		const static Info::Elem GatePwKnob = GatePw2Knob;
		const static Info::Elem DivMultCvKnob = DivMultCv2Knob;
		const static Info::Elem DivMultKnob = DivMult2Knob;
		const static Info::Elem InvModeSwitch = InvMode2Switch;
		const static Info::Elem GatePwCvIn = GatePwCv2In;
		const static Info::Elem DivMultCvJackIn = DivMultCv2JackIn;
		const static Info::Elem ClkInIn = ClkIn2In;
		const static Info::Elem ResetIn = Reset2In;
		const static Info::Elem InvOutOut = InvOut2Out;
		const static Info::Elem OutOut = Out2Out;
		const static Info::Elem InvLight = Inv2Light;
		const static Info::Elem OutLight = Out2Light;
	};

	struct MappingC
	{
		const static Info::Elem GatePwAttKnob = GatePwAtt3Knob;
		const static Info::Elem GatePwKnob = GatePw3Knob;
		const static Info::Elem DivMultCvKnob = DivMultCv3Knob;
		const static Info::Elem DivMultKnob = DivMult3Knob;
		const static Info::Elem InvModeSwitch = InvMode3Switch;
		const static Info::Elem GatePwCvIn = GatePwCv3In;
		const static Info::Elem DivMultCvJackIn = DivMultCv3JackIn;
		const static Info::Elem ClkInIn = ClkIn3In;
		const static Info::Elem ResetIn = Reset3In;
		const static Info::Elem InvOutOut = InvOut3Out;
		const static Info::Elem OutOut = Out3Out;
		const static Info::Elem InvLight = Inv3Light;
		const static Info::Elem OutLight = Out3Light;
	};

	struct MappingD
	{
		const static Info::Elem GatePwAttKnob = GatePwAtt4Knob;
		const static Info::Elem GatePwKnob = GatePw4Knob;
		const static Info::Elem DivMultCvKnob = DivMultCv4Knob;
		const static Info::Elem DivMultKnob = DivMult4Knob;
		const static Info::Elem InvModeSwitch = InvMode4Switch;
		const static Info::Elem GatePwCvIn = GatePwCv4In;
		const static Info::Elem DivMultCvJackIn = DivMultCv4JackIn;
		const static Info::Elem ClkInIn = ClkIn4In;
		const static Info::Elem ResetIn = Reset4In;
		const static Info::Elem InvOutOut = InvOut4Out;
		const static Info::Elem OutOut = Out4Out;
		const static Info::Elem InvLight = Inv4Light;
		const static Info::Elem OutLight = Out4Light;
	};

	QCDChannel<QCDCore, MappingA> channelA;
	QCDChannel<QCDCore, MappingB> channelB;
	QCDChannel<QCDCore, MappingC> channelC;
	QCDChannel<QCDCore, MappingD> channelD;

	friend QCDChannel<QCDCore, MappingA>;
	friend QCDChannel<QCDCore, MappingB>;
	friend QCDChannel<QCDCore, MappingC>;
	friend QCDChannel<QCDCore, MappingD>;

public:
	QCDCore()
		: channelA(this), channelB(this), channelC(this), channelD(this), ticks(0), lastTap(0), tapPeriod(0), lastTapOut(0), ticksTapPressed(0), lastTapButtonState(MomentaryButton::State_t::RELEASED), triggerDetectorTap(1.0f, 2.0f){
		set_samplerate(48000.f);
	}

	void update() override {
		auto tapClock = tapOut();

		auto clockInA = getInput<MappingA::ClkInIn>();
		auto clockInB = getInput<MappingB::ClkInIn>();
		auto clockInC = getInput<MappingC::ClkInIn>();
		auto clockInD = getInput<MappingD::ClkInIn>();

		std::optional<float> clockToA = tapClock;
		std::optional<float> clockToB = tapClock;
		std::optional<float> clockToC = tapClock;
		std::optional<float> clockToD = tapClock;

		if(clockInA) {
			clockToA = clockInA;
			clockToB = clockInA;
			clockToC = clockInA;
			clockToD = clockInA;
		}

		if(clockInB) {
			clockToB = clockInB;
			clockToC = clockInB;
			clockToD = clockInB;
		}

		if(clockInC) {
			clockToC = clockInC;
			clockToD = clockInC;
		}

		if(clockInD) {
			clockToD = clockInD;
		}
		
		channelA.update(clockToA);
		channelB.update(clockToB);
		channelC.update(clockToC);
		channelD.update(clockToD);
	}

	void set_samplerate(float sr) override {
		timeStepInS = 1.f /sr;

		ticksToResetTap = 1.f / timeStepInS;

		channelA.set_samplerate(sr);
		channelB.set_samplerate(sr);
		channelC.set_samplerate(sr);
		channelD.set_samplerate(sr);
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	float tapOut() {
		auto now = ++ticks;

		auto tapButton = getState<TapTempoButton>();

		tapOutTiming(now, tapButton);

		return updateTapOut(now, tapButton);
	}

	void tapOutTiming(auto now, auto tapButtonState) {
		if(tapButtonState == MomentaryButton::State_t::PRESSED && lastTapButtonState == MomentaryButton::State_t::RELEASED) {
			tapPeriod = now - lastTap;
			lastTap = now;
			lastTapOut = now;
		} else if (tapButtonState == MomentaryButton::State_t::PRESSED && lastTapButtonState == MomentaryButton::State_t::PRESSED) {
			++ticksTapPressed;

			if(ticksTapPressed >= ticksToResetTap) {
				tapPeriod = 0;
			}
		} else {
			ticksTapPressed = 0;
		}

		lastTapButtonState = tapButtonState;
	}

	float updateTapOut(auto now, auto tapButtonState) {
		bool clockOut;

		if(tapPeriod != 0) {
			if(tapButtonState == MomentaryButton::State_t::PRESSED) {
				clockOut = true;
			}else{
				if(now >= lastTapOut + tapPeriod) {
					clockOut = true;
					lastTapOut = now;
				} else if (now >= lastTapOut + (tapPeriod >> 1)) {
					clockOut = false;
				} else {
					clockOut = true;
				}
			}		
		} else {
			clockOut = false;
		}

		float tapOutVoltage;

		if(clockOut == true) {
			tapOutVoltage = 10.f;
			setLED<Info::Elem::TapTempoButton>(1.f);
		} else {
			tapOutVoltage = 0.f;
			setLED<Info::Elem::TapTempoButton>(0.f);
		}

		setOutput<Info::Elem::TapOut>(tapOutVoltage);

		return tapOutVoltage;
	}

private:
	float timeStepInS; 
	uint32_t ticks;
	uint32_t lastTap;
	uint32_t tapPeriod;
	uint32_t lastTapOut;
	uint32_t ticksTapPressed;
	uint32_t ticksToResetTap;
	MomentaryButton::State_t lastTapButtonState;


private:
	FlipFlop triggerDetectorTap;
	EdgeDetector triggerEdgeDetectorTap;

};

} // namespace MetaModule
