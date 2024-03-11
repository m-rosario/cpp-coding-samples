//
// Based on oboe/samples/rhythmgame/Mixer
//

#ifndef KEYBOARD_AUDIOMIXER_H
#define KEYBOARD_AUDIOMIXER_H

#include <cstdint>
#include <atomic>
#include "RenderableAudio.h"
#include "../effects/AudioEffect.h"
#include "MixerTrack.h"
#include <math.h>

static constexpr int8_t MIXER_TRACKS = 127;


class AudioMixer : public RenderableAudio {

public:
    AudioMixer(int32_t sampleRate, int8_t numChannels, int32_t bufferSizeInFrames){
        mSampleRate = sampleRate;
        mChannelCount = numChannels;
        mMixingBuffer = new float[bufferSizeInFrames * numChannels]; // TODO: smart pointer
        for(int i = 0; i < MIXER_TRACKS; i++){
            mMixerTracks[i] = nullptr;
        }
    }


    ~AudioMixer(){
        delete [] mMixingBuffer;
        for(int i = 0; i < mNextFreeTrackIndex; i++){
            delete mMixerTracks[i];
        }
    }


    int8_t addTrack(AudioFile *audioFile);
    void renderAudio(float *audioData, int64_t numFrames);
    MixerTrack * getMixerTrack(int8_t trackIndex){
        return mMixerTracks[trackIndex];
    }

    uint_fast64_t incrementPlayCount(){
        return ++mCurrentPlayID;
    }
private:
    static constexpr int8_t MAX_POLYPHONY = 10;
    static constexpr float COMPRESSOR_THRESHOLD = 0.707f;
    static constexpr float COMPRESSOR_RATIO = 1.0f/8.0f;
    static constexpr float LIMITER_THRESHOLD = 0.90f;
    static constexpr float LIMITER_RATIO = 1.0f/12.0f;
    float *mMixingBuffer;
    MixerTrack *mMixerTracks[MIXER_TRACKS]; // TODO: this might be better as a linked list for easy track removal
    int8_t mNextFreeTrackIndex = 0;

    int32_t mSampleRate;
    int8_t mChannelCount;

    uint_fast64_t mCurrentPlayID = MAX_POLYPHONY; //Initialized like this so that their difference is never negative (using uint...)
};

#endif //KEYBOARD_AUDIOMIXER_H
