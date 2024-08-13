#pragma once

namespace MetaModule
{

class EnvelopeFollower {
public:

	float operator()(float input) {
		if (input > peak) {
			peak += (input - peak) / attackSlew;
		} else {
			peak += (input - peak) / decaySlew;
		}

		return peak;
	}

	void setDecay(float decayInSeconds) {
		decaySlew = decayInSeconds * samplingRate;
	}

	void setAttack(float attackInSeconds) {
		attackSlew = attackInSeconds * samplingRate;
	}

	void setSamplerate(float sr) {
		auto decayInSeconds = decaySlew / samplingRate;
		auto attackInSeconds = attackSlew / samplingRate;

		samplingRate = sr;

		setDecay(decayInSeconds);
		setAttack(attackInSeconds);
	}

private:
	float peak = 0.f;
	float samplingRate = 48000.f;
	float decaySlew = 0.2f * samplingRate;
	float attackSlew = 0.005f * samplingRate;
};

} // namespace MetaModule
