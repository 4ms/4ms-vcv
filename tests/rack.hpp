#pragma once
#include "jansson.h"
#include <array>
#include <string>
#include <vector>

namespace rack::app
{
struct PortWidget {};
} // namespace rack::app

struct NVGcolor {
	union {
		float rgba[4];
		struct {
			float r, g, b, a;
		};
	};
};
typedef struct NVGcolor NVGcolor;

static NVGcolor dummyColor;
inline NVGcolor nvgRGB(unsigned char r, unsigned char g, unsigned char b) {
	return dummyColor;
}

namespace rack
{
struct Plugin {
	std::string slug;
	std::string version;
};
struct Model {
	std::string slug;
	Plugin *plugin;
};
struct Module {
	long dummy;
	Model *model;
	uint64_t id;
	json_t *dataToJson() {
		return nullptr;
	}
	Model *getModel() {
		return model;
	}
};

struct ParamHandle {
	int64_t moduleId = -1;
	int paramId = 0;
	Module *module = NULL;

	std::string text;
	NVGcolor color;
};

struct Cable {};

struct _Engine {
	void removeParamHandle(rack::ParamHandle *) {
	}
	void removeParamHandle_NoLock(rack::ParamHandle *) {
	}
	rack::ParamHandle *getParamHandle(int, int) {
		return {};
	}
	void updateParamHandle(rack::ParamHandle *, int, int, bool) {
	}
	void updateParamHandle_NoLock(rack::ParamHandle *, int, int, bool) {
	}
	void addParamHandle(rack::ParamHandle *) {
	}

	std::array<int64_t, 4> getModuleIds() {
		std::array<int64_t, 4> x{1, 2, 3, 4};
		return x;
	}

	Module modules[4];
	Module *getModule(int64_t id) {
		return &modules[id % 4];
	}
};

struct Context {
	static inline _Engine _engine;
	_Engine *engine = &_engine;
};

// Context *contextGet()
// {
// 	static Context _context;
// 	return &_context;
// }

} // namespace rack

struct _APP {
	rack::_Engine *engine;
};

static _APP oAPP;

// export to tests:
// _APP *APP = &oAPP;
