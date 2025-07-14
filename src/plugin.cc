#include "plugin.hh"
#include "thread/async_thread_control.hh"

rack::Plugin *pluginInstance;

namespace MetaModule
{
std::string last_file_path;
}

__attribute__((__visibility__("default"))) void init(rack::Plugin *p) {
	pluginInstance = p;

	//////////////// Auto generated ///////////////////////
	// Add models below here
	p->addModel(modelAtvert2);
	p->addModel(modelBPF);
	p->addModel(modelCLKD);
	p->addModel(modelCLKM);
	p->addModel(modelComplexEG);
	p->addModel(modelDEV);
	p->addModel(modelDLD);
	p->addModel(modelDetune);
	p->addModel(modelDjembe);
	p->addModel(modelDrum);
	p->addModel(modelENVVCA);
	p->addModel(modelEnOsc);
	p->addModel(modelFM);
	p->addModel(modelFollow);
	p->addModel(modelFreeverb);
	p->addModel(modelGate);
	p->addModel(modelHPF);
	p->addModel(modelHubMedium);
	p->addModel(modelKPLS);
	p->addModel(modelL4);
	p->addModel(modelLPG);
	p->addModel(modelMNMX);
	p->addModel(modelMPEG);
	p->addModel(modelMultiLFO);
	p->addModel(modelNoise);
	p->addModel(modelOctave);
	p->addModel(modelPEG);
	p->addModel(modelPI);
	p->addModel(modelPan);
	p->addModel(modelPitchShift);
	p->addModel(modelProb8);
	p->addModel(modelQCD);
	p->addModel(modelQPLFO);
	p->addModel(modelRCD);
	p->addModel(modelSCM);
	p->addModel(modelSH);
	p->addModel(modelSHEV);
	p->addModel(modelSISM);
	p->addModel(modelSeq8);
	p->addModel(modelSlew);
	p->addModel(modelSource);
	p->addModel(modelStMix);
	p->addModel(modelSwitch14);
	p->addModel(modelSwitch41);
	p->addModel(modelTapo);
	p->addModel(modelTSP);
	p->addModel(modelVCAM);
	p->addModel(modelVerb);
	p->addModel(modelMMAudioExpander);
	p->addModel(modelMMButtonExpander);

	MetaModule::Async::start_module_threads();
}

extern "C" void destroy() {
	printf("plugin destroy\n");
	MetaModule::Async::kill_module_threads();
}

__attribute__((__visibility__("default"))) extern "C" json_t *settingsToJson() {
	json_t *rootJ = json_object();
	json_object_set_new(rootJ, "wifiUrl", json_string(MetaModule::wifiUrl.c_str()));
	json_object_set_new(rootJ, "wifiPath", json_integer((unsigned)MetaModule::wifiVolume));
	return rootJ;
}

__attribute__((__visibility__("default"))) extern "C" void settingsFromJson(json_t *rootJ) {
	if (auto wifiUrlJ = json_object_get(rootJ, "wifiUrl"))
		MetaModule::wifiUrl = json_string_value(wifiUrlJ);

	if (auto wifiVolJ = json_object_get(rootJ, "wifiPath")) {
		auto val = json_integer_value(wifiVolJ);

		if (val == (unsigned)MetaModule::Volume::USB)
			MetaModule::wifiVolume = MetaModule::Volume::USB;

		else if (val == (unsigned)MetaModule::Volume::Card)
			MetaModule::wifiVolume = MetaModule::Volume::Card;

		else if (val == (unsigned)MetaModule::Volume::Internal)
			MetaModule::wifiVolume = MetaModule::Volume::Internal;
	}
}
