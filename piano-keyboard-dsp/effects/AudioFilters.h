//
// Created by Miguel on 7/30/2018.
//

#ifndef KEYBOARD_AUDIOFILTERS_H
#define KEYBOARD_AUDIOFILTERS_H

#include "AudioEffect.h"
#include <cmath>
#include <cstdint>

class FirstOrderLPF: public AudioEffect{
public:
    FirstOrderLPF(float cutoffFrequency, float sampleRate, int8_t numChannels){
        fc = cutoffFrequency;
        fs = sampleRate;
        //One register per channel
        z1 = new float[numChannels];
        mChannelCount = numChannels;
        flushRegisters();
        calculateCoefficients();
    }

    ~FirstOrderLPF(){
        delete [] z1;
    }

    bool processAudioFrame(float *pInputBuffer, float *pOutputBuffer, int8_t numChannels) override;
    void calculateCoefficients();
    void flushRegisters();
    void setCutoffFrequency(float cutoffFrequency);
private:
    int8_t mChannelCount;

    float *z1;
    float fc;
    float fs;
    float theta;
    float gamma;
    float b1;
    float a0;
};

class Fader: public AudioEffect{
public:
    Fader(){
        mFaderLinear = 1.0f;
        mFaderLog = 0.0f;
    }
    void setFaderLinear(float linearValue){
        mFaderLinear = linearValue;
        mFaderLog = 20.0f * logf(linearValue);
    }
    void setFaderLog(float logValue){
        mFaderLog = logValue;
        mFaderLinear = powf(10.0f, logValue/20.0f);
    }

    bool processAudioFrame(float *pInputBuffer, float *pOutputBuffer, int8_t numChannels) override;

private:
    float mFaderLinear;
    float mFaderLog;
};

#endif //KEYBOARD_AUDIOFILTERS_H
