#include "audio_stream.hh"
#include "conf/board_conf.hh"
#include "controls.hh"
#include "debug.hh"
#include "drivers/timekeeper.hh"
#include "hardware_tests.hh"
#include "looping_delay.hh"
#include "system.hh"
#include "timer.hh"

// #include "test_audio.hh"

namespace
{
// Initialize the system before main()
LDKit::System _init;
} // namespace

void main() {
	using namespace LDKit;
	using AudioInBlock = AudioStreamConf::AudioInBlock;
	using AudioOutBlock = AudioStreamConf::AudioOutBlock;

	Controls controls;
	if (Board::PingButton::PinT::read() && Board::RevButton::PinT::read()) {
		HAL_Delay(50);
		if (Board::PingButton::PinT::read() && Board::RevButton::PinT::read())
			HWTests::run(controls);
	}

	Flags flags;
	Params params{controls, flags};
	LoopingDelay looping_delay{params, flags};
	AudioStream audio([&looping_delay, &params](const AudioInBlock &in, AudioOutBlock &out) {
		params.update();
		looping_delay.update(in, out);
	});

	params.start();
	controls.start();
	audio.start();

	while (true) {
		__NOP();
	}
}

void recover_from_task_fault() {
	LDKit::SystemTarget::restart();
}
