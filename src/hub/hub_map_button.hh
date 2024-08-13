#pragma once
#include "hub/hub_module.hh"
#include <rack.hpp>

class HubMapButton : public rack::Button {

public:
	HubMapButton(MetaModuleHubBase *hub, rack::app::ModuleWidget &parent)
		: parent{parent}
		, hub{hub} {
	}
	void draw(const DrawArgs &args) override;
	void onDragStart(const rack::event::DragStart &e) override;
	void onHover(const rack::event::Hover &e) override;
	void onLeave(const rack::event::Leave &e) override;
	void onEnter(const rack::event::Enter &e) override;

	void start_mapping();

	MappableObj hubParamObj;
	bool isMapped = false;
	MappableObj mappedToId{MappableObj::Type::None, -1, -1};

	// void _createMapping(LabelButtonID srcId);
protected:
	bool hovered{false};
	rack::app::ModuleWidget &parent;
	MetaModuleHubBase *hub;
};
