//
// Created by Miguel on 7/21/2018.
//

#include "NativeSoundManager.h"
#include "effects/AudioFilters.h"


NativeSoundManager::NativeSoundManager(AAssetManager *assetManager, int sampleRate, int framesPerBurst){
    mAssetManager = assetManager;
    mNativeSampleRate = sampleRate;
    mNyquist = mNativeSampleRate / 2;
    //Reduce stream latency by setting the buffer size to a multiple of the burst size
    mNativeFramesPerBurst = framesPerBurst;
    mBufferSizeInFrames = framesPerBurst * BUFFER_BURST_MULTIPLIER;
    initialize();
}

void NativeSoundManager::initialize(){

    openAudioStream();

    if(mAudioStreamOpened){
        //Sets the actual buffer size to be the actual buffer capacity allowed. Might be lower than the requested value during stream building.
        mAudioStream->setBufferSizeInFrames(mAudioStream->getBufferCapacityInFrames());
    }

    //Create the mixer
    mAudioMixer = new AudioMixer(mNativeSampleRate, CHANNELS, mBufferSizeInFrames);

    //Add the tracks to the mixer
    for(int noteIndex = 0; noteIndex < PIANO_NOTES; noteIndex++){
        const char* filename;
        if(mNativeSampleRate == 44100){
            filename = mFilenames44100[noteIndex].c_str();
        }
        else{
            //Default sample rate
            filename = mFilenames48000[noteIndex].c_str();
        }

        AudioFile* audioFile = AudioFile::loadFromAssets(mAssetManager, filename, mNativeSampleRate, CHANNELS);
        mAudioMixer->addTrack(audioFile);

        mAudioMixer->getMixerTrack(noteIndex)->setUsingFXChain(true);
        mFaders[noteIndex] = new Fader();
        mAudioMixer->getMixerTrack(noteIndex)->setFXEnabled(
                mAudioMixer->getMixerTrack(noteIndex)->addFX(mFaders[noteIndex]),
                true);

        mLPFs[noteIndex] = new FirstOrderLPF(mNyquist,mNativeSampleRate,CHANNELS);
        mAudioMixer->getMixerTrack(noteIndex)->setFXEnabled(
                mAudioMixer->getMixerTrack(noteIndex)->addFX(mLPFs[noteIndex]),
                true);

    }
    startAudioStream();
}

void NativeSoundManager::destroy() {
    /** Destruction is skipped, because the C++ library is static. If you try to clean up and create
     * new instances you may end up with multiple instances and pointers of the same objects.
     *
     * Just let the system clean up memory by itself when the app's JVM is unloaded as a whole.
     * Re-creation after that is simply treated as a fist time initialization, so destruction is unnecessary.
     *
     * Only one instance, and only one audio stream. Simple. If system kills app, it gets killed as a whole. **/
    closeAudioStream();
    delete mAudioMixer;
    for (int i = 0; i < PIANO_NOTES; i++) {
        delete mFaders[i];
        delete mLPFs[i];
    }

}

void NativeSoundManager::openAudioStream() {
    if(!mAudioStreamOpened){
        //Build the stream
        AudioStreamBuilder builder;
        builder.setFormat(AudioFormat::Float);
        builder.setChannelCount(CHANNELS);
        builder.setSampleRate(mNativeSampleRate);
        if(mNativeFramesPerBurst > 0){
            builder.setDefaultFramesPerBurst(mNativeFramesPerBurst);
            builder.setFramesPerCallback(mNativeFramesPerBurst);
            builder.setBufferCapacityInFrames(mBufferSizeInFrames);
        }
        builder.setCallback(this);

        builder.setPerformanceMode(PerformanceMode::LowLatency);
        builder.setSharingMode(SharingMode::Exclusive);

        //Open the stream
        Result result = builder .openStream(&mAudioStream);
        if(result == Result::OK){
            mAudioStreamOpened = true;
        }
    }
}

void NativeSoundManager::startAudioStream() {
    if(mAudioStreamOpened && !mAudioStreamStarted){
        Result result = mAudioStream->requestStart();
        if(result == Result::OK){
            mAudioStreamStarted = true;
        }
    }
}

void NativeSoundManager::pauseAudioStream() {
    if(mAudioStreamOpened && mAudioStreamStarted){
        Result result = mAudioStream->requestPause();
        if(result == Result::OK){
            mAudioStreamStarted = false;
        }
    }
}

void NativeSoundManager::closeAudioStream(){
    /*
     * This is not used for the same reason we avoid destruction: it is unnecessary for a static object!
     * A closed stream cannot be re-opened anyways, so we are forced to re-create it from scratch if we close it.
     */
    Result result = mAudioStream->close();
    if(result == Result::OK){
        mAudioStreamStarted = false;
        mAudioStreamOpened = false;
    }
}

void NativeSoundManager::playNote(int noteIndex, int velocity){
    //Volume-Velocity mapping function
    mFaders[noteIndex]->setFaderLinear(velocity/127.0f);
    //Cutoff modeling function to avoid unrealistic linear drop. The function is clamped between 800Hz and Nyquist
    float cutoffFrequency = ((float) mNyquist - 800.0f)*powf(velocity / 127.0f,5) + 800.0f;
    mLPFs[noteIndex]->setCutoffFrequency(cutoffFrequency);
    mAudioMixer->getMixerTrack(noteIndex)->setPlaying(true);
    //The play ID is used to identify the track with the most recent play, as to prioritize during polyphony limiting
    mAudioMixer->getMixerTrack(noteIndex)->setPlayID(mAudioMixer->incrementPlayCount());
}

void NativeSoundManager::endNote(int noteIndex, int velocity){
    //the Release Velocity Fade-out time is clamped between 300 ms and MAX_FADE_OUT_TIME_MS
    int releaseVelocityFadeOutTime = int((MAX_FADE_OUT_TIME_MS-300)*(1.0f-velocity/127.0f)+300);
    mAudioMixer->getMixerTrack(noteIndex)->startFadeOut(releaseVelocityFadeOutTime, FADE_OUT_FLOOR_DB);
}

DataCallbackResult NativeSoundManager::onAudioReady(AudioStream* oboeStream, void* audioData, int32_t numFrames){
    //The given number of audio frames are rendered and stored in the audioData pointer, which the system then plays out
    mAudioMixer->renderAudio(static_cast<float *>(audioData), numFrames);
    return DataCallbackResult::Continue;
}
