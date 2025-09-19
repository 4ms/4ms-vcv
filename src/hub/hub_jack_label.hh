#pragma once
#include "hub/hub_module.hh"
#include <rack.hpp>

namespace MetaModule
{

class HubJackLabel : public rack::TransparentWidget {

public:
	enum class JackDir { Input, Output };

	HubJackLabel(MetaModuleHubBase *hub, int jack_id, JackDir dir)
		: hub{hub}
		, jack_id{jack_id}
		, dir{dir} {
	}

	void draw(const DrawArgs &args) override;

	std::string text;

private:
	MetaModuleHubBase *hub;
	int jack_id;
	JackDir dir;
};

} // namespace MetaModule
