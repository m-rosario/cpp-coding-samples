//
// Created by Miguel on 7/29/2018.
//

#ifndef KEYBOARD_AUDIOEFFECT_H
#define KEYBOARD_AUDIOEFFECT_H

#include <cstdint>

class AudioEffect{
public:
    virtual bool processAudioFrame(float * pInputBuffer, float * pOutputBuffer, int8_t numChannels) {
        //Default implementation, simply pass audio through
        for(int channelIndex = 0; channelIndex < numChannels; channelIndex++){
            pOutputBuffer[channelIndex] = pInputBuffer[channelIndex];
        }
        return true;
    };
};

#endif //KEYBOARD_AUDIOEFFECT_H
