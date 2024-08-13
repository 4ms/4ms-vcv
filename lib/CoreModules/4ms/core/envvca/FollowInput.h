#pragma once
#include <cmath>
#include <cstdint>

class FollowInput
{
private:
	static constexpr float inputHysteresisInV = 0.1f;

public:
    float process(float input) {
        // the follow input is rather sensitive to noise. in order to prevent oscillations around a noisy follow voltage some hysteresis is added to the input
        auto hystOutput = applyHysteresis(input);

        /*
        the hysteresis transforms a slow continious modulation into a stepped voltage. this can cause frequent state transitions of the internal oscillator from rising/falling to idle. 
        as this results in erratic behavior of the eor/eof jacks the stepped voltage has to be filtered in order to create a continious modulation.
        this is done by an adjustable low pass filter. its filter coefficient is calculated based on the gradient of the incoming follow voltage,
        which is identified by the number of processing steps which were necessary to cross the hysteresis threshold.
        */
        if(hystOutput.differenceToLastOutput >= inputHysteresisInV) {

            const auto gradient = hystOutput.differenceToLastOutput / hystOutput.timeToLastOutputOverThreshold;

            if(gradient >= inputHysteresisInV || hystOutput.differenceToLastOutput >= 0.5f) {
                filterCoeff = 1.0f;
            }
            else
            {
                filterCoeff = gradient / 0.05f;
            }
        }			

        float filterOutput = filter(hystOutput.output, filterCoeff);

        return filterOutput;
    }

private:
    struct hysteresisOutput_t {
        float output;
        float differenceToLastOutput;
        uint32_t timeToLastOutputOverThreshold;
    };
    
private:
    hysteresisOutput_t applyHysteresis(float input) {
        static float previousInput = 0.0f;
        static uint32_t count = 0;

        if (count < std::numeric_limits<decltype(count)>::max()) {
            count++;
        }

        hysteresisOutput_t output;

        output.differenceToLastOutput = std::fabs(input - previousInput);

        if (output.differenceToLastOutput >= inputHysteresisInV)
        {
            previousInput = input;
            output.timeToLastOutputOverThreshold = count;
            count = 0;
        }

        output.output = previousInput;

        return output;
    }

    float filter(float input, float filterCoeff) {
        static float previousFollowInputFilterOutput = 0.0f;

        float filterOutput = filterCoeff * input + (1.0f - filterCoeff) * previousFollowInputFilterOutput;

        previousFollowInputFilterOutput = filterOutput;

        return filterOutput;
    }

private:
    float filterCoeff = 0.0f;
};
