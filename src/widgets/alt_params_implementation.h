#include "helpers/4ms_elements.hh"
#include "osdialog.h"
#include "rack.hpp"
#include "widgets/vcv_creation_context.hh"
#include <CoreModules/elements/element_state_conversion.hh>
#include <algorithm>

namespace MetaModule
{
extern std::string last_file_path;
}

namespace MetaModule::VCVImplementation::Widget
{

/*
 * Leaf item in an alt paramter context menu to select a certain choice
 * for a single paramter on a module instance
 */

struct AltParamChoiceItem : rack::ui::MenuItem {
	AltParamChoiceItem(rack::Module *module_,
					   std::size_t param_idx_,
					   AltParamChoiceLabeled el_,
					   std::size_t choiceIndex_)
		: module(module_)
		, param_idx(param_idx_)
		, el(el_)
		, choiceIndex(choiceIndex_) {};
	void onAction(const ActionEvent &e) override {
		e.unconsume();

		module->getParam(param_idx).setValue(choiceIndex);
	}
	void draw(const DrawArgs &args) override {
		// add checkmark if this choice is selected
		auto currentState = std::clamp<unsigned>(std::round(module->getParam(param_idx).getValue()), 0, el.num_pos - 1);
		rightText = currentState == choiceIndex ? CHECKMARK_STRING : " ";

		rack::ui::MenuItem::draw(args);
	}

private:
	rack::Module *module;
	std::size_t param_idx;
	AltParamChoiceLabeled el;
	std::size_t choiceIndex;
};

/*
 * Submenu that holds all options for a single alt paramter on a module instance
 */

struct AltParamChoiceLabeledMenu : rack::ui::MenuItem {
	AltParamChoiceLabeledMenu(rack::Module *module_, std::size_t param_idx_, AltParamChoiceLabeled el)
		: module(module_)
		, param_idx(param_idx_)
		, element(el) {
	}

	rack::ui::Menu *createChildMenu() override {
		auto childMenu = new rack::ui::Menu;

		for (std::size_t i = 0; i < element.num_pos; i++) {
			auto choiceItem = new AltParamChoiceItem(module, param_idx, element, i);
			auto choiceText = element.pos_names[i];
			choiceItem->text = std::string(choiceText);
			childMenu->addChild(choiceItem);
		}
		return childMenu;
	}

private:
	rack::Module *module;
	std::size_t param_idx;
	AltParamChoiceLabeled element;
};

struct FileBrowseActionMenuItem : rack::ui::MenuItem {
	rack::engine::Param *param;
	std::string filterext;

	FileBrowseActionMenuItem(rack::Param &param, std::string_view filterext)
		: param{&param}
		, filterext{filterext} {
	}

	void onAction(const ActionEvent &e) override {
		if (param) {
			// Get filters from AltParamAction element (std::string_view argument)
			osdialog_filters *filters = nullptr;

			if (filterext == ".wav, .WAV")
				filters = osdialog_filters_parse("WAV (.wav):wav,WAV");

			if (char *path = osdialog_file(OSDIALOG_OPEN, "", nullptr, filters)) {
				// Set global
				last_file_path = path;
				free(path);

				// Set the param, which notifies the module that a path is available
				param->setValue(1);
			}

			if (filters)
				osdialog_filters_free(filters);
		}
	}
};

/*
 * Slider that snaps to integer values
*/

struct QuantizedSlider : public rack::ui::Slider {
	void onDragMove(const DragMoveEvent &e) override {
		if (quantity) {

			// This is hardcoded in the rack source code and copied here
			constexpr float SENSITIVITY = 0.001f;

			const float QuantizationSize = 1.0f / quantity->getMaxValue();

			auto quantityIncrement = SENSITIVITY * e.mouseDelta.x;
			accumulatedDrag += quantityIncrement;

			if (std::abs(accumulatedDrag) >= QuantizationSize) {
				auto thisDrag = accumulatedDrag > 0 ? QuantizationSize : -QuantizationSize;
				accumulatedDrag -= thisDrag;

				auto newValue = std::round(quantity->getValue() + thisDrag * quantity->getMaxValue());
				quantity->setValue(newValue);
			}
		}
	}

	void draw(const DrawArgs &args) override {
		BNDwidgetState state = BND_DEFAULT;
		if (APP->event->hoveredWidget == this)
			state = BND_HOVER;
		if (APP->event->draggedWidget == this)
			state = BND_ACTIVE;

		float progress = quantity ? quantity->getScaledValue() : 0.f;
		std::string text = quantity ? std::to_string(int(quantity->getValue() + 1)) : "";
		bndSlider(args.vg, 0.0, 0.0, box.size.x, box.size.y, BND_CORNER_NONE, state, progress, text.c_str(), NULL);
	}

	float accumulatedDrag = 0.0f;
};

/*
 * Implementations for rendering the context menu entries for certain alt param elements types
 */

inline void
do_render_to_menu(AltParamContinuous el, rack::ui::Menu *menu, Indices &indices, const WidgetContext_t &context) {
	auto slider = new rack::ui::Slider;
	slider->quantity = context.module->getParamQuantity(indices.param_idx);

	// hardcoded slider with according to contained text
	slider->box.size.x = slider->quantity->getString().size() * 12;
	menu->addChild(slider);
}

inline void
do_render_to_menu(AltParamChoice el, rack::ui::Menu *menu, Indices &indices, const WidgetContext_t &context) {
	auto slider = new QuantizedSlider;
	slider->quantity = context.module->getParamQuantity(indices.param_idx);

	// hardcoded slider with according to contained text
	slider->box.size.x = slider->quantity->getString().size() * 12;
	menu->addChild(slider);
}

inline void
do_render_to_menu(AltParamChoiceLabeled el, rack::ui::Menu *menu, Indices &indices, const WidgetContext_t &context) {
	auto *item = new AltParamChoiceLabeledMenu(context.module, indices.param_idx, el);
	item->text = el.short_name;
	item->rightText = RIGHT_ARROW;
	menu->addChild(item);
}

inline void
do_render_to_menu(AltParamAction el, rack::ui::Menu *menu, Indices &indices, const WidgetContext_t &context) {
	auto &param = context.module->getParam(indices.param_idx);
	auto menu_item = new FileBrowseActionMenuItem(param, el.args[0]);
	menu_item->text = el.short_name.data();
	menu->addChild(menu_item);
}

} // namespace MetaModule::VCVImplementation::Widget
