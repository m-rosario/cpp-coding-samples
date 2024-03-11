//
// Created by Miguel on 7/27/2018.
//

#ifndef KEYBOARD_RENDERABLEFLOATAUDIO_H
#define KEYBOARD_RENDERABLEFLOATAUDIO_H

#include <cstdint>

class RenderableAudio {

public:
    virtual ~RenderableAudio() = default;
    virtual void renderAudio(float *audioData, int64_t numFrames) = 0;
};

#endif //KEYBOARD_RENDERABLEFLOATAUDIO_H
