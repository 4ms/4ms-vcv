#pragma once
#include "audio_stream_conf.hh"
#include <cstdint>

#include "../mocks/mocks.hh"

namespace LDKit
{

struct Util {

	// in_between()
	// Utility function to determine if address mid is in between addresses beg and end in a circular (ring) buffer.
	constexpr static bool in_between(uint32_t mid, uint32_t beg, uint32_t end, uint8_t reverse) {
		if (beg == end)
			return mid == beg;

		if (beg < end) {
			return ((mid >= beg) && (mid <= end)) != reverse;
		}
		return ((mid >= end) && (mid <= beg)) == reverse;
	}

	//  !rev &&  (end > start): |     [->........]     |   NOT WRAPPING: rev != (end>start)
	//   rev &&  (end > start): |...<-]          [.....|       WRAPPING: rev == (end>start)
	//  !rev && !(end > start): |.....]          [->...|       WRAPPING: rev == (end>start)
	//   rev && !(end > start): |     [........<-]     |   NOT WRAPPING: rev != (end>start)
	// Key:
	//  [-> or <-] === start
	//    [ or ]   === end
	//     .....   === loop
	//     |   |   === memory boundaries

	// Adds or subtracts offset to/from base_addr, wrapping at [0, Brain::MemorySizeBytes)
	constexpr static uint32_t offset_samples(uint32_t base_addr, int32_t offset, bool subtract = false) {
		constexpr uint32_t Size = Brain::MemorySizeBytes / MemorySampleSize;

		if (subtract)
			offset = -offset;

		// Check for underflow, i.e. if base_addr = 0 and offset is negative
		if (offset < 0) {
			while (base_addr < (uint32_t)(-offset))
				base_addr += Size;
		}

		// TODO: Check this doesn't wrap!
		base_addr += offset;

		while (base_addr >= Size)
			base_addr -= Size;

		return base_addr;
	}
};

static_assert(!Util::in_between(5, 1, 4, 0), "not wrapped, not reverse, mid not between");
static_assert(Util::in_between(3, 1, 4, 0), "not wrapped, not reverse, mid between");
static_assert(Util::in_between(5, 1, 4, 1), "not wrapped, reverse, mid not between");
static_assert(!Util::in_between(3, 1, 4, 1), "not wrapped, reverse, mid between");

static_assert(Util::in_between(5, 4, 1, 0), "wrapped, not reverse, mid not between");
static_assert(!Util::in_between(3, 4, 1, 0), "wrapped, not reverse, mid between");
static_assert(!Util::in_between(5, 4, 1, 1), "wrapped, reverse, mid not between");
static_assert(Util::in_between(3, 4, 1, 1), "wrapped, reverse, mid between");

static_assert(Util::in_between(3, 3, 3, 1), "zero length, reverse, mid equal");
static_assert(Util::in_between(3, 3, 3, 0), "zero length, not reverse, mid equal");
static_assert(!Util::in_between(2, 3, 3, 1), "zero length, reverse, mid not equal");
static_assert(!Util::in_between(2, 3, 3, 0), "zero length, not reverse, mid not equal");

constexpr static uint32_t Size = Brain::MemorySizeBytes / MemorySampleSize;
static_assert(Util::offset_samples(12, 8) == 12 + 8, "add");
static_assert(Util::offset_samples(0, -8) == Size - 8, "subtract across lower loop boundary");
static_assert(Util::offset_samples(0, 8, true) == Size - 8, "subtract across loop boundary");
static_assert(Util::offset_samples(2, -8) == Size - 6, "subtract across lower loop boundary");
static_assert(Util::offset_samples(Size - 4, 8) == 4, "add across loop boundary");
static_assert(Util::offset_samples(Size + 4, -8) == (Size - 4), "subtract across upper loop boundary");
static_assert(Util::offset_samples(320, Size) == 320, "integer overflow");
static_assert(Util::offset_samples(0xAB00, Size * 2) == 0xAB00, "integer overflow");
static_assert(Util::offset_samples(0xAB00, Size, true) == 0xAB00, "integer underflow");
static_assert(Util::offset_samples(0xAB00, Size * 2, true) == 0xAB00, "integer underflow");
} // namespace LDKit
