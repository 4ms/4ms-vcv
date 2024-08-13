#pragma once
#include "plugin.hh"

namespace MetaModule
{

class MetaModuleTextBox : public rack::LedDisplayTextField {
	void draw(const DrawArgs &args) override {
		rack::LedDisplayTextField::draw(args);
	}
};

struct Davies1900hBlackKnob4ms : rack::Davies1900hKnob {
	Davies1900hBlackKnob4ms() {
		setSvg(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/Davies1900hBlack.svg")));
	}
};

struct Davies1900hBlackKnobUnlined4ms : rack::Davies1900hKnob {
	Davies1900hBlackKnobUnlined4ms() {
		setSvg(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/Davies1900hBlack_no_pip.svg")));
	}
};

struct DaviesLarge4ms : rack::Davies1900hKnob {
	DaviesLarge4ms() {
		setSvg(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/Davies_large.svg")));
	}
};

struct Small9mmKnob : rack::RoundKnob {
	Small9mmKnob() {
		setSvg(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/9mm_knob.svg")));
	}
};

struct Small9mmUnlinedKnob : rack::RoundKnob {
	Small9mmUnlinedKnob() {
		setSvg(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/9mm_knob_no_pip.svg")));
	}
};

struct SubMiniToggle3pos : rack::app::SvgSwitch {
	SubMiniToggle3pos() {
		addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/SubMiniToggle_0.svg")));
		addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/SubMiniToggle_1.svg")));
		addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/SubMiniToggle_2.svg")));
	}
};

struct SubMiniToggleHoriz3pos : rack::app::SvgSwitch {
	SubMiniToggleHoriz3pos() {
		addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/SubMiniToggleH_0.svg")));
		addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/SubMiniToggleH_1.svg")));
		addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/SubMiniToggleH_2.svg")));
	}
};

struct SubMiniToggle2pos : rack::app::SvgSwitch {
	SubMiniToggle2pos() {
		addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/SubMiniToggle_0.svg")));
		addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/SubMiniToggle_2.svg")));
	}
};

struct SubMiniToggleHoriz2pos : rack::app::SvgSwitch {
	SubMiniToggleHoriz2pos() {
		addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/SubMiniToggleH_0.svg")));
		addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance, "res/components/SubMiniToggleH_2.svg")));
	}
};

template<typename TSwitch>
struct LatchingSwitch : TSwitch {
	LatchingSwitch() {
		this->momentary = false;
	}
};

template<typename TBase>
struct FourmsSliderLight : rack::RectangleLight<rack::TSvgLight<TBase>> {
	FourmsSliderLight() {
		this->setSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/components/FourmsSliderLight.svg")));
	}
};

struct FourmsSlider : rack::app::SvgSlider {
	FourmsSlider() {
		setBackgroundSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/components/FourmsSlider.svg")));
		setHandleSvg(rack::Svg::load(rack::asset::plugin(pluginInstance, "res/components/FourmsSliderHandle.svg")));
		setHandlePosCentered(rack::math::Vec(19.84260 / 2, 76.53517 - 11.74218 / 2),
							 rack::math::Vec(19.84260 / 2, 0.0 + 11.74218 / 2));
		// maxHandlePos = rack::mm2px(rack::math::Vec(22.078, 0.738));
		// minHandlePos = rack::mm2px(rack::math::Vec(0.738, 0.738));
	}
};

template<typename TLightBase = rack::RedLight>
struct FourmsLightSlider : rack::LightSlider<FourmsSlider, FourmsSliderLight<TLightBase>> {
	FourmsLightSlider() = default;
};

template<typename TBase>
struct FourmsSliderHorizontalLight : rack::RectangleLight<rack::TSvgLight<TBase>> {
	FourmsSliderHorizontalLight() {
		this->setSvg(
			rack::Svg::load(rack::asset::plugin(pluginInstance, "res/components/FourmsSliderLightHorizontal.svg")));
	}
};

struct FourmsSliderHorizontal : rack::app::SvgSlider {
	FourmsSliderHorizontal() {
		horizontal = true;
		maxHandlePos = rack::mm2px(rack::math::Vec(22.078, 0.738));
		minHandlePos = rack::mm2px(rack::math::Vec(0.738, 0.738));
		setBackgroundSvg(
			rack::Svg::load(rack::asset::plugin(pluginInstance, "res/components/FourmsSliderHorizontal.svg")));
	}
};

template<typename TLightBase = rack::RedLight>
struct FourmsLightSliderHorizontal :
	rack::LightSlider<FourmsSliderHorizontal, FourmsSliderHorizontalLight<TLightBase>> {
	FourmsLightSliderHorizontal() {
		// TODO Fix positions
		this->light->box.size = rack::mm2px(rack::math::Vec(3.276, 1.524));
		// TODO Fix SVG
		this->setHandleSvg(
			rack::Svg::load(rack::asset::plugin(pluginInstance, "res/components/FourmsSliderHorizontalHandle.svg")));
	}
};

template<typename TBase = rack::GrayModuleLightWidget>
struct TRedBlueLight : TBase {
	TRedBlueLight() {
		this->addBaseColor(rack::SCHEME_RED);
		this->addBaseColor(rack::SCHEME_BLUE);
	}
};
using RedBlueLightWidget = TRedBlueLight<>;

template<typename TBase = rack::GrayModuleLightWidget>
struct TOrangeLight : TBase {
	TOrangeLight() {
		this->addBaseColor(rack::SCHEME_YELLOW);
	}
};
using OrangeLightWidget = TOrangeLight<>;

} // namespace MetaModule
