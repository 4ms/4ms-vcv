#pragma once
#include "mapping/module_directory.hh"

struct KnobNameMenuLabel : rack::ui::MenuLabel {
	rack::ParamQuantity *paramQty;
	void step() override {
		text = paramQty->getLabel();
		rack::ui::MenuLabel::step();
	}
};

struct MappedKnobMenuLabel : rack::ui::MenuLabel {
	int64_t moduleId;
	int paramId;
	std::string moduleName;
	std::string paramName;

	void step() override {
		if (moduleName.empty())
			moduleName = std::to_string(moduleId);
		if (paramName.empty())
			paramName = std::to_string(paramId);

		text = "Mapped to: " + moduleName + " " + paramName;
		rack::ui::MenuLabel::step();
	}
};

struct KnobValueTextBox : rack::ui::TextField {
	rack::ParamQuantity *paramQuantity;
	KnobValueTextBox(rack::ParamQuantity *paramQ)
		: paramQuantity{paramQ} {
		if (paramQuantity)
			text = paramQuantity->getDisplayValueString();
		selectAll();
	}

	void onChange(const rack::event::Change &e) override {
		if (paramQuantity) {
			float oldValue = paramQuantity->getValue();
			paramQuantity->setDisplayValueString(text);
			float newValue = paramQuantity->getValue();

			if (oldValue != newValue) {
				// Push ParamChange history action
				rack::history::ParamChange *h = new rack::history::ParamChange;
				h->moduleId = paramQuantity->module->id;
				h->paramId = paramQuantity->paramId;
				h->oldValue = oldValue;
				h->newValue = newValue;
				APP->history->push(h);
			}
		}
	}
};

struct KnobValueMenuItem : rack::widget::Widget {
	KnobValueTextBox *txt;

	KnobValueMenuItem(float width, float relative_width, rack::ParamQuantity *paramQ) {
		box.pos = {0, 0};
		box.size = {width, BND_WIDGET_HEIGHT};
		txt = new KnobValueTextBox{paramQ};
		txt->box.pos = {relative_width * width, 0};
		txt->box.size = {(1.f - relative_width) * width, BND_WIDGET_HEIGHT};
		addChild(txt);
	}

	void draw(const DrawArgs &args) override {
		bndMenuLabel(args.vg, 0.0, 0.0, box.size.x, box.size.y, -1, "Value:");
		rack::widget::Widget::draw(args);
	}
};
