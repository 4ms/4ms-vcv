#pragma once
#include <rack.hpp>

namespace MetaModule
{

// Clears text after a delay
class LabelDelay : public rack::Label {
public:
	unsigned timeToHide = 0;

	void step() override {
		if (timeToHide) {
			if (--timeToHide == 0) {
				this->text = "";
			}
		}
		rack::Label::step();
	}
};
} // namespace MetaModule
