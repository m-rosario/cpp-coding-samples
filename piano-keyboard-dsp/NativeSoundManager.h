
#ifndef KEYBOARD_NATIVESOUNDMANAGER_H
#define KEYBOARD_NATIVESOUNDMANAGER_H

#include <oboe/Oboe.h>
#include <jni.h>
#include <cmath>
#include <android/asset_manager_jni.h>
#include "mixer/AudioMixer.h"
#include <cstring>
#include <jni.h>
#include "mixer/AudioFile.h"
#include "effects/AudioFilters.h"


using namespace oboe;

static constexpr int CHANNELS = 1;
static constexpr int PIANO_NOTES = 88;
static constexpr int MAX_FADE_OUT_TIME_MS = 4000;
static constexpr float FADE_OUT_FLOOR_DB = -24.0f;
static constexpr int BUFFER_BURST_MULTIPLIER = 3;

class NativeSoundManager : public AudioStreamCallback {
public:
    explicit NativeSoundManager(AAssetManager *assetManager, int sampleRate, int framesPerBurst);
    void initialize();
    void destroy();
    void playNote(int noteIndex, int velocity);
    void endNote(int noteIndex, int velocity);
    void openAudioStream();
    void startAudioStream();
    void pauseAudioStream();
    void closeAudioStream();
    // Inherited from oboe::AudioStreamCallback
    DataCallbackResult onAudioReady(AudioStream* oboeStream, void* audioData, int32_t numFrames) override;
private:
    AAssetManager *mAssetManager{nullptr};
    int mNativeSampleRate = 48000; //Default
    int mNyquist = 24000; //Default
    AudioMixer *mAudioMixer;
    AudioStream *mAudioStream{nullptr};
    int mNativeFramesPerBurst;
    int mBufferSizeInFrames;

    bool mAudioStreamOpened {false};
    bool mAudioStreamStarted {false};

    Fader *mFaders[PIANO_NOTES];
    FirstOrderLPF *mLPFs[PIANO_NOTES];

    std::string mFilenames44100[PIANO_NOTES] {
            "samples44k/y44100_00.raw",
            "samples44k/y44100_01.raw",
            "samples44k/y44100_02.raw",
            "samples44k/y44100_03.raw",
            "samples44k/y44100_04.raw",
            "samples44k/y44100_05.raw",
            "samples44k/y44100_06.raw",
            "samples44k/y44100_07.raw",
            "samples44k/y44100_08.raw",
            "samples44k/y44100_09.raw",

            "samples44k/y44100_10.raw",
            "samples44k/y44100_11.raw",
            "samples44k/y44100_12.raw",
            "samples44k/y44100_13.raw",
            "samples44k/y44100_14.raw",
            "samples44k/y44100_15.raw",
            "samples44k/y44100_16.raw",
            "samples44k/y44100_17.raw",
            "samples44k/y44100_18.raw",
            "samples44k/y44100_19.raw",

            "samples44k/y44100_20.raw",
            "samples44k/y44100_21.raw",
            "samples44k/y44100_22.raw",
            "samples44k/y44100_23.raw",
            "samples44k/y44100_24.raw",
            "samples44k/y44100_25.raw",
            "samples44k/y44100_26.raw",
            "samples44k/y44100_27.raw",
            "samples44k/y44100_28.raw",
            "samples44k/y44100_29.raw",

            "samples44k/y44100_30.raw",
            "samples44k/y44100_31.raw",
            "samples44k/y44100_32.raw",
            "samples44k/y44100_33.raw",
            "samples44k/y44100_34.raw",
            "samples44k/y44100_35.raw",
            "samples44k/y44100_36.raw",
            "samples44k/y44100_37.raw",
            "samples44k/y44100_38.raw",
            "samples44k/y44100_39.raw",

            "samples44k/y44100_40.raw",
            "samples44k/y44100_41.raw",
            "samples44k/y44100_42.raw",
            "samples44k/y44100_43.raw",
            "samples44k/y44100_44.raw",
            "samples44k/y44100_45.raw",
            "samples44k/y44100_46.raw",
            "samples44k/y44100_47.raw",
            "samples44k/y44100_48.raw",
            "samples44k/y44100_49.raw",

            "samples44k/y44100_50.raw",
            "samples44k/y44100_51.raw",
            "samples44k/y44100_52.raw",
            "samples44k/y44100_53.raw",
            "samples44k/y44100_54.raw",
            "samples44k/y44100_55.raw",
            "samples44k/y44100_56.raw",
            "samples44k/y44100_57.raw",
            "samples44k/y44100_58.raw",
            "samples44k/y44100_59.raw",

            "samples44k/y44100_60.raw",
            "samples44k/y44100_61.raw",
            "samples44k/y44100_62.raw",
            "samples44k/y44100_63.raw",
            "samples44k/y44100_64.raw",
            "samples44k/y44100_65.raw",
            "samples44k/y44100_66.raw",
            "samples44k/y44100_67.raw",
            "samples44k/y44100_68.raw",
            "samples44k/y44100_69.raw",

            "samples44k/y44100_70.raw",
            "samples44k/y44100_71.raw",
            "samples44k/y44100_72.raw",
            "samples44k/y44100_73.raw",
            "samples44k/y44100_74.raw",
            "samples44k/y44100_75.raw",
            "samples44k/y44100_76.raw",
            "samples44k/y44100_77.raw",
            "samples44k/y44100_78.raw",
            "samples44k/y44100_79.raw",

            "samples44k/y44100_80.raw",
            "samples44k/y44100_81.raw",
            "samples44k/y44100_82.raw",
            "samples44k/y44100_83.raw",
            "samples44k/y44100_84.raw",
            "samples44k/y44100_85.raw",
            "samples44k/y44100_86.raw",
            "samples44k/y44100_87.raw",
    };

    std::string mFilenames48000[PIANO_NOTES] {
            "samples48k/y48000_00.raw",
            "samples48k/y48000_01.raw",
            "samples48k/y48000_02.raw",
            "samples48k/y48000_03.raw",
            "samples48k/y48000_04.raw",
            "samples48k/y48000_05.raw",
            "samples48k/y48000_06.raw",
            "samples48k/y48000_07.raw",
            "samples48k/y48000_08.raw",
            "samples48k/y48000_09.raw",

            "samples48k/y48000_10.raw",
            "samples48k/y48000_11.raw",
            "samples48k/y48000_12.raw",
            "samples48k/y48000_13.raw",
            "samples48k/y48000_14.raw",
            "samples48k/y48000_15.raw",
            "samples48k/y48000_16.raw",
            "samples48k/y48000_17.raw",
            "samples48k/y48000_18.raw",
            "samples48k/y48000_19.raw",

            "samples48k/y48000_20.raw",
            "samples48k/y48000_21.raw",
            "samples48k/y48000_22.raw",
            "samples48k/y48000_23.raw",
            "samples48k/y48000_24.raw",
            "samples48k/y48000_25.raw",
            "samples48k/y48000_26.raw",
            "samples48k/y48000_27.raw",
            "samples48k/y48000_28.raw",
            "samples48k/y48000_29.raw",

            "samples48k/y48000_30.raw",
            "samples48k/y48000_31.raw",
            "samples48k/y48000_32.raw",
            "samples48k/y48000_33.raw",
            "samples48k/y48000_34.raw",
            "samples48k/y48000_35.raw",
            "samples48k/y48000_36.raw",
            "samples48k/y48000_37.raw",
            "samples48k/y48000_38.raw",
            "samples48k/y48000_39.raw",

            "samples48k/y48000_40.raw",
            "samples48k/y48000_41.raw",
            "samples48k/y48000_42.raw",
            "samples48k/y48000_43.raw",
            "samples48k/y48000_44.raw",
            "samples48k/y48000_45.raw",
            "samples48k/y48000_46.raw",
            "samples48k/y48000_47.raw",
            "samples48k/y48000_48.raw",
            "samples48k/y48000_49.raw",

            "samples48k/y48000_50.raw",
            "samples48k/y48000_51.raw",
            "samples48k/y48000_52.raw",
            "samples48k/y48000_53.raw",
            "samples48k/y48000_54.raw",
            "samples48k/y48000_55.raw",
            "samples48k/y48000_56.raw",
            "samples48k/y48000_57.raw",
            "samples48k/y48000_58.raw",
            "samples48k/y48000_59.raw",

            "samples48k/y48000_60.raw",
            "samples48k/y48000_61.raw",
            "samples48k/y48000_62.raw",
            "samples48k/y48000_63.raw",
            "samples48k/y48000_64.raw",
            "samples48k/y48000_65.raw",
            "samples48k/y48000_66.raw",
            "samples48k/y48000_67.raw",
            "samples48k/y48000_68.raw",
            "samples48k/y48000_69.raw",

            "samples48k/y48000_70.raw",
            "samples48k/y48000_71.raw",
            "samples48k/y48000_72.raw",
            "samples48k/y48000_73.raw",
            "samples48k/y48000_74.raw",
            "samples48k/y48000_75.raw",
            "samples48k/y48000_76.raw",
            "samples48k/y48000_77.raw",
            "samples48k/y48000_78.raw",
            "samples48k/y48000_79.raw",

            "samples48k/y48000_80.raw",
            "samples48k/y48000_81.raw",
            "samples48k/y48000_82.raw",
            "samples48k/y48000_83.raw",
            "samples48k/y48000_84.raw",
            "samples48k/y48000_85.raw",
            "samples48k/y48000_86.raw",
            "samples48k/y48000_87.raw",
    };
};


#endif //KEYBOARD_NATIVESOUNDMANAGER_H
