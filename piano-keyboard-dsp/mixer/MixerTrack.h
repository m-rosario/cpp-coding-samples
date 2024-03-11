//
// Created by Miguel on 8/4/2018.
//

#ifndef KEYBOARD_MIXERTRACK_H
#define KEYBOARD_MIXERTRACK_H

#include <cstdint>
#include <atomic>
#include "RenderableAudio.h"
#include "../effects/AudioEffect.h"
#include "AudioFile.h"
#include <cmath>

class MixerTrack: public RenderableAudio{
public:
    MixerTrack(AudioFile *audioFile, int32_t sampleRate, int8_t numChannels){
        mAudioFile = audioFile;
        mSampleRate = sampleRate;
        mChannelCount = numChannels;
        mTotalFrames = mAudioFile->getTotalFrames();

        for(int slotIndex = 0; slotIndex < FX_SLOTS; slotIndex++){
            mAudioEffects[slotIndex] = nullptr;
        }
        pFXInputFrameBuffer = new float[mChannelCount];
        pFXOutputFrameBuffer = new float[mChannelCount];

        for(int fadingStreamIndex = 0; fadingStreamIndex < MAX_FADING_STREAMS; fadingStreamIndex++){
            mIsStreamFadingOut[fadingStreamIndex] = false;
        }
    }

    ~MixerTrack(){
        delete [] pFXInputFrameBuffer;
        delete [] pFXOutputFrameBuffer;
    }

    void renderAudio(float *audioData, int64_t numFrames);

    int addFX(AudioEffect *);
    void setUsingFXChain(bool isUsingFXChain) { mIsUsingFXChain = isUsingFXChain; };
    void applyFXChain();
    void setFXEnabled(int fxSlot, bool isEnabled){mIsFXSlotEnabled[fxSlot] = isEnabled;}

    void resetPlayHead() { mReadFrameIndex = 0; };
    void setPlaying(bool isPlaying);
    void setLooping(bool isLooping) { mIsLooping = isLooping; };

    void startFadeOut(int milliseconds, float fadeOutFloorDB);

    bool isPlayingOrFading();

    void stopPlayingAndFading();
    void setPlayID(uint_fast64_t playID){
        mPlayID = playID;
    }

    uint_fast64_t getPlayID(){
        return mPlayID;
    }
private:
    static constexpr int8_t FX_SLOTS = 8;
    static constexpr int8_t MAX_FADING_STREAMS = 2;

    void applyFadeOut(float *mixingBuffer, int64_t numFrames);

    AudioFile *mAudioFile;
    uint_fast64_t mPlayID = -1;
    int64_t mReadFrameIndex = 0;
    int64_t mTotalFrames;
    std::atomic<bool> mIsPlaying { false };
    std::atomic<bool> mIsLooping { false };

    std::atomic<bool> mIsUsingFXChain { false };
    std::atomic<bool> mIsFXSlotEnabled[FX_SLOTS];
    AudioEffect *mAudioEffects[FX_SLOTS];
    int8_t mCurrentFXSlot = 0;
    float *pFXInputFrameBuffer;
    float *pFXOutputFrameBuffer;

    int32_t mSampleRate;
    int8_t mChannelCount;

    int8_t mCurrentFadingStream = -1; //Circular array index
    bool mIsFadeOutRunning = false;
    bool mIsStreamFadingOut[MAX_FADING_STREAMS];
    float mFadeOutScalar[MAX_FADING_STREAMS];
    int64_t mFadeOutStartingFrameIndex[MAX_FADING_STREAMS];
    int64_t mFadeOutRelativeReadFrameIndex[MAX_FADING_STREAMS];
    int64_t mFadeOutFinalFrameIndex[MAX_FADING_STREAMS];
};

#endif //KEYBOARD_MIXERTRACK_H
