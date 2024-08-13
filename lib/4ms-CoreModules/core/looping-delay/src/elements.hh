#pragma once

#include <cstdint>

namespace Brain
{
constexpr inline uint32_t MemorySizeBytes = 0x00800000;
}

namespace LDKit
{
enum { FirmwareMajorVersion = 1 };
enum { FirmwareMinorVersion = 0 };

enum PotAdcElement : uint32_t {
	TimePot,
	FeedbackPot,
	DelayFeedPot,
	MixPot,
};

enum CVAdcElement : uint32_t {
	TimeCV,
	FeedbackCV,
	DelayFeedCV,
	MixCV,
};

constexpr static uint32_t NumPots = 4;
constexpr static uint32_t NumCVs = 4;
constexpr static uint32_t NumAdcs = NumPots + NumCVs;

enum TrigInJackElement : uint32_t {
	PingJack,
};

using RAMSampleT = int16_t;
constexpr inline uint32_t MemorySampleSize = sizeof(RAMSampleT);
constexpr inline uint32_t MemorySamplesNum = Brain::MemorySizeBytes / MemorySampleSize;

} // namespace LDKit
