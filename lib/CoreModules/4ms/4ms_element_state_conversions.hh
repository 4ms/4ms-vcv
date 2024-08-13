#pragma once

#include "4ms_elements.hh"
#include <cmath>


namespace MetaModule::StateConversion
{

// FIXME: These are never called because convertState() is only called for direct members of the Element variant, not derived types

template<typename T>
constexpr Toggle2pos::State_t convertState(const T &, float val) requires(std::derived_from<T, Toggle2pos>)
{
	if (val < 0.5f) {
		return Toggle2pos::State_t::DOWN;
	} else {
		return Toggle2pos::State_t::UP;
	}
}

template<typename T>
constexpr Toggle3pos::State_t convertState(const T &, float val) requires(std::derived_from<T, Toggle3pos>)
{
	if (val < 0.25f) {
		return Toggle3pos::State_t::DOWN;
	} else if (val < 0.75f) {
		return Toggle3pos::State_t::CENTER;
	} else {
		return Toggle3pos::State_t::UP;
	}
}

template<typename T>
constexpr Toggle2posHoriz::State_t convertState(const T &, float val) requires(std::derived_from<T, Toggle2posHoriz>)
{
	if (val < 0.5f) {
		return Toggle2posHoriz::State_t::LEFT;
	} else {
		return Toggle2posHoriz::State_t::RIGHT;
	}
}

template<typename T>
constexpr Toggle3posHoriz::State_t convertState(const T &, float val) requires(std::derived_from<T, Toggle3posHoriz>)
{
	if (val < 0.25f) {
		return Toggle3posHoriz::State_t::LEFT;
	} else if (val < 0.75f) {
		return Toggle3posHoriz::State_t::CENTER;
	} else {
		return Toggle3posHoriz::State_t::RIGHT;
	}
}

}
