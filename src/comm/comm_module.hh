#pragma once
#include "CoreModules/CoreProcessor.hh"
#include "comm/comm_jack.hh"

// CommModule wraps CoreProcessors for use in VCV Rack

class CommModule : public rack::Module {
public:
	std::unique_ptr<CoreProcessor> core;
	std::vector<CommInputJack> inJacks;
	std::vector<CommOutputJack> outJacks;

private:
	bool sampleRateChanged = true;

	// Needed for converting legacy patch files (<2.0.15)
	std::string patch_version;

protected:
	CommModule() = default;
	~CommModule() = default;

	void configComm(unsigned NUM_PARAMS, unsigned NUM_INPUTS, unsigned NUM_OUTPUTS, unsigned NUM_LIGHTS);
	void process(const ProcessArgs &args) override;
	void onSampleRateChange() override;

	json_t *dataToJson() override;
	void dataFromJson(json_t *rootJ) override;

	void fromJson(json_t *rootJ) override;
	void paramsFromJson(json_t *rootJ) override;
};
