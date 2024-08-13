#pragma once
#include "conf/board_conf.hh"

namespace LDKit
{
class PCBDetect {

public:
	static bool is_kit() {
		static const bool _is_kit = _check_pcb_is_kit();
		return _is_kit;
	}

	// Hardware difference between Kit and Built PCBs:
	// On the built PCB only, the two loop out pins are shorted together.
	// To avoid multiple outputs connected, we must set one pin to output and one pin to input
	static bool _check_pcb_is_kit() {
		bool _is_kit = false;

		// Init pins as Kit
		Board::LoopClkKit loop_out_kit;
		Board::LoopClkBuiltRead loop_in_built;

		// Check if pins are shorted by sending a signal
		// If signal does not pass ==> it's a Kit PCB
		bool signal = false;
		for (unsigned i = 0; i < 4; i++) {
			loop_out_kit.set(signal);
			HAL_Delay(10);
			if (loop_in_built.read() != signal)
				_is_kit = true;
			signal = !signal;
		}

		if (!_is_kit) {
			// Init pins as Built
			Board::LoopClkBuilt loop_out_built;
			Board::LoopClkKitRead loop_in_kit;
		}

		return _is_kit;
	}
};

}; // namespace LDKit
