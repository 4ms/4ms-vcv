#pragma once
#include "util/audio_frame.hh"

struct AudioStreamConf {

	// BlockSize: Number of Frames processed each time AudioStream::process() is called
	static constexpr int BlockSize = 1;

	using SampleT = int32_t;

	static constexpr int SampleBits = 24;
	static constexpr int NumInChans = 2;
	static constexpr int NumOutChans = 2;
	static constexpr int HalvesInAWhole = 2;

	using AudioInFrame = AudioFrame<SampleT, SampleBits, NumInChans>;
	using AudioInBlock = std::array<AudioInFrame, BlockSize>;
	using AudioInBuffer = std::array<AudioInBlock, HalvesInAWhole>;

	using AudioOutFrame = AudioFrame<SampleT, SampleBits, NumOutChans>;
	using AudioOutBlock = std::array<AudioOutFrame, BlockSize>;
	using AudioOutBuffer = std::array<AudioOutBlock, HalvesInAWhole>;

	static constexpr inline auto AudioBufferSizeBytes = sizeof(AudioInBuffer);
	static constexpr inline auto AudioBlockSizeBytes = sizeof(AudioInBlock);
};
