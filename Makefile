#Makefile based on https://github.com/baconpaul/airwin2rack/blob/main/Makefile

RACK_DIR ?= ../..
include $(RACK_DIR)/arch.mk

EXTRA_CMAKE :=
RACK_PLUGIN := plugin.so

ifdef ARCH_WIN
  RACK_PLUGIN := plugin.dll
endif

ifdef ARCH_MAC
  EXTRA_CMAKE := -DCMAKE_OSX_ARCHITECTURES="x86_64"
  RACK_PLUGIN := plugin.dylib
  ifdef ARCH_ARM64
    EXTRA_CMAKE := -DCMAKE_OSX_ARCHITECTURES="arm64"
  endif
  EXTRA_CMAKE += -DCMAKE_OSX_DEPLOYMENT_TARGET="10.15"
endif

CMAKE_BUILD ?= build
cmake_rack_plugin := $(CMAKE_BUILD)/$(RACK_PLUGIN)

$(info cmake_rack_plugin target is '$(cmake_rack_plugin)')

# create empty plugin lib to skip the make target execution
$(shell touch $(RACK_PLUGIN))

# trigger CMake build when running `make dep`
DEPS += $(cmake_rack_plugin)

$(cmake_rack_plugin): CMakeLists.txt
	$(CMAKE) -B $(CMAKE_BUILD) -DRACK_SDK_DIR=$(RACK_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(CMAKE_BUILD)/dist $(EXTRA_CMAKE)
	cmake --build $(CMAKE_BUILD) -- -j $(shell getconf _NPROCESSORS_ONLN)
	cmake --install $(CMAKE_BUILD)

rack_plugin: $(cmake_rack_plugin)
	cp -vf $(cmake_rack_plugin) .

# Add files to the ZIP package when running `make dist`
dist: rack_plugin res

configure:
	$(CMAKE) --fresh -B $(CMAKE_BUILD) -DRACK_SDK_DIR=$(RACK_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(CMAKE_BUILD)/dist $(EXTRA_CMAKE)

DISTRIBUTABLES += $(wildcard LICENSE*) res README.md 

# Include the VCV plugin Makefile framework
include $(RACK_DIR)/plugin.mk

.PHONY: tests

tests:
	@$(MAKE) --no-print-directory -f tests/Makefile
