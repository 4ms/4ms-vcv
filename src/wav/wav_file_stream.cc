#include "wav/wav_file_stream.hh"
#include "util/lockfree_fifo_spsc_dyn.hh"
#include "wav/dr_wav.h"
#include <array>
#include <cstdio>
#include <span>
#include <string_view>

namespace MetaModule
{
namespace
{
void print_message(auto... args) {
	constexpr bool WAV_FILE_STREAM_PRINT_ERRORS = true;
	if constexpr (WAV_FILE_STREAM_PRINT_ERRORS)
		printf("%s", args...);
}
} // namespace

struct WavFileStream::Internal {
	size_t max_samples;

	drwav wav;

	enum class State { NotLoaded, Loaded, LoadedEof, FileError };
	std::atomic<State> state;

	std::atomic<uint32_t> frames_in_buffer = 0;
	std::atomic<uint32_t> next_frame_to_write = 0;
	std::atomic<uint32_t> next_sample_to_read = 0;

	LockFreeFifoSpscDyn<int16_t> pre_buff;

	// 4kB is an efficient size to read from an SD Card or USB Drive
	// but the gaps between reads due to async thread timing is too large.
	// 8kB has a better ratio (16kB is better but makes GUI too laggy)
	static constexpr unsigned ReadBlockBytes = 4096;

	// read_buff needs to be big enough to hold 8kB of any data converted to int16_t
	// Worst case: 8kB of 8-bit mono data will convert to 8192 ints
	std::array<int16_t, ReadBlockBytes> read_buff;

	Internal(size_t max_samples)
		: max_samples{max_samples}
		, pre_buff{max_samples} {
	}

	bool resize(size_t max_samples) {
		this->max_samples = max_samples;

		// TODO: is it really OK to resize if state is FileError?
		if (state.load(std::memory_order_seq_cst) == State::NotLoaded)
			return false;

		auto buffsize = std::min<uint32_t>(max_samples, wav.totalPCMFrameCount * wav.channels);

		if (pre_buff.max_size() != buffsize) {
			pre_buff.resize(buffsize);
			reset_prebuff();

			return true;
		} else {
			return false;
		}
	}

	size_t max_size() const {
		return max_samples;
	}

	size_t buffer_samples() const {
		return pre_buff.max_size();
	}

	size_t buffer_frames() const {
		auto channels = wav.channels >= 2 ? 2 : 1;
		return buffer_samples() / channels;
	}

	bool load(std::string_view sample_path) {
		unload();

		if (drwav_init_file(&wav, sample_path.data(), nullptr)) {
			state.store(State::Loaded, std::memory_order_seq_cst);
			resize(max_samples);
			return true;
		} else {
			state.store(State::NotLoaded, std::memory_order_seq_cst);
			return false;
		}
	}

	// Called by destructor (GUI thread) and async process
	void unload() {
		reset_prebuff();

		if (state.load(std::memory_order_seq_cst) != State::NotLoaded) {
			drwav_uninit(&wav);
		}
	}

	bool is_loaded() const {
		auto s = state.load(std::memory_order_seq_cst);
		return s == State::Loaded || s == State::LoadedEof;
	}

	void read_frames_from_file() {
		unsigned frames_to_read = ReadBlockBytes / wav.fmt.blockAlign;
		read_frames_from_file(frames_to_read);
	}

	void read_frames_from_file(int num_frames) {
		if (state.load(std::memory_order_seq_cst) != State::Loaded)
			return;

		while (num_frames > 0) {
			// Read blocks of maximum 4kB at a time
			unsigned frames_to_read = std::min(ReadBlockBytes / wav.fmt.blockAlign, (unsigned)num_frames);

			if (auto num_free = pre_buff.num_free(); num_free == 0)
				return;
			else if (num_free < frames_to_read) {
				frames_to_read = pre_buff.num_free() / wav.channels;
				num_frames = frames_to_read; // abort after this read
			}

			auto frames_read = drwav_read_pcm_frames_s16(&wav, frames_to_read, read_buff.data());

			if (wav.readCursorInPCMFrames == wav.totalPCMFrameCount) {
				state = State::LoadedEof;
			} else if (frames_read < frames_to_read) {
				state = State::FileError;
				break;
			}

			if (frames_read > frames_to_read) {
				print_message("WavFileStream: Internal error: drwav read more frames than requested\n");
				frames_read = frames_to_read;
			}

			auto samples = std::span<const int16_t>(read_buff.data(), frames_read * wav.channels);

			// Copy to pre-buffer, one sample at a time
			unsigned frame_ctr = 0;
			for (auto out : samples) {
				if (!pre_buff.put(out)) {
					print_message("WavFileStream: Buffer overflow\n");
				}
				if (++frame_ctr >= wav.channels) {
					frame_ctr = 0;

					next_frame_to_write.store(next_frame_to_write.load() + 1);

					auto f = frames_in_buffer.load();
					if (f < buffer_frames())
						frames_in_buffer.store(f + 1);
				}
			}

			num_frames -= frames_read;

			if (state == State::LoadedEof) {
				break;
			}
		}
	}

	float pop_sample() {
		auto p = pre_buff.get().value_or(0);
		float f = p / (float)INT16_MIN;
		next_sample_to_read.store(next_sample_to_read.load() + 1);
		return -f;
	}

	bool is_stereo() const {
		return wav.channels > 1;
		// return is_loaded() ? wav.channels > 1 : false;
	}

	float sample_seconds() const {
		return (float)wav.totalPCMFrameCount / (float)wav.sampleRate;
	}

	unsigned samples_available() const {
		return pre_buff.num_filled();
	}

	unsigned frames_available() const {
		if(wav.channels == 0){
			return 0;
		}
		return samples_available() / wav.channels;
	}

	bool is_eof() const {
		return state.load(std::memory_order_seq_cst) == State::LoadedEof;
	}

	bool is_file_error() const {
		return state.load(std::memory_order_seq_cst) == State::FileError;
	}

	unsigned current_playback_frame() const {
		if(wav.channels == 0){
			return 0;
		}
		return next_sample_to_read.load() / wav.channels;
	}

	unsigned latest_buffered_frame() const {
		return next_frame_to_write.load();
	}

	unsigned total_frames() const {
		return is_loaded() ? (unsigned)wav.totalPCMFrameCount : 0;
	}

	// Must only be called by audio thread
	void reset_playback_to_frame(uint32_t frame_num) {
		if (is_frame_in_buffer(frame_num)) {
			pre_buff.set_read_offset((next_frame_to_write.load() - frame_num) * wav.channels);
		} else {
			// requested frame is not in the buffer, so we need to start pre-buffering
			reset_prebuff();
		}
		next_sample_to_read = frame_num * wav.channels;
	}

	// Called by filesystem thread:
	void seek_frame_in_file(uint32_t frame_num) {
		if (is_frame_in_buffer(frame_num)) {
			// do nothing: the requested frame is already in the buffer
		} else {
			drwav_seek_to_pcm_frame(&wav, frame_num);
			next_frame_to_write = frame_num;
			if (state.load() == State::LoadedEof) {
				state.store(State::Loaded);
			}
		}
	}

	std::optional<uint32_t> wav_sample_rate() const {
		if (is_loaded())
			return wav.sampleRate;
		else
			return {};
	}

	bool is_frame_in_buffer(uint32_t frame_num) const {
		if (frames_in_buffer == 0)
			return false;

		auto first = first_frame_in_buffer();
		auto last = next_frame_to_write.load();

		if (last >= first) {
			// Not wrapping: check if it's in [first, last)
			if (frame_num >= first && frame_num < last)
				return true;
		} else {
			// Wrapping: check [0, lastL) or [first, max)
			if (frame_num < last || frame_num >= first)
				return true;
		}

		return false;
	}

	void reset_prebuff() {
		pre_buff.set_write_pos(0);
		pre_buff.set_read_pos(0);
		next_frame_to_write = 0;
		next_sample_to_read = 0;
		frames_in_buffer = 0;
	}

	uint32_t first_frame_in_buffer() const {
		// TODO:  frames_in_buffer == 0 returns next_frame_to_write, try std::nullopt?
		if (next_frame_to_write >= frames_in_buffer)
			return next_frame_to_write - frames_in_buffer;
		else
			return next_frame_to_write + total_frames() - frames_in_buffer;
	}
};

// WavFileStream is a wrapper for WavFileStream::Internal
WavFileStream::WavFileStream(size_t max_samples)
	: internal{std::make_unique<Internal>(max_samples)} {
}

WavFileStream::~WavFileStream() = default;

// clang-format off
bool WavFileStream::resize(size_t max_samples){ return internal->resize(max_samples); }
size_t WavFileStream::max_size() const{ return internal->max_size(); }
size_t WavFileStream::buffer_samples() const{ return internal->buffer_samples(); }
size_t WavFileStream::buffer_frames() const{ return internal->buffer_frames(); }
bool WavFileStream::load(std::string_view sample_path){ return internal->load(sample_path); }
void WavFileStream::unload(){ internal->unload(); }
bool WavFileStream::is_loaded() const{ return internal->is_loaded(); }
void WavFileStream::read_frames_from_file(){ internal->read_frames_from_file(); }
void WavFileStream::read_frames_from_file(int num_frames){ internal->read_frames_from_file(num_frames); }
float WavFileStream::pop_sample(){ return internal->pop_sample(); }
unsigned WavFileStream::samples_available() const{ return internal->samples_available(); }
unsigned WavFileStream::frames_available() const{ return internal->frames_available(); }
float WavFileStream::sample_seconds() const{ return internal->sample_seconds(); }
bool WavFileStream::is_eof() const{ return internal->is_eof(); }
bool WavFileStream::is_file_error() const{ return internal->is_file_error(); }
unsigned WavFileStream::current_playback_frame() const{ return internal->current_playback_frame(); }
unsigned WavFileStream::latest_buffered_frame() const{ return internal->latest_buffered_frame(); }
uint32_t WavFileStream::first_frame_in_buffer() const{ return internal->first_frame_in_buffer(); }
void WavFileStream::reset_playback_to_frame(uint32_t frame_num){ internal->reset_playback_to_frame(frame_num); }
void WavFileStream::seek_frame_in_file(uint32_t frame_num){ internal->seek_frame_in_file(frame_num); }
bool WavFileStream::is_stereo() const{ return internal->is_stereo(); }
unsigned WavFileStream::total_frames() const{ return internal->total_frames(); }
std::optional<uint32_t> WavFileStream::wav_sample_rate() const{ return internal->wav_sample_rate(); }
// clang-format on

} // namespace MetaModule
