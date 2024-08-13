#pragma once

#include "adaptors/persistent_storage.hh"
#include "easiglib/buffer.hh"
#include "parameters.hh"

namespace EnOsc {

constexpr const int kScaleNr = 10;
constexpr const int kBankNr = 3;
constexpr const int kMaxScaleSize = 2 * kMaxNumOsc;
constexpr const f kScaleUnicityThreshold = 0.1_f;

struct PitchPair {
	f p1, p2, crossfade;
};

class Scale {
	// scale_[0] = 0, contains [size_] sorted elements
	f scale_[kMaxScaleSize];
	int size_ = 0;
	friend class PreScale;

public:
	Scale() {
	}
	Scale(std::initializer_list<f> scale) {
		size_ = scale.size();
		std::copy(scale.begin(), scale.end(), scale_);
	}

	// #pragma GCC push_options
	// #pragma GCC optimize ("O0")

	bool validate() const {
		if (!(size_ > 0 && size_ <= kMaxScaleSize))
			return false;
		for (auto &note : scale_)
			if (!(note >= 0_f && note <= 150_f))
				return false;
		return true;
	}

	// #pragma GCC pop_options

	// pitch>0
	PitchPair Process(f const pitch) const {
		f max = scale_[size_ - 1];
		// quotient by the max
		f oct = (pitch / max).integral();
		f octaves = oct * max;
		f semitones = pitch - octaves;
		// semitones [0..max[
		int index = binary_search(semitones, scale_, size_);
		// index: [0..size_-2]
		f p1 = scale_[index];
		f p2 = scale_[index + 1];
		f crossfade = (semitones - p1) / (p2 - p1);
		p1 += octaves;
		p2 += octaves;

		if ((index + (oct.floor() * (size_ + 1))) & 1) {
			crossfade = 1_f - crossfade;
			f tmp = p1;
			p1 = p2;
			p2 = tmp;
		}

		return {p1, p2, crossfade};
	}

	void copy_from(const Scale &g) {
		*this = g;
	}
};

class PreScale : Scale {
public:
	bool add(f x) {
		if (size_ < kMaxScaleSize - 2) { // -2: if Octave wrap, will add one
			scale_[size_++] = x;
			return true;
		} else {
			return false;
		}
	}

	int size() const {
		return size_;
	}
	void clear() {
		size_ = 0;
	}
	f get(int i) const {
		return scale_[i];
	}
	void set_last(f const x) {
		scale_[size_ - 1] = x;
	}
	bool remove_last() {
		if (size_ > 1) {
			size_--;
			return true;
		} else
			return false;
	}

	void quantize() {
		for (int i = 0; i < size_; i++) {
			scale_[i] = scale_[i].integral();
		}
	}

	// do not call if size==0
	bool copy_to(Scale *g, bool wrap_octave) {
		// sort table
		std::sort(scale_, scale_ + size_);
		// normalize from smallest element
		f base = scale_[0];
		for (int i = 0; i < kMaxScaleSize; i++) {
			if (i < size_)
				scale_[i] -= base;
			else
				scale_[i] = 0_f;
		}

		if (wrap_octave) {
			f max = scale_[size_ - 1];
			scale_[size_++] = ((max / 12_f).integral() + 1_f) * 12_f;
		}

		// remove duplicate elements
		uniquify(scale_, size_, kScaleUnicityThreshold);

		if (size_ > 1) {
			// copy to real scale
			*g = *this;
			return true;
		} else {
			return false;
		}
	}
};

class Quantizer {
	struct ScaleTable : std::array<std::array<Scale, kScaleNr>, kBankNr> {
		bool validate() {
			for (const auto &bank : *this)
				for (const auto &scale : bank)
					if (!scale.validate())
						return false;
			return true;
		}
	};

	ScaleTable const default_scales_ = {
		{{{{
			  // 12TET
			  {0_f, 12_f},				   // Slot 1: octave
			  {0_f, 7_f, 12_f},			   // Slot 2: octave+fifth
			  {0_f, 4_f, 7_f, 12_f},	   // Slot 3: major triad
			  {0_f, 3_f, 7_f, 12_f},	   // Slot 4: minor triad
			  {0_f, 3_f, 5_f, 7_f, 12_f},  // Slot 5: minor triad
			  {0_f, 3_f, 5_f, 10_f, 12_f}, // Slot 6: ??
			  {0_f, 1_f, 5_f, 8_f, 12_f},  //Slot 7:
			  {0_f, 7_f},				   // Slot 8: circle of fifths
			  {0_f, 3_f, 5_f, 7_f},		   // Slot 9:
			  {0_f, 1_f},				   // Slot 10: all semitones
		  }},
		  {{
			  // OCTAVE

			  // Harmonics

			  // harmonics 2-3 (perfect fifth)
			  {
				  0_f,
				  7.0195500086_f,
				  12_f,
			  },

			  // harmonics 4-7
			  {0_f, 3.86313714_f, 7.01955001_f, 9.68825906_f, 12_f},

			  // harmonics 8-15
			  {0_f,
			   2.03910002_f,
			   3.86313714_f,
			   5.51317942_f,
			   7.01955001_f,
			   8.40527662_f,
			   9.68825906_f,
			   10.88268715_f,
			   12_f},

			  // odd harmonics 1-16 (8)
			  {0_f,
			   7.0195500086_f,
			   15.86313713_f,
			   21.68825906_f,
			   26.03910001_f,
			   29.51317942_f,
			   32.40527661_f,
			   34.88268714_f,
			   36_f},

			  // Pythagorean scales

			  // Do Fa So. np.log2([1., 4./3, 3./2, 2.]) * 12.
			  {0_f, 4.98044999_f, 7.01955001_f, 12_f},

			  // Do Re Fa So La. np.log2([1., 9./8, 4./3, 3./2, 27./16, 2.]) * 12.
			  {0_f, 2.03910002_f, 4.98044999_f, 7.01955001_f, 9.05865003_f, 12_f},

			  // // np.log2([1., 9./8, 81./64, 4./3, 3./2, 27./16, 243./128, 2.]) * 12.
			  // { 0_f, 2.03910002_f, 4.07820003_f, 4.98044999_f,
			  //   7.01955001_f, 9.05865003_f, 11.09775004_f,  12_f },

			  // Just scales

			  // np.log2(sorted([1., 3./2, 4./3, 5./3, 2.])) * 12.
			  {0_f, 4.98044999_f, 7.01955001_f, 8.84358713_f, 12_f},

			  // np.log2(sorted([1., 3./2, 4./3, 5./3, 5./4, 7./4, 2.])) * 12.
			  {0_f, 3.86313714_f, 4.98044999_f, 7.01955001_f, 8.84358713_f, 9.68825906_f, 12_f},

			  // N-TET: np.log2(np.exp2(np.arange(1.+N)/N)) * 12.

			  // 5-TET
			  {0_f, 2.4_f, 4.8_f, 7.2_f, 9.6_f, 12_f},

			  // 7-TET
			  {0_f, 1.71428571_f, 3.42857143_f, 5.14285714_f, 6.85714286_f, 8.57142857_f, 10.28571429_f, 12_f},

		  }},
		  {{
			  // FREE
			  {0_f, 7.0195500086_f},	 // perfect fifths spiral
			  {0_f, 2_f},				 // whole tone = MLT#1
			  {0_f, 1_f, 3_f},			 // MLT #2
			  {0_f, 2_f, 3_f, 4_f},		 // MLT #3
			  {0_f, 1_f, 2_f, 5_f, 6_f}, // MLT #4

			  // Bohlen-Pierce
			  {0_f,
			   1.46304231_f,
			   2.92608462_f,
			   4.38912693_f,
			   5.85216923_f,
			   7.31521154_f,
			   8.77825385_f,
			   10.24129616_f,
			   11.70433847_f,
			   13.16738078_f,
			   14.63042308_f,
			   16.09346539_f,
			   17.5565077_f,
			   19.01955001_f},

			  // Bohlen-Pierce major triad
			  {0_f, 8.77825385_f, 14.63042308_f, 19.01955001_f},

			  // Bohlen-Pierce minor triad
			  {0_f, 5.85216923_f, 14.63042308_f, 19.01955001_f},

			  // Carlos alpha
			  {0_f, 0.78_f},

			  // Carlos beta
			  {0_f, 0.638_f},
		  }}}}};

	ScaleTable scales_;
	Persistent<WearLevel<FlashBlock<2, ScaleTable>>> scales_storage_{&scales_, default_scales_};

public:
	void reset_scale(Parameters::Scale scale) {
		int i = scale.mode;
		int j = scale.value;
		scales_[i][j].copy_from(default_scales_[i][j]);
	}

	Scale *get_scale(Parameters::Scale scale) {
		return &scales_[scale.mode][scale.value];
	}

	void Save() {
		scales_storage_.Save();
	}
};

}
