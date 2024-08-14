#include "plugin.hh"

rack::Plugin *pluginInstance;

__attribute__((__visibility__("default"))) void init(rack::Plugin *p) {
	pluginInstance = p;

	//////////////// Auto generated ///////////////////////
	// Add models below here
	p->addModel(modelSHEV);
	p->addModel(modelENVVCA);
	p->addModel(modelDEV);
	p->addModel(modelFreeverb);
	p->addModel(modelVCAM);
	p->addModel(modelTapo);
	p->addModel(modelSISM);
	p->addModel(modelSH);
	p->addModel(modelSCM);
	p->addModel(modelRCD);
	p->addModel(modelQPLFO);
	p->addModel(modelQCD);
	p->addModel(modelPI);
	p->addModel(modelPEG);
	p->addModel(modelLPG);
	p->addModel(modelL4);
	p->addModel(modelDLD);
	p->addModel(modelCLKM);
	p->addModel(modelCLKD);
	p->addModel(modelVerb);
	p->addModel(modelSwitch41);
	p->addModel(modelSwitch14);
	p->addModel(modelSeq8);
	p->addModel(modelProb8);
	p->addModel(modelOctave);
	p->addModel(modelMNMX);
	p->addModel(modelAtvert2);
	p->addModel(modelBPF);
	p->addModel(modelComplexEG);
	p->addModel(modelDetune);
	p->addModel(modelDjembe);
	p->addModel(modelDrum);
	p->addModel(modelEnOsc);
	p->addModel(modelFM);
	p->addModel(modelFollow);
	p->addModel(modelGate);
	p->addModel(modelHPF);
	p->addModel(modelHubMedium);
	p->addModel(modelKPLS);
	p->addModel(modelMultiLFO);
	p->addModel(modelNoise);
	p->addModel(modelPan);
	p->addModel(modelPitchShift);
	p->addModel(modelSlew);
	p->addModel(modelSource);
	p->addModel(modelStMix);
	p->addModel(modelMPEG);
}
