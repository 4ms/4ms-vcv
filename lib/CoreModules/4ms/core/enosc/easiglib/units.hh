#include "math.hh"
#include "numtypes.hh"

namespace easiglib
{

struct Freq : private Float {

	static f semitones_to_ratio(f p) {
		return Math::fast_exp2(p / 12_f);
	}

	explicit constexpr Freq(f x, f sample_rate)
		: Float(x / sample_rate){};

	static Freq of_pitch(f p, f sample_rate) {
		return Freq(semitones_to_ratio(p - 69._f) * 440_f, sample_rate);
	}

	constexpr Float const repr() const {
		return *this;
	}

	u0_32 to_increment() const {
		return u0_32(this->repr());
	}
};

} // namespace easiglib
