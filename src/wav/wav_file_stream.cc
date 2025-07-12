// TODO: this is identical to file in MetaModule firmware repo, can we share it in CoreModules?
#include "wav/wav_file_stream.hh"
#include "util/lockfree_fifo_spsc_dyn.hh"
#include "wav/dr_wav.h"
#include <cstdio>
#include <span>
#include <string_view>

namespace MetaModule
{

static constexpr bool PRINT_ERRORS = true;
void print_message(auto... args) {
	if constexpr (PRINT_ERRORS)
		printf(args...);
}

WavFileStream::WavFileStream(size_t max_samples)
	: MaxSamples{MathTools::next_power_of_2(max_samples)}
	, pre_buff{MaxSamples} {
	if (MaxSamples != max_samples)
		print_message("WavFileStream must be constructed with a power of 2. %zu will be used\n", MaxSamples);
}

void WavFileStream::resize(size_t max_samples) {
	MaxSamples = MathTools::next_power_of_2(max_samples);
	pre_buff.resize(MaxSamples);
	if (MaxSamples != max_samples)
		print_message("WavFileStream must be resized with a power of 2. %zu will be used\n", MaxSamples);
}

bool WavFileStream::load(std::string_view sample_path) {
	unload();

	eof = false;

	loaded = drwav_init_file(&wav, sample_path.data(), nullptr);
	frames_in_buffer = 0;
	return loaded;
}

void WavFileStream::unload() {
	reset_prebuff();

	if (loaded) {
		drwav_uninit(&wav);
		loaded = false;
	}
}

bool WavFileStream::is_loaded() const {
	return loaded;
}

void WavFileStream::read_frames_from_file() {
	unsigned frames_to_read = ReadBlockBytes / wav.fmt.blockAlign;
	read_frames_from_file(frames_to_read);
}

void WavFileStream::read_frames_from_file(int num_frames) {
	if (!loaded || eof)
		return;

	while (num_frames > 0) {
		// Read blocks of maximum 4kB at a time
		unsigned frames_to_read = std::min(ReadBlockBytes / wav.fmt.blockAlign, (unsigned)num_frames);

		auto frames_read = drwav_read_pcm_frames_f32(&wav, frames_to_read, read_buff.data());

		// This is not correct, if we happen to read up to the end of the file
		// (but don't attempt to go past, then we'll be at the EOF but eof will be false
		eof = (frames_read != frames_to_read) || (wav.readCursorInPCMFrames == wav.totalPCMFrameCount);

		if (frames_read > frames_to_read) {
			printf("WavFileStream: Internal error: drwav read more frames than requested\n");
			frames_read = frames_to_read;
		}

		auto samples = std::span<const float>(read_buff.data(), frames_read * wav.channels);

		// Copy to pre-buffer, one sample at a time
		unsigned frame_ctr = 0;
		for (auto out : samples) {
			if (!pre_buff.put(out)) {
				printf("WavFileStream: Buffer overflow\n");
				// TODO: Handle buffer overflow: we read too much from disk and the audio thread
				// is not consuming the samples fast enough to make room.
				// Set drwav read cursor back to this position, pop back if we're not a frame boundary,
				// set next_frame_to_write, and abort
			}
			if (++frame_ctr >= wav.channels) {
				frame_ctr = 0;

				next_frame_to_write.store(next_frame_to_write.load() + 1);

				auto f = frames_in_buffer.load();
				if (f < MaxSamples / wav.channels)
					frames_in_buffer.store(f + 1);
			}
		}

		num_frames -= frames_read;

		if (eof) {
			break;
		}
	}
}

float WavFileStream::pop_sample() {
	auto p = pre_buff.get().value_or(0);
	next_sample_to_read.store(next_sample_to_read.load() + 1);
	return p;
}

bool WavFileStream::is_stereo() const {
	return loaded ? wav.channels > 1 : false;
}

unsigned WavFileStream::samples_available() const {
	return pre_buff.num_filled();
}

unsigned WavFileStream::frames_available() const {
	return samples_available() / wav.channels;
}

bool WavFileStream::is_eof() const {
	return eof;
}

unsigned WavFileStream::current_playback_frame() const {
	return next_sample_to_read.load() / wav.channels;
}

unsigned WavFileStream::total_frames() const {
	return loaded ? (unsigned)wav.totalPCMFrameCount : 0;
}

// Must only be called by audio thread
void WavFileStream::reset_playback_to_frame(uint32_t frame_num) {
	if (is_frame_in_buffer(frame_num)) {
		pre_buff.set_read_offset((next_frame_to_write.load() - frame_num) * wav.channels);
	} else {
		// requested frame is not in the buffer, so we need to start pre-buffering
		pre_buff.reset();
	}
	next_sample_to_read = frame_num * wav.channels;
}

// Called by filesystem thread:
void WavFileStream::seek_frame_in_file(uint32_t frame_num) {
	if (is_frame_in_buffer(frame_num)) {
		// do nothing: the requested frame is already in the buffer
	} else {
		drwav_seek_to_pcm_frame(&wav, frame_num);
		next_frame_to_write = frame_num;

		eof = false;
	}
}

std::optional<uint32_t> WavFileStream::wav_sample_rate() const {
	if (loaded)
		return wav.sampleRate;
	else
		return {};
}

bool WavFileStream::is_frame_in_buffer(uint32_t frame_num) const {
	auto first = first_frame_in_buffer();
	auto last = next_frame_to_write.load();

	if (last >= first) {
		// Not wrapping: check if it's in [F, L)
		if (frame_num >= first && frame_num < last)
			return true;
	} else {
		// Wrapping: check [0, L) and [F, max)
		if (frame_num < last || frame_num >= first)
			return true;
	}

	return false;
}

void WavFileStream::reset_prebuff() {
	pre_buff.set_write_pos(0);
	pre_buff.set_read_pos(0);
	next_frame_to_write = 0;
	next_sample_to_read = 0;
	frames_in_buffer = 0;
}

uint32_t WavFileStream::first_frame_in_buffer() const {
	// TODO:  frames_in_buffer == 0 returns next_frame_to_write, try std::nullopt?
	if (next_frame_to_write >= frames_in_buffer)
		return next_frame_to_write - frames_in_buffer;
	else
		return next_frame_to_write + total_frames() - frames_in_buffer;
}

} // namespace MetaModule
