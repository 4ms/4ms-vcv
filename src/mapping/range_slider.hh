#pragma once
#include "MappableObject.h"
#include "hub/hub_module.hh"
#include <rack.hpp>
#include <cmath>
#include <cstdio>
#include <functional>
#include <tinyexpr.h>

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

// Internal slider - takes quantity from parent, doesn't own it
template<RangePart MINMAX>
struct RangeSlider : rack::ui::Slider {
	std::function<void()> onEditRequested;

	RangeSlider(MappedRangeQuantity<MINMAX> *qty) {
		quantity = qty;
	}
	~RangeSlider() {
		quantity = nullptr; // Don't delete - owned by parent
	}

	void onDoubleClick(const DoubleClickEvent &e) override {
		if (onEditRequested) {
			onEditRequested();
			e.consume(this);
			return;
		}
		rack::ui::Slider::onDoubleClick(e);
	}
};

// Text field for precise value entry
struct RangeTextField : rack::ui::TextField {
	std::function<void(float)> onSubmit;
	std::function<void()> onCancel;
	bool submitting = false;
	bool cancelling = false;

	void onSelectKey(const SelectKeyEvent &e) override {
		if (e.action == GLFW_PRESS || e.action == GLFW_REPEAT) {
			if (e.key == GLFW_KEY_ENTER || e.key == GLFW_KEY_KP_ENTER) {
				submitValue();
				e.consume(this);
				return;
			}
			if (e.key == GLFW_KEY_ESCAPE) {
				cancelValue();
				e.consume(this);
				return;
			}
		}
		rack::ui::TextField::onSelectKey(e);
	}

	void onDeselect(const DeselectEvent &e) override {
		if (!cancelling)
			submitValue();
		cancelling = false;
		rack::ui::TextField::onDeselect(e);
	}

private:
	void cancelValue() {
		cancelling = true;
		if (onCancel)
			onCancel();
	}

	void submitValue() {
		if (submitting)
			return;
		submitting = true;

		double val = te_interp(text.c_str(), nullptr);
		if (std::isnan(val)) {
			if (onCancel)
				onCancel();
		} else {
			if (onSubmit)
				onSubmit(static_cast<float>(val));
		}

		submitting = false;
	}
};

// Composite widget: slider + text field, toggles on double-click
template<RangePart MINMAX>
struct EditableRangeSlider : rack::widget::Widget {
private:
	MappedRangeQuantity<MINMAX> *quantity = nullptr;
	RangeSlider<MINMAX> *slider = nullptr;
	RangeTextField *textField = nullptr;

public:
	EditableRangeSlider(MetaModuleHubBase *hub, MappableObj paramObj) {
		quantity = new MappedRangeQuantity<MINMAX>{hub, paramObj};

		slider = new RangeSlider<MINMAX>(quantity);
		slider->onEditRequested = [this]() { switchToTextMode(); };
		slider->box.pos = rack::math::Vec(0, 0);
		addChild(slider);

		textField = new RangeTextField();
		textField->box.pos = rack::math::Vec(0, 0);
		textField->hide();
		addChild(textField);

		textField->onSubmit = [this](float displayVal) {
			displayVal = rack::math::clamp(displayVal, 0.f, 100.f);
			quantity->setDisplayValue(displayVal);
			switchToSliderMode();
		};

		textField->onCancel = [this]() { switchToSliderMode(); };

		box.size = rack::math::Vec(100, 20);
	}

	~EditableRangeSlider() {
		delete quantity;
	}

	void onButton(const ButtonEvent &e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
			switchToTextMode();
			e.consume(this);
			return;
		}

		rack::widget::Widget::onButton(e);

		if (!e.isConsumed() && e.button == GLFW_MOUSE_BUTTON_LEFT)
			e.consume(this);
	}

	void draw(const DrawArgs &args) override {
		slider->box.size = box.size;
		textField->box.size = box.size;
		rack::widget::Widget::draw(args);
	}

private:
	void switchToTextMode() {
		slider->hide();

		// Pre-populate with current value
		char buf[16];
		std::snprintf(buf, sizeof(buf), "%.1f", quantity->getDisplayValue());
		textField->setText(buf);
		textField->selectAll();
		textField->show();

		APP->event->setSelectedWidget(textField);
	}

	void switchToSliderMode() {
		textField->hide();
		slider->show();
		APP->event->setSelectedWidget(nullptr);
	}
};

} // namespace MetaModule
