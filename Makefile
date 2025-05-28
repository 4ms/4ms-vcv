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


# 
# Generate SVGs, info headers
#

BRAND_SLUG := 4ms/fp

INFO_SVGS := $(notdir $(wildcard lib/CoreModules/4ms/svg/*_info.svg))
MODULES := $(INFO_SVGS:_info.svg=)
VCV_SVGS := $(addsuffix _artwork.svg,$(addprefix res/modules/,$(MODULES)))
INFO_HEADERS := $(addsuffix _info.hh,$(addprefix lib/CoreModules/4ms/info/,$(MODULES)))

# Call this if an info.svg file is updated
debug:
	$(info INFO SVGS: $(INFO_SVGS))
	$(info MODULES: $(MODULES))
	$(info VCV_SVGS: $(VCV_SVGS))
	$(info INFO_HEADERS: $(INFO_HEADERS))

update-images: vcv-svgs info-headers 

vcv-svgs: $(VCV_SVGS)

info-headers: $(INFO_HEADERS)

res/modules/%_artwork.svg: lib/CoreModules/4ms/svg/%_info.svg
	scripts/vcv-artwork.py --input $< --output $@

lib/CoreModules/4ms/info/%.hh: lib/CoreModules/4ms/svg/%.svg
	scripts/createinfo.py --input $< --outdir $(PWD)/lib/CoreModules/4ms/info --brand $(BRAND_SLUG)

