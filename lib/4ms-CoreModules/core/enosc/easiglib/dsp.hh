#pragma once

#include "filter.hh"
#include "numtypes.hh"
#include "units.hh"

namespace easiglib
{

struct Random {
	static uint32_t state() {
		return state_;
	}
	static uint32_t Word() {
		state_ = state_ * 1664525L + 1013904223L;
		return state();
	}
	static int16_t Int16() {
		return Word() >> 16;
	}
	// float between 0 and 1
	static Float Float01() {
		return f(Word()) / f(4294967296ULL);
	}
	static bool Bool() {
		return Word() & 1;
	}

	static constexpr uint32_t rand_max = UINT32_MAX;

private:
	static inline uint32_t state_;
};

// N number of stages, R decimation rate
// careful: gain must be <= 2^16
template<int N, int R>
class CicDecimator {
	s17_15 hi[N];
	s17_15 hc[N];
	static constexpr int gain = ipow(R, N);

public:
	// reads [R*size] input samples, writes [size] output samples:
	void Process(s1_15 *input, s1_15 *output, int size) {
		while (size--) {
			// N integrators
			for (int i = 0; i < R; i++) {
				hi[0] += s17_15(*input++);
				for (int n = 1; n < N; n++) {
					hi[n] += hi[n - 1];
				}
			}
			// N combs
			s17_15 v = hi[N - 1];
			for (int n = 0; n < N; n++) {
				s17_15 in = v;
				v -= hc[n];
				hc[n] = in;
			}
			*output++ = s1_15::wrap(v / gain);
		}
	}
};

// N number of stages, R interpolation rate
// careful: gain must be <= 2^16
template<int N, int R>
class CicInterpolator {
	int32_t hi[N] = {0};
	int32_t hc[N] = {0};
	static constexpr int gain = ipow(R, N - 1);

public:
	// reads [size] input samples, writes [R*size] output samples:
	void Process(int16_t *input, int16_t *output, int size) {
		while (size--) {
			// N combs
			int32_t v = *input++;
			for (int n = 0; n < N; n++) {
				int32_t in = v;
				v -= hc[n];
				hc[n] = in;
			}
			// N integrators
			for (int i = 0; i < R; i++) {
				hi[0] += i == 0 ? v : 0;
				for (int n = 1; n < N; n++) {
					hi[n] += hi[n - 1];
				}
				*output++ = static_cast<int16_t>(hi[N - 1] / gain);
			}
		}
	}
};

template<int N, int R>
class PdmFilter : CicDecimator<N, R> {

	const int16_t setbits[256] = {
#define S(n) (int16_t)((2 * (n)-8) << 11)
#define B2(n) S(n), S(n + 1), S(n + 1), S(n + 2)
#define B4(n) B2(n), B2(n + 1), B2(n + 1), B2(n + 2)
#define B6(n) B4(n), B4(n + 1), B4(n + 1), B4(n + 2)
		B6(0), B6(1), B6(1), B6(2)};
public:
	// reads [8*R*size] binary input sample, outputs [size] samples
	void Process(uint8_t *input, int16_t *output, int size) {
		int16_t temp[R * size];
		for (int i = 0; i < size * R; i++) {
			temp[i] = setbits[*input];
			input++;
		}
		CicDecimator<N, R>::Process(temp, output, size);
	}
};

// Magic circle algorithm
class MagicSine {
	f sinz_ = 0_f;
	f cosz_ = 1_f;
	f freq_ = 2_f * Math::pi * 0.001_f;

public:
	MagicSine(f freq)
		: freq_(2_f * Math::pi * freq) {
	}

	f Process() {
		sinz_ += freq_ * cosz_;
		cosz_ -= freq_ * sinz_;
		return sinz_;
	}

	void set_frequency(f freq) { // freq = f_n / f_s
		freq_ = 2.0_f * Math::pi * freq;
		// this is an approximation, ok for small frequencies. The actual
		// value is f = 2sin(pi f_n T) (T sampling period, f_n freq)
	}
};

class Hysteresis {
	bool armed = true;
	f low, high;

public:
	Hysteresis(f l, f h)
		: low(l)
		, high(h) {
	}
	bool Process(f x) {
		if (armed && x > high) {
			armed = false;
			return true;
		} else if (x < low) {
			armed = true;
		}
		return false;
	}
};

class Derivator {
	f state;

public:
	Derivator(f s)
		: state(s) {
	}
	f Process(f x) {
		f d = x - state;
		state = x;
		return d;
	}
};

template<int DELAY>
class ChangeDetector {
	Derivator d1{0_f}, d2{0_f};
	Hysteresis hy_pos, hy_neg;
	bool armed_pos = true, armed_neg = true;
	int delay = 0;

public:
	ChangeDetector(f lo, f hi)
		: hy_pos(lo, hi)
		, hy_neg(lo, hi) {
	}
	bool Process(f input) {
		f speed = d1.Process(input);
		f accel = d2.Process(speed);
		bool pos = hy_pos.Process(accel);
		bool neg = hy_neg.Process(-accel);
		if (delay && delay-- == 1) {
			return true;
		}
		if (pos) {
			armed_pos = true;
		}
		if (neg) {
			armed_neg = true;
		}
		if ((neg && armed_pos) || (pos && armed_neg)) {
			armed_pos = armed_neg = false;
			delay = DELAY;
		}
		return false;
	}
};

template<class T>
class Sampler {
	T buffer;
	bool hold_ = false;

public:
	T Process(T x) {
		return hold_ ? buffer : buffer = x;
	}
	void hold() {
		hold_ = true;
	}
	void release() {
		hold_ = false;
	}
};

struct TriangleOscillator {
	TriangleOscillator(u0_32 freq)
		: freq_(freq) {
	}
	TriangleOscillator()
		: freq_(0._u0_32) {
	}
	u0_32 Process() {
		phase_ += freq_;
		u0_32 sample = phase_ * 2;
		if (phase_ > 0.5_u0_32)
			sample = 1._u0_32 - sample;
		return sample;
	}

	void set_frequency(u0_32 freq) {
		freq_ = freq;
	}
	void set_phase(u0_32 phase) {
		phase_ = phase;
	}

private:
	u0_32 phase_ = 0._u0_32;
	u0_32 freq_;
};
} // namespace easiglib
