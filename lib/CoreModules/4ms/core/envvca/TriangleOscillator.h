#pragma once

#include <cstdio>

class TriangleOscillator
{
public:
	// FIXME: FOLLOW state is orthagonal to [RISE or FALL] state
    enum State_t {TRIGGERED, FOLLOW};
    enum SlopeState_t {IDLE, RISING, FALLING};

public:
    TriangleOscillator() : outputInV(0.0f), state(State_t::FOLLOW), slopeState(SlopeState_t::RISING), cycling(false), retriggerPending(false), sustainMax(false), targetVoltage(0.0f)
    {
    }

    void setRiseTimeInS(float val)
    {
        slopeRising = (MaxValInV - MinValInV) / val;
    }

    void setFallTimeInS(float val)
    {
        slopeFalling = (MinValInV - MaxValInV) / val;
    }

    void doRetrigger()
    {
        retriggerPending = true;
    }

    void setTargetVoltage(float val)
    {
        if (val > FollowTresholdInV)
        {
            targetVoltage = val;
        }
        else
        {
            targetVoltage = MinValInV;
        }
    }

    void proceed(float timeInS)
    {
		if (retriggerPending)
		{
			slopeState = SlopeState_t::RISING;
            state = State_t::TRIGGERED;
			retriggerPending = false;
		}

        if (state != State_t::TRIGGERED)
        {
            if (outputInV < targetVoltage)
            {
                slopeState = SlopeState_t::RISING;

                outputInV += slopeRising * timeInS;
                outputInV = std::min(targetVoltage, outputInV);
            }
            else if (outputInV > targetVoltage)
            {
                slopeState = SlopeState_t::FALLING;

                outputInV += slopeFalling * timeInS;
                outputInV = std::max(targetVoltage, outputInV);
            }
            else
            {
                slopeState = SlopeState_t::IDLE;

                if(cycling && targetVoltage == MinValInV)
                {
                    state = State_t::TRIGGERED;
                }
            }
        }
        else
        {
            if (slopeState == SlopeState_t::RISING)
            {
                outputInV += slopeRising * timeInS;

                if (outputInV > MaxValInV)
                {
                    outputInV = MaxValInV - (outputInV - MaxValInV);
                    if(!sustainMax) {
                        slopeState = SlopeState_t::FALLING;
                    }
                }
            }
            else
            {
                outputInV += slopeFalling * timeInS;

                auto lowerLimitInV = MinValInV;

                if (targetVoltage != MinValInV)
                {
                    lowerLimitInV = targetVoltage;
                }

                if (outputInV < lowerLimitInV)
                {
                    outputInV = lowerLimitInV + (lowerLimitInV - outputInV);
                    slopeState = SlopeState_t::RISING;
                    if(!cycling || targetVoltage != MinValInV)
                    {
                        state = State_t::FOLLOW;
                    }                
                }
            }
        }
    }

    void holdMax(bool val) 
    {
        sustainMax = val;
    }

    SlopeState_t getSlopeState() const
    {
        return slopeState;
    }

    void setCycling(bool val)
    {
        cycling = val;
    }

    float getOutput() const
    {
        return outputInV;
    }

private:
    float outputInV;
    State_t state;
    SlopeState_t slopeState;
    bool cycling;
    bool retriggerPending;
    bool sustainMax;
    float targetVoltage;

    float slopeFalling = 0.5f;
    float slopeRising = 0.5f;

    static constexpr float MaxValInV = 5.0f;
    static constexpr float MinValInV = 0.0f;
    static constexpr float FollowTresholdInV = 0.1f;
};
