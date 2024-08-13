#pragma once
#include "hub/hub_module.hh"
#include <rack.hpp>

struct KnobAliasTextBox : rack::ui::TextField {
	MappableObj src;
	MetaModuleHubBase *hub;

	KnobAliasTextBox(MetaModuleHubBase *hub, MappableObj src)
		: src{src}
		, hub{hub} {
	}

	void onChange(const rack::event::Change &e) override {
		hub->mappings.setMapAliasName(src, text);
	}
};

struct KnobAliasMenuItem : rack::widget::Widget {
	MappableObj src;
	MetaModuleHubBase *hub;
	KnobAliasTextBox *txt;

	KnobAliasMenuItem(MetaModuleHubBase *hub, MappableObj src)
		: src{src}
		, hub{hub} {
		box.pos = {0, 0};
		box.size = {120, BND_WIDGET_HEIGHT};
		txt = new KnobAliasTextBox{hub, src};
		txt->box.pos = {45, 0};
		txt->box.size = {120 - txt->box.pos.x, BND_WIDGET_HEIGHT};
		txt->text = "";
		txt->text = hub->mappings.getMapAliasName(src);
		addChild(txt);
	}

	void draw(const DrawArgs &args) override {
		bndMenuLabel(args.vg, 0.0, 0.0, box.size.x, box.size.y, -1, "Alias:");
		Widget::draw(args);
	}
};
