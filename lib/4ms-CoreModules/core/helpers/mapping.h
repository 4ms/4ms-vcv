#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace Mapping
{

namespace Impl
{

template<typename T, std::size_t LEN, typename rangeClass, typename F>
struct EmptyArray {
	constexpr EmptyArray(const F &func)
		: data() {
		constexpr float min = rangeClass::min;
		constexpr float max = rangeClass::max;

		for (std::size_t i = 0; i < LEN; i++) {
			auto x = min + i * (max - min) / float(LEN - 1);
			data[i] = func(x);
		}
	}
	std::array<T, LEN> data;
};

} // namespace Impl

template<std::size_t LEN>
class LookupTable_t {
public:
	using Base_t = std::array<float, LEN>;

public:
	constexpr LookupTable_t(const float min_, const float max_, const Base_t &input)
		: min(min_)
		, max(max_) {
		static_assert(LEN >= 2);
		std::copy_n(input.begin(), LEN, points.begin());
	}

	constexpr float lookup(float val) const {
		float idx = ((val - min) / (max - min)) * (LEN - 1);

		if (idx <= 0.f)
			return points.front();
		else if (idx >= (LEN - 1))
			return points.back();
		else {
			auto lower_idx = (uint32_t)idx;
			float phase = idx - lower_idx;
			auto lower = points[lower_idx];
			auto upper = points[lower_idx + 1];
			return lower + phase * (upper - lower);
		}
	}

private:
	Base_t points;
	const float min;
	const float max;

public:
	template<typename rangeClass, typename F>
	static constexpr LookupTable_t generate(const F func) {
		constexpr Impl::EmptyArray<float, LEN, rangeClass, F> dataArray(func);
		return LookupTable_t(rangeClass::min, rangeClass::max, dataArray.data);
	}
};

} // namespace Mapping
