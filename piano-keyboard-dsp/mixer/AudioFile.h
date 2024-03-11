//
// Based on oboe/samples/rhythmgame/SoundRecording
//

#ifndef KEYBOARD_AUDIOFILE_H
#define KEYBOARD_AUDIOFILE_H

#include <cstdint>
#include <android/asset_manager.h>
#include <atomic>
#include "RenderableAudio.h"
#include "../effects/AudioEffect.h"

class AudioFile {
public:
    AudioFile(const float *sourceData, int64_t numFrames, int32_t sampleRate, int8_t numChannels)
            : mAudioData(sourceData)
            , mTotalFrames(numFrames)
            , mSampleRate(sampleRate)
            , mChannelCount(numChannels){
    };

    static AudioFile * loadFromAssets(AAssetManager *assetManager, const char * filename, int32_t sampleRate, int8_t trackChannels);
    int64_t getTotalFrames(){ return mTotalFrames; }
    const float *mAudioData = nullptr;
private:
    int32_t mSampleRate;
    int8_t mChannelCount;
    int64_t mTotalFrames;
};

#endif //KEYBOARD_AUDIOFILE_H
