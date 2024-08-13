#pragma once

namespace MetaModule
{
class FlipFlop {
public:
	FlipFlop(float min_, float max_)
		: currentState(false)
		, min(min_)
		, max(max_) {
	}

	bool operator()(float val) {
		if (val >= max) {
			currentState = true;
		} else if (val <= min) {
			currentState = false;
		} else {
			// no change
		}
		return currentState;
	}

private:
	bool currentState;
	const float min;
	const float max;
};
} // namespace MetaModule
