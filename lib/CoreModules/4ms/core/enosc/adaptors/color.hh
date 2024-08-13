#pragma once

#include "../easiglib/dsp.hh"

namespace EnOsc
{

using namespace easiglib; //for literal suffix

struct Color {
	struct Adjustment {
		u1_7 r, g, b;
	};

	explicit constexpr Color(u0_8 r, u0_8 g, u0_8 b)
		: r_(r)
		, g_(g)
		, b_(b) {
	}
	u0_8 red() {
		return r_;
	}
	u0_8 green() {
		return g_;
	}
	u0_8 blue() {
		return b_;
	}
	constexpr Color operator+(Color const that) const {
		return Color(r_.add_sat(that.r_), g_.add_sat(that.g_), b_.add_sat(that.b_));
	}
	constexpr Color blend(Color const that) const {
		return Color(
			r_.div2<1>() + that.r_.div2<1>(), g_.div2<1>() + that.g_.div2<1>(), b_.div2<1>() + that.b_.div2<1>());
	}
	constexpr Color blend(Color const that, u0_8 const phase) const {
		return Color(Signal::crossfade(r_, that.r_, phase),
					 Signal::crossfade(g_, that.g_, phase),
					 Signal::crossfade(b_, that.b_, phase));
	}
	constexpr bool operator!=(Color const that) {
		return this->r_ != that.r_ || this->g_ != that.g_ || this->b_ != that.b_;
	}
	constexpr Color adjust(Adjustment const adj) const {
		return Color((r_ * adj.r).template to_sat<0, 8>(),
					 (g_ * adj.g).template to_sat<0, 8>(),
					 (b_ * adj.b).template to_sat<0, 8>());
	}

private:
	u0_8 r_, g_, b_;
};

struct Colors {
	//greyscale
	static constexpr Color black = Color(0._u0_8, 0._u0_8, 0._u0_8);
	static constexpr Color grey50 = Color(0.5_u0_8, 0.5_u0_8, 0.5_u0_8);
	static constexpr Color grey = Color(0.3_u0_8, 0.3_u0_8, 0.3_u0_8);
	static constexpr Color white = Color(max_val<u0_8>, max_val<u0_8>, max_val<u0_8>);

	//primary/secondary
	static constexpr Color red = Color(max_val<u0_8>, 0._u0_8, 0.1_u0_8);
	static constexpr Color green = Color(0._u0_8, max_val<u0_8>, 0._u0_8);
	// static constexpr Color blue = Color(0._u0_8, 0._u0_8, max_val<u0_8>);
	static constexpr Color blue = Color(0._u0_8, 0._u0_8, 0.8_u0_8);
	static constexpr Color yellow = Color(max_val<u0_8>, max_val<u0_8>, 0._u0_8);
	static constexpr Color magenta = Color(max_val<u0_8>, 0._u0_8, max_val<u0_8>);
	static constexpr Color cyan = Color(0._u0_8, max_val<u0_8>, max_val<u0_8>);

	//fun
	static constexpr Color orange = red.blend(yellow);
	static constexpr Color pink = Color(max_val<u0_8>, 0.32_u0_8, 0.24_u0_8);
	static constexpr Color peach = Color(0.6_u0_8, 0.1_u0_8, 0.03_u0_8);
	// static constexpr Color lemon = Color(max_val<u0_8>, 0.42_u0_8, 0._u0_8);
	static constexpr Color lemon = Color(0.5_u0_8, 0.5_u0_8, 0._u0_8);
	static constexpr Color hotpink = Color(max_val<u0_8>, 0._u0_8, 0.5_u0_8);
	static constexpr Color mint = Color(0.34_u0_8, 0.29_u0_8, 0.66_u0_8);
	static constexpr Color ice = Color(0._u0_8, max_val<u0_8>, 0.27_u0_8);

	//blends
	static constexpr Color dark_grey = grey.blend(black, 0.1_u0_8);
	static constexpr Color dark_red = red.blend(black);
	static constexpr Color dark_green = green.blend(black);
	static constexpr Color dark_blue = blue.blend(black);
	static constexpr Color dark_yellow = yellow.blend(black);

	static constexpr Color dark_dark_yellow = dark_yellow.blend(black);
	static constexpr Color dark_magenta = magenta.blend(black);
	static constexpr Color dark_cyan = cyan.blend(black);
};
} // namespace EnOsc
