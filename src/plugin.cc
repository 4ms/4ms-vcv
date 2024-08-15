#include "plugin.hh"

rack::Plugin *pluginInstance;

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
	p->addModel(modelVCAM);
	p->addModel(modelVerb);
}
