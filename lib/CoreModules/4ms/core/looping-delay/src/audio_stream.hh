#pragma once

#include "audio_stream_conf.hh"
#include "conf/codec_conf.hh"
#include "conf/rcc_conf.hh"
#include "debug.hh"
#include "drivers/callable.hh"
#include "drivers/codec_PCM3060.hh"
#include "drivers/i2c.hh"
#include <functional>

namespace LDKit
{

struct AudioStream {
	// TODO: Verify this region is non-cacheable on F7
	alignas(256) static inline
		__attribute__((section(".noncachable"))) AudioStreamConf::AudioInBuffer audio_in_dma_buffer;
	alignas(256) static inline
		__attribute__((section(".noncachable"))) AudioStreamConf::AudioOutBuffer audio_out_dma_buffer;

public:
	using AudioProcessFunction =
		std::function<void(const AudioStreamConf::AudioInBlock &, AudioStreamConf::AudioOutBlock &)>;

	AudioStream(AudioProcessFunction &&process_func)
		: codec_i2c{Board::codec_i2c_conf}
		, codec{codec_i2c, Board::sai_conf}
		, _process_func{std::move(process_func)} {

		codec.init(mdrivlib::CodecPCM3060Register::default_setup_i2s_24bit_hpf);
		codec.set_rx_buffer_start(audio_in_dma_buffer);
		codec.set_tx_buffer_start(audio_out_dma_buffer);
		codec.set_callbacks([this] { _process<1>(); }, [this] { _process<0>(); });
	}

	void start() { codec.start(); }
	void stop() { codec.stop(); }

	void set_callback(AudioProcessFunction &&process_func) {
		stop();
		_process_func = std::move(process_func);
		start();
	}

private:
	mdrivlib::I2CPeriph codec_i2c;
	mdrivlib::CodecPCM3060 codec;

	AudioProcessFunction _process_func;

	template<uint32_t buffer_half>
	void _process() {
		// Debug::Pin0::high();
		_process_func(audio_in_dma_buffer[1 - buffer_half], audio_out_dma_buffer[buffer_half]);
		// Debug::Pin0::low();
	}
};

} // namespace LDKit
