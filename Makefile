# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..

SHARED = ../shared

## Cpputil
INCLUDES += -I$(SHARED)/cpputil
SOURCES += $(wildcard $(SHARED)/cpputil/util/*.cc)

## Rapidyaml
RYMLDIR = $(SHARED)/patch_convert/ryml/rapidyaml
INCLUDES += -I$(RYMLDIR)/src
INCLUDES += -I$(RYMLDIR)/ext/c4core/src
SOURCES += $(wildcard $(RYMLDIR)/src/c4/yml/*.cpp)
SOURCES += $(wildcard $(RYMLDIR)/ext/c4core/src/c4/*.cpp)

## Patch serialization
INCLUDES += -I$(SHARED)/patch_convert/ryml
SOURCES += $(SHARED)/patch_convert/ryml/ryml_serial.cc
SOURCES += $(SHARED)/patch_convert/ryml/ryml_init.cc
SOURCES += $(SHARED)/patch_convert/patch_to_yaml.cc

# Modules
INCLUDES += -Isrc
INCLUDES += -I$(SHARED)
#FIXME: path to firmware dir
INCLUDES += -I../firmware/metamodule-plugin-sdk/metamodule-core-interface
INCLUDES += -I$(SHARED)/CoreModules/4ms
# SOURCES += $(SHARED)/CoreModules/moduleFactory.cc
SOURCES += $(wildcard $(SHARED)/CoreModules/4ms/core/*Core.cc)

# Ensemble Oscillator
SOURCES += $(SHARED)/CoreModules/4ms/core/enosc/data.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/enosc/dynamic_data.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/enosc/easiglib/numtypes.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/axoloti-wrapper/axoloti_math.cpp

# Tapographic Delay
INCLUDES += -I$(SHARED)/CoreModules/4ms/core/tapo/stmlib
INCLUDES += -I$(SHARED)/CoreModules/4ms/core/tapo/
INCLUDES += -I$(SHARED)/CoreModules/4ms/core/alpaca/include
CXXFLAGS += -DTEST
SOURCES  += $(SHARED)/CoreModules/4ms/core/tapo/multitap_delay.cc
SOURCES  += $(SHARED)/CoreModules/4ms/core/tapo/resources.cc
SOURCES  += $(SHARED)/CoreModules/4ms/core/tapo/tap_allocator.cc
SOURCES  += $(SHARED)/CoreModules/4ms/core/tapo/control.cc
SOURCES  += $(SHARED)/CoreModules/4ms/core/tapo/ui.cc
SOURCES  += $(SHARED)/CoreModules/4ms/core/tapo/stmlib/utils/random.cc
SOURCES  += $(SHARED)/CoreModules/4ms/core/tapo/stmlib/dsp/units.cc

# plugin
SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard src/*.cc)
SOURCES += $(wildcard src/hub/*.cc)
SOURCES += $(wildcard src/comm/*.cc)
SOURCES += $(wildcard src/mapping/*.cc)
SOURCES += $(wildcard src/models/*.cc)

# DLD
INCLUDES += -I$(SHARED)/CoreModules/4ms/core/looping-delay/src
SOURCES  += $(SHARED)/CoreModules/4ms/core/looping-delay/src/calibration_storage.cc

# QPLFO
SOURCES += $(SHARED)/CoreModules/4ms/core/qplfo/qplfo.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/qplfo/qplfo_mocks.cc

# MiniPEG
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/peg_base.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/trigout.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/analog_conditioning.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/env_update.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/envelope_calcs.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/flash_user.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/params.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/pingable_env.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/dig_inouts.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/timers.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/calibration.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/env_transition.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/leds.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/shareddrv/debounced_digins.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/shareddrv/dac.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg-common/pwm.cc

SOURCES += $(SHARED)/CoreModules/4ms/core/mpeg/envelope_calcs.cc
SOURCES += $(SHARED)/CoreModules/4ms/core/peg/envelope_calcs.cc

INCLUDES += -I$(SHARED)/CoreModules/4ms/core/
INCLUDES += -I$(SHARED)/CoreModules/4ms/core/peg-common/
INCLUDES += -I$(SHARED)/CoreModules/4ms/core/peg-common/mocks

# FLAGS will be passed to both the C and C++ compiler
FLAGS += -std=c++2a 
FLAGS += $(INCLUDES)
FLAGS += -g2

CFLAGS +=
CXXFLAGS += -DVCVRACK

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS +=

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)

all: debug

debug:
	$(CXX) --version

clean:
	$(CXX) --version


# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
CXXFLAGS += -mmacosx-version-min=10.15 
endif

dynamic_data.cc.o: CXXFLAGS+=-Wno-unknown-pragmas

all: plugin res

include make-models.mk

plugin: $(TARGET)

run: install
	$(shell $(RACK_DIR)/Rack &)
	
compile_commands:
	rm -rf build
	compiledb make -j16 all
	compdb -p ./ list > compile_commands_with_headers.json 2>/dev/null
	rm compile_commands.json 
	mv compile_commands_with_headers.json compile_commands.json

tests: core_tests util_tests vcv_tests axo_tests patch_convert_tests

core_tests:
	$(info ... Running CoreModule tests ...)
	@cd $(SHARED)/CoreModules && $(MAKE) --no-print-directory -f tests/Makefile

util_tests:
	$(info ... Running shared/cpputil tests ...)
	@cd $(SHARED)/cpputil && $(MAKE) --no-print-directory -f tests/Makefile

vcv_tests:
	$(info ... Running vcv plugin tests ...)
	@$(MAKE) --no-print-directory -f tests/Makefile

axo_tests:
	$(info ... Running Axoloti-wrapper tests ...)
	@cd $(SHARED)/CoreModules/4ms/core/axoloti-wrapper && $(MAKE) --no-print-directory -f tests/Makefile

patch_convert_tests:
	$(info ... Running patch convert (yaml<-->patch) tests ...)
	@cd $(SHARED)/patch_convert && $(MAKE) --no-print-directory -f tests/Makefile

win_dist:
	CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ STRIP=x86_64-w64-mingw32-strip RACK_DIR=../../Rack-SDK make -j16 dist

clean_tests:
	@cd $(SHARED)/CoreModules && $(MAKE) --no-print-directory -f tests/Makefile clean
	@cd $(SHARED)/cpputil && $(MAKE) --no-print-directory -f tests/Makefile clean
	@$(MAKE) --no-print-directory -f tests/Makefile clean
	@cd $(SHARED)/CoreModules/4ms/core/axoloti-wrapper && $(MAKE) --no-print-directory -f tests/Makefile clean
	@cd $(SHARED)/patch_convert && $(MAKE) --no-print-directory -f tests/Makefile clean

.PHONY: all plugin tests core_tests vcv_tests util_tests axo_tests patch_convert_tests compile_commands run

