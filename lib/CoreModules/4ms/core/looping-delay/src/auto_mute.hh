#include <cmath>
#include <cstdint>

template<uint32_t LPFCoefRecip, int32_t MinLevel, uint32_t AttackPeriod, uint32_t DecayPeriod>
struct AutoMute {
	enum State { Unmuted, FadingDown, Muted, FadingUp } state = Unmuted;
	float fade_amt = 1.f;
	float lpf = MinLevel;

	float AttackRate = 1.f / AttackPeriod;
	float DecayRate = 1.f / DecayPeriod;

	static constexpr float LPFCoef = 1.f / LPFCoefRecip;

	AutoMute() = default;

	void setAttackPeriod(uint32_t val)
	{
		AttackRate = 1.f / val;
	}

	void setDecayPeriod(uint32_t val)
	{
		DecayRate = 1.f / val;
	}

	int32_t update(int32_t cur) {
		lpf += LPFCoef * (std::abs(cur) - lpf);

		if (lpf < MinLevel && state != Muted)
			state = FadingDown;

		if (lpf >= MinLevel && state != Unmuted)
			state = FadingUp;

		switch (state) {
			case Unmuted:
				return cur;
				break;

			case FadingDown:
				fade_amt -= DecayRate;
				if (fade_amt <= 0.f) {
					fade_amt = 0.f;
					state = Muted;
				}
				break;

			case Muted:
				return 0.f;
				break;

			case FadingUp:
				fade_amt += AttackRate;
				if (fade_amt >= 1.f) {
					fade_amt = 1.f;
					state = Unmuted;
				}
				break;
		}

		return cur * fade_amt;
	}
};
