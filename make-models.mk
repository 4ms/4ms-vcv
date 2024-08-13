coremodules := $(SHARED)/CoreModules
svgscript := $(SHARED)/svgextract/svgextract.py
model_plugin_dir := src/models

models = $(subst _info.hh,,$(notdir $(wildcard $(coremodules)/4ms/info/*_info.hh)))
model_ccs = $(addsuffix .cc,$(addprefix $(model_plugin_dir)/,$(models)))

plugin-models: $(model_ccs)

$(model_plugin_dir)/%.cc:
	python3 $(svgscript) addToVcvPlugin $* 4ms

