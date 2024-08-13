#include "peg_base.hh"


namespace MetaModule::PEG
{

void PEGBase::init_debouncer() {
	// debouncing not required
}

bool PEGBase::is_pressed(DebouncedDiginList pin) {
	switch (pin)
	{
		case DebouncedDiginList::PING_BUTTON: return digio.PingBut.is_pressed();
		case DebouncedDiginList::CYCLE_BUTTON: return digio.CycleBut.is_pressed();
		case DebouncedDiginList::TRIGGER_JACK: return digio.TrigJack.is_pressed();
		case DebouncedDiginList::CYCLE_JACK: return digio.CycleJack.is_pressed();
		// case DebouncedDiginList::PING_JACK: return digio.PingJack.is_pressed();
		default: return false;
	}
}

bool PEGBase::just_pressed(DebouncedDiginList pin) {
	switch (pin)
	{
		case DebouncedDiginList::PING_BUTTON: return digio.PingBut.is_just_pressed();
		case DebouncedDiginList::CYCLE_BUTTON: return digio.CycleBut.is_just_pressed();
		case DebouncedDiginList::TRIGGER_JACK: return digio.TrigJack.is_just_pressed();
		case DebouncedDiginList::CYCLE_JACK: return digio.CycleJack.is_just_pressed();
		// case DebouncedDiginList::PING_JACK: return digio.PingJack.is_just_pressed();
		default: return false;
	}
}

bool PEGBase::just_released(DebouncedDiginList pin) {
	switch (pin)
	{
		case DebouncedDiginList::PING_BUTTON: return digio.PingBut.is_just_released();
		case DebouncedDiginList::CYCLE_BUTTON: return digio.CycleBut.is_just_released();
		case DebouncedDiginList::TRIGGER_JACK: return digio.TrigJack.is_just_released();
		case DebouncedDiginList::CYCLE_JACK: return digio.CycleJack.is_just_released();
		// case DebouncedDiginList::PING_JACK: return digio.PingJack.is_just_released();
		default: return false;
	}
}

}
