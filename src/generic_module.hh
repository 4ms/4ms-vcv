#pragma once
#include "CoreModules/elements/element_counter.hh"
#include "CoreModules/moduleFactory.hh"
#include "comm/comm_module.hh"
#include "util/base_concepts.hh"
#include "widgets/vcv_module_creator.hh"
#include "widgets/vcv_widget_creator.hh"

template<Derived<MetaModule::ModuleInfoBase> Info>
struct GenericModule {
	static rack::Model *create() {
		return rack::createModel<Module, Widget>(Info::slug.data());
	}

	struct Module : CommModule {
		Module() {
			// Create processing core
			core = MetaModule::ModuleFactory::create(Info::slug);

			// Register with VCV the number of elements of each type
			auto cnt = ElementCount::count<Info>();
			configComm(cnt.num_params, cnt.num_inputs, cnt.num_outputs, cnt.num_lights);

			// Configure elements with VCV
			// this includes alt parameters
			MetaModule::VCVModuleParamCreator<Info> creator{this};
			for (auto &element : Info::Elements) {
				std::visit([&creator](auto &el) { creator.config_element(el); }, element);
			}

			// Bypass Routes
			for (auto route : Info::bypass_routes) {
				configBypass(route.input, route.output);
			}
		}
	};

	struct Widget : rack::app::ModuleWidget {
		CommModule *mainModule;

		Widget(CommModule *module)
			: mainModule{module} {
			using namespace rack;

			// link this widget to given module
			setModule(static_cast<Module *>(module));

			// use svg file as panel
			setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, Info::svg_filename.data())));

			// draw screws
			addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, 0)));
			addChild(createWidget<ScrewBlack>(rack::math::Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
			if (box.size.x > RACK_GRID_WIDTH * 7) // >7HP = 2 screws
			{
				addChild(createWidget<ScrewBlack>(rack::math::Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
				addChild(createWidget<ScrewBlack>(
					rack::math::Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
			}

			// create widgets from all elements
			MetaModule::VCVWidgetCreator<Info> creator(this, module);
			for (auto &element : Info::Elements) {
				std::visit([&creator](auto &el) { creator.create(el); }, element);
			}
		}

		// custom menu item that draws a child menu populated with entries
		// defined by the alt parameter elements of the module
		struct AltParamMenuTop : rack::ui::MenuItem {
			AltParamMenuTop(MetaModule::VCVWidgetCreator<Info> creator_)
				: creator(creator_) {
			}

			rack::ui::Menu *createChildMenu() override {
				auto childMenu = new rack::ui::Menu;

				// let each element render itself to the menu
				for (auto &element : Info::Elements) {
					std::visit([&childMenu, this](auto &el) { creator.renderToContextMenu(el, childMenu); }, element);
				}
				return childMenu;
			}

		private:
			MetaModule::VCVWidgetCreator<Info> creator;
		};

		void appendContextMenu(rack::ui::Menu *menu) override {
			MetaModule::VCVWidgetCreator<Info> creator(this, module);

			// add single entry with submenu
			// we need to forward the creator so the entry itself is able to create further menu items
			auto altParamItem = new AltParamMenuTop(creator);
			altParamItem->text = "Alt Parameters";
			altParamItem->rightText = RIGHT_ARROW;

			menu->addChild(altParamItem);
		}
	};
};
