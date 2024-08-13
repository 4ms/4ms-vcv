#pragma once

#include "Tables.h"

class Channel
{
public:
    Channel()
        : potValue(0.f), inputValue(0.f), outputValue(0.f), controlValue(0.f), muteValue(0.f) {

    }

    void input(float input) {
        inputValue = input;
    }

    void pot(float pot) {
        potValue = pot;
    }

    void control(float control) {
        controlValue = control;
    }

    void mute(bool isMuted) {
        if (isMuted == true) {
            muteValue = 0.f;
        } else {
            muteValue = 1.f;
        }
    }

    float output(void) {
        outputValue = inputValue * VoltageToGainTable.lookup(potValue * controlValue * muteValue);

        return outputValue;
    }

    float getLEDbrightness(void) {
        return VoltageToGainTable.lookup(potValue * controlValue * muteValue);
    }

private:
float potValue;
float inputValue;
float outputValue;
float controlValue;
float muteValue;
};
