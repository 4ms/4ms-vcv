#pragma once

namespace MetaModule
{
class DCBlock {
public:
	DCBlock(float factor_)
		: factor(factor_)
		, previousInput(0.f)
		, previousOutput(0.f) {
	}

	float operator()(float input) {
		auto output = input - previousInput + factor * previousOutput;
		previousInput = input;
        previousOutput = output; 
        return output;
	}

private:
	const float factor;
	float previousInput;
	float previousOutput;
};
} // namespace MetaModule