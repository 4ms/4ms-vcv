#pragma once
#include <rack.hpp>

// FIXME HACK
namespace MetaModule
{

extern std::string wifiUrl;
enum Volume { Internal = 0, USB = 1, Card = 2 };
extern Volume wifiVolume;

} // namespace MetaModule

extern rack::Plugin *pluginInstance;

//////////////// Auto generated ///////////////////////
// Add models below here
extern rack::Model *modelOctave;
extern rack::Model *modelNoise;
extern rack::Model *modelSHEV;
extern rack::Model *modelENVVCA;
extern rack::Model *modelDEV;
extern rack::Model *modelFreeverb;
extern rack::Model *modelVCAM;
extern rack::Model *modelTapo;
extern rack::Model *modelSISM;
extern rack::Model *modelSH;
extern rack::Model *modelSCM;
extern rack::Model *modelRCD;
extern rack::Model *modelQPLFO;
extern rack::Model *modelQCD;
extern rack::Model *modelPI;
extern rack::Model *modelPEG;
extern rack::Model *modelLPG;
extern rack::Model *modelL4;
extern rack::Model *modelDLD;
extern rack::Model *modelCLKM;
extern rack::Model *modelCLKD;
extern rack::Model *modelVerb;
extern rack::Model *modelSwitch41;
extern rack::Model *modelSwitch14;
extern rack::Model *modelSeq8;
extern rack::Model *modelProb8;
extern rack::Model *modelMNMX;
extern rack::Model *modelAtvert2;
extern rack::Model *modelBPF;
extern rack::Model *modelComplexEG;
extern rack::Model *modelDetune;
extern rack::Model *modelDjembe;
extern rack::Model *modelDrum;
extern rack::Model *modelEnOsc;
extern rack::Model *modelFM;
extern rack::Model *modelFollow;
extern rack::Model *modelGate;
extern rack::Model *modelHPF;
extern rack::Model *modelHubMedium;
extern rack::Model *modelKPLS;
extern rack::Model *modelMultiLFO;
extern rack::Model *modelPan;
extern rack::Model *modelPitchShift;
extern rack::Model *modelSlew;
extern rack::Model *modelSource;
extern rack::Model *modelStMix;
extern rack::Model *modelMPEG;
extern rack::Model *modelMMAudioExpander;
