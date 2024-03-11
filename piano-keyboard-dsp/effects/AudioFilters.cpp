//
// Created by Miguel on 7/30/2018.
//

#include "AudioFilters.h"

//Fader
bool Fader::processAudioFrame(float *pInputBuffer, float *pOutputBuffer, int8_t numChannels) {
    for(int channelIndex = 0; channelIndex < numChannels; channelIndex++){
        pOutputBuffer[channelIndex] = pInputBuffer[channelIndex]*mFaderLinear;
    }
    return true;
}

//FirstOrderLPF
bool FirstOrderLPF::processAudioFrame(float *pInputBuffer, float *pOutputBuffer, int8_t numChannels) {
    mChannelCount = numChannels;
    //Apply filtering to each individual channel
    for(int channelIndex = 0; channelIndex < mChannelCount; channelIndex++){
        pOutputBuffer[channelIndex] = pInputBuffer[channelIndex]*a0 - z1[channelIndex]*b1;
        z1[channelIndex] = pOutputBuffer[channelIndex];
    }
    return true;
}
void FirstOrderLPF::calculateCoefficients(){
    theta = 2*M_PI*fc/fs;
    gamma = 2 - cosf(theta);
    b1 = sqrtf(gamma*gamma - 1) - gamma;
    a0 = 1 + b1;
}

void FirstOrderLPF::flushRegisters() {
    for(int channelIndex = 0; channelIndex < mChannelCount; channelIndex++){
        z1[channelIndex] = 0;
    }
}

void FirstOrderLPF::setCutoffFrequency(float cutoffFrequency) {
    fc = cutoffFrequency;
    calculateCoefficients();
}
