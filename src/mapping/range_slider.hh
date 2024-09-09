#pragma once
#include "MappableObject.h"
#include "hub/hub_module.hh"
#include <rack.hpp>

namespace MetaModule
{

enum class RangePart { Min, Max };

template<RangePart MINMAX>
struct MappedRangeQuantity : rack::Quantity {
private:
	float _val{0.f};
	std::string _paramObj;
	MetaModuleHubBase *hub;
	const MappableObj _dst_id;

public:
	MappedRangeQuantity(MetaModuleHubBase *hub, MappableObj paramObj)
		: _paramObj{MINMAX == RangePart::Min ? "Min: " : "Max"}
		, hub{hub}
		, _dst_id{paramObj} {
	}

	void setValue(float value) override {
		_val = MathTools::constrain(value, 0.0f, 1.0f);
		if constexpr (MINMAX == RangePart::Min)
			hub->mappings.setRangeMin(_dst_id, _val);
		else
			hub->mappings.setRangeMax(_dst_id, _val);
	}

	float getValue() override {
		if constexpr (MINMAX == RangePart::Min)
			return hub->mappings.getRange(_dst_id).first;
		else
			return hub->mappings.getRange(_dst_id).second;
	}

	// clang-format off
	float getMinValue() override { return 0; }
	float getMaxValue() override { return 1; }
	float getDefaultValue() override { return 0.0; }
	float getDisplayValue() override { return getValue() * 100.f; }
	void setDisplayValue(float displayValue) override { setValue(displayValue / 100.f); }
	std::string getLabel() override { return _paramObj; }
	std::string getUnit() override { return "%"; }
	// clang-format on
};

template<RangePart MINMAX>
struct RangeSlider : rack::ui::Slider {

public:
	RangeSlider(MetaModuleHubBase *hub, MappableObj paramObj) {
		quantity = new MappedRangeQuantity<MINMAX>{hub, paramObj};
	}
	~RangeSlider() {
		delete quantity;
	}
};

} // namespace MetaModule
