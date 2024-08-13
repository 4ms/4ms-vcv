#pragma once

namespace MetaModule {

class PeakDetector{
public:
    PeakDetector()
        : peak(0.f), decayTimeInSeconds(0.2f), samplingRate(48000.f) {

    }

    float operator()(float input) {
        if (input > peak) {
            peak = input;
        } else {
            peak += (input - peak) / (decayTimeInSeconds * samplingRate);
        }

        return peak;
    }

    void setDecay(float decayInSeconds)
    {
        decayTimeInSeconds = decayInSeconds;
    }

    void setSamplerate(float sr) {
        samplingRate = sr;
    }

private:
    float peak;
    float decayTimeInSeconds;
    float samplingRate;

};




}