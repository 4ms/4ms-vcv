#include "info/RCD_info.hh"
#include "CoreModules/SmartCoreProcessor.hh"
#include "CoreModules/moduleFactory.hh"

#include "CoreModules/4ms/core/helpers/EdgeDetector.h"
#include "CoreModules/4ms/core/helpers/FlipFlop.h"

namespace MetaModule
{

class RCDCore : public SmartCoreProcessor<RCDInfo> {
	using Info = RCDInfo;
	using ThisCore = RCDCore;
	using enum Info::Elem;

public:
	RCDCore()
		: clockTriggerDetector(1.0f, 2.0f), resetTriggerDetector(1.0f, 2.0f), adc(0), autoreset(0), divideByMax(0) {
	}

	void update() override {
		readSwitches();
		readRotate();

		if (auto resetInputValue = getInput<ResetIn>(); resetInputValue) {
			if (resetTriggerEdgeDetector(resetTriggerDetector(*resetInputValue))) {
				reset();
			}
		}

		recalculateDivisions();

		if (auto clockInputValue = getInput<ClkIn>(); clockInputValue) {
			const auto clockTrigger = clockTriggerDetector(*clockInputValue);
			if (clockTriggerRisingEdgeDetector(clockTrigger)) {
				processRisingClockEdge();
			} else if (clockTriggerFallingEdgeDetector(clockTrigger)) {
				processFallingClockEdge();
			}
		}
	}

	void set_samplerate(float sr) override {
	}

	// Boilerplate to auto-register in ModuleFactory
	// clang-format off
	static std::unique_ptr<CoreProcessor> create() { return std::make_unique<ThisCore>(); }
	static inline bool s_registered = ModuleFactory::registerModuleType(Info::slug, create, ModuleInfoView::makeView<Info>(), Info::png_filename);
	// clang-format on

private:
	FlipFlop clockTriggerDetector;
	EdgeDetector clockTriggerRisingEdgeDetector;
	FallingEdgeDetector clockTriggerFallingEdgeDetector;
	FlipFlop resetTriggerDetector;
	EdgeDetector resetTriggerEdgeDetector;

private:
	std::array<uint32_t, 8> divisions = {0,1,2,3,4,5,6,7}; //divX in original code
	std::array<uint32_t, 8> counters = { 0 }; //oX in original code
	uint32_t adc;
	uint32_t autoreset;
	uint32_t divideByMax;

private:
	enum Counting_t {UP, DOWN};
	Counting_t countingMode;

	enum Output_t {GATE, TRIGGER};
	Output_t outputMode;

	enum Spread_t {SPREAD_ON, SPREAD_OFF};
	Spread_t spreadMode;

	enum AutoReset_t {AUTORESET_16, AUTORESET_OFF};
	AutoReset_t autoresetMode;

	enum MaxDiv_t {MAXDIV_64, MAXDIV_8};
	MaxDiv_t maxDiv16Mode;
	MaxDiv_t maxDiv32Mode;

private:
	static constexpr float ledOn = 1.f;
	static constexpr float ledOff = 0.f;

private:
	void readSwitches(void) {
		if (getState<CountingSwitch>() == Toggle2posHoriz::State_t::LEFT) {
			countingMode = Counting_t::DOWN;
		} else {
			countingMode = Counting_t::UP;
		}

		if (getState<GateTrigSwitch>() == Toggle2posHoriz::State_t::LEFT) {
			outputMode = Output_t::GATE;
		} else {
			outputMode = Output_t::TRIGGER;
		}

		if (getState<MaxDiv1Switch>() == Toggle2posHoriz::State_t::LEFT) {
			maxDiv32Mode = MaxDiv_t::MAXDIV_64;
		} else {
			maxDiv32Mode = MaxDiv_t::MAXDIV_8;
		}

		if (getState<MaxDiv2Switch>() == Toggle2posHoriz::State_t::LEFT) {
			maxDiv16Mode = MaxDiv_t::MAXDIV_64;
		} else {
			maxDiv16Mode = MaxDiv_t::MAXDIV_8;
		}

		if (getState<SpreadSwitch>() == Toggle2posHoriz::State_t::LEFT) {
			spreadMode = Spread_t::SPREAD_ON;
		} else {
			spreadMode = Spread_t::SPREAD_OFF;
		}

		if (getState<AutoNResetSwitch>() == Toggle2posHoriz::State_t::LEFT) {
			autoresetMode = AutoReset_t::AUTORESET_16;
		} else {
			autoresetMode = AutoReset_t::AUTORESET_OFF;
		}
	}

	void readRotate() {
		if(auto rotateValue = getInput<RotateIn>(); rotateValue) {
			adc = uint32_t(std::clamp(*rotateValue, 0.f, 5.f) / 5.f * 255.f);
		} else {
			adc = 0;
		}
	}

	void reset(void) {
		counters.fill(0);
	}

	void recalculateDivisions(void) {
		uint32_t rotationAmount = 0;

		if (spreadMode == Spread_t::SPREAD_ON){
				if (maxDiv16Mode == MaxDiv_t::MAXDIV_64){
					if (maxDiv32Mode == MaxDiv_t::MAXDIV_64){
						divideByMax = 63;
						rotationAmount = adc>>2; //0..63
					} else {
						divideByMax = 15;
						rotationAmount = adc>>4; //0..15
					}
				} else {
					if (maxDiv32Mode == MaxDiv_t::MAXDIV_64){
						divideByMax = 31;
						rotationAmount = adc>>3; //0..31
					} else {
						divideByMax = 7;
						rotationAmount = adc>>5; //0..7 (/1-/8)
					}
				}
		} else {
				if (maxDiv16Mode == MaxDiv_t::MAXDIV_64){
					if (maxDiv32Mode == MaxDiv_t::MAXDIV_64){
						divideByMax = 63;
						rotationAmount = (adc>>3)+32; //31..95 masked by 63 gives 31-63,0-30 (/32-/64,/1-/31)
					} else {
						divideByMax = 15;
						rotationAmount = (adc>>4)+8; //8...23 masked by 15 gives 8-15,0-7 (/9-/16,/1-/8)
					}
				} else {
					if (maxDiv32Mode == MaxDiv_t::MAXDIV_64){
						divideByMax = 31;
						rotationAmount = (adc>>3)+16; //16..47 masked by 31 gives 16-31,0-15 (/17-/32,/1-/16)
					} else {
						divideByMax = 7;
						rotationAmount = adc>>5; //0..7 (/1-/8)
					}
				}
		}

		for (uint32_t index = 0; index < divisions.size(); index++) {
			divisions[index] = divideBy(rotationAmount, divideByMax, index, spreadMode);
		}
	}

	void processRisingClockEdge(void) {
		setLED<LedInLight>(ledOn);

		if (countingMode == Counting_t::DOWN){
			if (counters[0]==0) {setOutput<D1Out>(10.0f); setLED<LedD1Light>(ledOn);}
			if (counters[1]==0) {setOutput<D2Out>(10.0f); setLED<LedD2Light>(ledOn);}
			if (counters[2]==0) {setOutput<D3Out>(10.0f); setLED<LedD3Light>(ledOn);}
			if (counters[3]==0) {setOutput<D4Out>(10.0f); setLED<LedD4Light>(ledOn);}
			if (counters[4]==0) {setOutput<D5Out>(10.0f); setLED<LedD5Light>(ledOn);}
			if (counters[5]==0) {setOutput<D6Out>(10.0f); setLED<LedD6Light>(ledOn);}
			if (counters[6]==0) {setOutput<D7Out>(10.0f); setLED<LedD7Light>(ledOn);}
			if (counters[7]==0) {setOutput<D8Out>(10.0f); setLED<LedD8Light>(ledOn);}

			if (outputMode == Output_t::GATE){
				if (counters[0]==((divisions[0]>>1)+1)) {setOutput<D1Out>(0.f); setLED<LedD1Light>(ledOff);}
				if (counters[1]==((divisions[1]>>1)+1)) {setOutput<D2Out>(0.f); setLED<LedD2Light>(ledOff);}
				if (counters[2]==((divisions[2]>>1)+1)) {setOutput<D3Out>(0.f); setLED<LedD3Light>(ledOff);}
				if (counters[3]==((divisions[3]>>1)+1)) {setOutput<D4Out>(0.f); setLED<LedD4Light>(ledOff);}
				if (counters[4]==((divisions[4]>>1)+1)) {setOutput<D5Out>(0.f); setLED<LedD5Light>(ledOff);}
				if (counters[5]==((divisions[5]>>1)+1)) {setOutput<D6Out>(0.f); setLED<LedD6Light>(ledOff);}
				if (counters[6]==((divisions[6]>>1)+1)) {setOutput<D7Out>(0.f); setLED<LedD7Light>(ledOff);}
				if (counters[7]==((divisions[7]>>1)+1)) {setOutput<D8Out>(0.f); setLED<LedD8Light>(ledOff);}
			} else {
				if (++counters[0]>divisions[0]) counters[0]=0;
				if (++counters[1]>divisions[1]) counters[1]=0;
				if (++counters[2]>divisions[2]) counters[2]=0;
				if (++counters[3]>divisions[3]) counters[3]=0;
				if (++counters[4]>divisions[4]) counters[4]=0;
				if (++counters[5]>divisions[5]) counters[5]=0;
				if (++counters[6]>divisions[6]) counters[6]=0;
				if (++counters[7]>divisions[7]) counters[7]=0;
			}
		} else { //DOWNBEAT is off (upbeat)
			if (outputMode == Output_t::GATE){
				if (counters[0]==((divisions[0]>>1)+1)) {setOutput<D1Out>(10.f); setLED<LedD1Light>(ledOn);}
				if (counters[1]==((divisions[1]>>1)+1)) {setOutput<D2Out>(10.f); setLED<LedD2Light>(ledOn);}
				if (counters[2]==((divisions[2]>>1)+1)) {setOutput<D3Out>(10.f); setLED<LedD3Light>(ledOn);}
				if (counters[3]==((divisions[3]>>1)+1)) {setOutput<D4Out>(10.f); setLED<LedD4Light>(ledOn);}
				if (counters[4]==((divisions[4]>>1)+1)) {setOutput<D5Out>(10.f); setLED<LedD5Light>(ledOn);}
				if (counters[5]==((divisions[5]>>1)+1)) {setOutput<D6Out>(10.f); setLED<LedD6Light>(ledOn);}
				if (counters[6]==((divisions[6]>>1)+1)) {setOutput<D7Out>(10.f); setLED<LedD7Light>(ledOn);}
				if (counters[7]==((divisions[7]>>1)+1)) {setOutput<D8Out>(10.f); setLED<LedD8Light>(ledOn);}

				if (counters[0]==0) {setOutput<D1Out>(0.0f); setLED<LedD1Light>(ledOff);}
				if (counters[1]==0) {setOutput<D2Out>(0.0f); setLED<LedD2Light>(ledOff);}
				if (counters[2]==0) {setOutput<D3Out>(0.0f); setLED<LedD3Light>(ledOff);}
				if (counters[3]==0) {setOutput<D4Out>(0.0f); setLED<LedD4Light>(ledOff);}
				if (counters[4]==0) {setOutput<D5Out>(0.0f); setLED<LedD5Light>(ledOff);}
				if (counters[5]==0) {setOutput<D6Out>(0.0f); setLED<LedD6Light>(ledOff);}
				if (counters[6]==0) {setOutput<D7Out>(0.0f); setLED<LedD7Light>(ledOff);}
				if (counters[7]==0) {setOutput<D8Out>(0.0f); setLED<LedD8Light>(ledOff);}
			} else {
				if (++counters[0]>divisions[0]) {counters[0]=0; setOutput<D1Out>(10.f); setLED<LedD1Light>(ledOn);}
				if (++counters[1]>divisions[1]) {counters[1]=0; setOutput<D2Out>(10.f); setLED<LedD2Light>(ledOn);}
				if (++counters[2]>divisions[2]) {counters[2]=0; setOutput<D3Out>(10.f); setLED<LedD3Light>(ledOn);}
				if (++counters[3]>divisions[3]) {counters[3]=0; setOutput<D4Out>(10.f); setLED<LedD4Light>(ledOn);}
				if (++counters[4]>divisions[4]) {counters[4]=0; setOutput<D5Out>(10.f); setLED<LedD5Light>(ledOn);}
				if (++counters[5]>divisions[5]) {counters[5]=0; setOutput<D6Out>(10.f); setLED<LedD6Light>(ledOn);}
				if (++counters[6]>divisions[6]) {counters[6]=0; setOutput<D7Out>(10.f); setLED<LedD7Light>(ledOn);}
				if (++counters[7]>divisions[7]) {counters[7]=0; setOutput<D8Out>(10.f); setLED<LedD8Light>(ledOn);}
			}
		}

		if (autoresetMode == AUTORESET_16){
			//(t+1)*2: 16, 32, 64, 128
			if (++autoreset>=((divideByMax+1)*2)){
				autoreset=0;
				counters.fill(0);
			}
		}
	}

	void processFallingClockEdge(void) {
		setLED<LedInLight>(ledOff);

		if (outputMode == Output_t::GATE){
			if (countingMode == Counting_t::DOWN){
				if (counters[0]==((divisions[0]+1)>>1)) {setOutput<D1Out>(0.f); setLED<LedD1Light>(ledOff);}
				if (counters[1]==((divisions[1]+1)>>1)) {setOutput<D2Out>(0.f); setLED<LedD2Light>(ledOff);}
				if (counters[2]==((divisions[2]+1)>>1)) {setOutput<D3Out>(0.f); setLED<LedD3Light>(ledOff);}
				if (counters[3]==((divisions[3]+1)>>1)) {setOutput<D4Out>(0.f); setLED<LedD4Light>(ledOff);}
				if (counters[4]==((divisions[4]+1)>>1)) {setOutput<D5Out>(0.f); setLED<LedD5Light>(ledOff);}
				if (counters[5]==((divisions[5]+1)>>1)) {setOutput<D6Out>(0.f); setLED<LedD6Light>(ledOff);}
				if (counters[6]==((divisions[6]+1)>>1)) {setOutput<D7Out>(0.f); setLED<LedD7Light>(ledOff);}
				if (counters[7]==((divisions[7]+1)>>1)) {setOutput<D8Out>(0.f); setLED<LedD8Light>(ledOff);}
			} else {
				if (counters[0]==((divisions[0]+1)>>1)) {setOutput<D1Out>(10.f); setLED<LedD1Light>(ledOn);}
				if (counters[1]==((divisions[1]+1)>>1)) {setOutput<D2Out>(10.f); setLED<LedD2Light>(ledOn);}
				if (counters[2]==((divisions[2]+1)>>1)) {setOutput<D3Out>(10.f); setLED<LedD3Light>(ledOn);}
				if (counters[3]==((divisions[3]+1)>>1)) {setOutput<D4Out>(10.f); setLED<LedD4Light>(ledOn);}
				if (counters[4]==((divisions[4]+1)>>1)) {setOutput<D5Out>(10.f); setLED<LedD5Light>(ledOn);}
				if (counters[5]==((divisions[5]+1)>>1)) {setOutput<D6Out>(10.f); setLED<LedD6Light>(ledOn);}
				if (counters[6]==((divisions[6]+1)>>1)) {setOutput<D7Out>(10.f); setLED<LedD7Light>(ledOn);}
				if (counters[7]==((divisions[7]+1)>>1)) {setOutput<D8Out>(10.f); setLED<LedD8Light>(ledOn);}
			}

			if (++counters[0]>divisions[0]) counters[0]=0;
			if (++counters[1]>divisions[1]) counters[1]=0;
			if (++counters[2]>divisions[2]) counters[2]=0;
			if (++counters[3]>divisions[3]) counters[3]=0;
			if (++counters[4]>divisions[4]) counters[4]=0;
			if (++counters[5]>divisions[5]) counters[5]=0;
			if (++counters[6]>divisions[6]) counters[6]=0;
			if (++counters[7]>divisions[7]) counters[7]=0;
			
		} else {
			setOutput<D1Out>(0.f); setLED<LedD1Light>(ledOff);
			setOutput<D2Out>(0.f); setLED<LedD2Light>(ledOff);
			setOutput<D3Out>(0.f); setLED<LedD3Light>(ledOff);
			setOutput<D4Out>(0.f); setLED<LedD4Light>(ledOff);
			setOutput<D5Out>(0.f); setLED<LedD5Light>(ledOff);
			setOutput<D6Out>(0.f); setLED<LedD6Light>(ledOff);
			setOutput<D7Out>(0.f); setLED<LedD7Light>(ledOff);
			setOutput<D8Out>(0.f); setLED<LedD8Light>(ledOff);
		}
	}

private:
	static constexpr std::array<uint32_t, 8> rotateTable = {0,1,2,3,5,7,11,15};

	uint32_t divideBy(uint32_t rotationAmount, uint32_t divideByMax, uint32_t jack, Spread_t spreadMode) {
		if (spreadMode == Spread_t::SPREAD_ON) {
			if (divideByMax==15) { // /2 /4 /6 /8 /10 /12 /14 /16 rotating within 1-16
				return ((((jack<<1)+1)+rotationAmount) & 15);
			} else if (divideByMax==31) {// /4 /8 /12... /32 rotating within 1-32
				return ((((jack<<2)+3)+rotationAmount) & 31);
			} else if (divideByMax==63) {// /8 /16 /20.../64 rotating within 1-64
				return ((((jack<<3)+7)+rotationAmount) & 63);
			} else {
				return rotateTable[((jack+rotationAmount) & 7)];
			}
		} else { //no spread, so jacks are 1...8 (or 1..16, etc depending on maxdivby)
			return ((jack+rotationAmount) & divideByMax);
		}
	}

};

} // namespace MetaModule
