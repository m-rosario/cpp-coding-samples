//
// Created by Miguel on 8/4/2018.
//

#include "MixerTrack.h"

void MixerTrack::renderAudio(float *mixingBuffer, int64_t numFrames) {

    if (mIsPlaying){
        // First check whether we're about to reach the end of the recording
        if (mReadFrameIndex + numFrames >= mTotalFrames){
            //The buffer frames exceed the number of frames needed to play the rest of the file
            //Shrink the buffer to only the amount needed
            numFrames = mTotalFrames - mReadFrameIndex;
        }

        //Copy the audio data into the mixing buffer
        for (int frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
            for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++) {
                //Copy audio data into the buffer. It has to be a copy because the internal data should not change
                mixingBuffer[(frameIndex*mChannelCount)+channelIndex] = mAudioFile->mAudioData[(mReadFrameIndex*mChannelCount)+channelIndex];
            }
            // Increment and handle wraparound
            if (++mReadFrameIndex >= mTotalFrames){
                //Reached the last frame, wrap back to zero
                mReadFrameIndex = 0;
                if(!mIsLooping){
                    mIsPlaying = false;
                    break;
                }
            }
        }
    } else {
        // Not playing file, fill with zeros to output silence
        for (int frameIndex = 0; frameIndex < numFrames * mChannelCount; ++frameIndex) {
            mixingBuffer[frameIndex] = 0;
        }
    }

    if(mIsFadeOutRunning && !mIsLooping){
        //Apply fade outs to the mixing buffer and mix with current stream if necessary
        applyFadeOut(mixingBuffer, numFrames);
    }

    //Post-processing
    if(mIsUsingFXChain && (mIsPlaying || mIsFadeOutRunning)){
        //Apply FX to the final mixing buffer
        for (int frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
            //We create temporary buffers to store the current frame data, including all its channels
            //It will be processed as a whole in the FX, which will decide what to do with each channel
            for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++) {
                pFXInputFrameBuffer[channelIndex] = mixingBuffer[(frameIndex*mChannelCount)+channelIndex];
            }
            //Now that the input buffer contains the data we process it into the output buffer
            applyFXChain();
            //And finally extract the frame data from the output buffer back into the mixing buffer
            for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++) {
                mixingBuffer[(frameIndex*mChannelCount)+channelIndex] =
                        pFXOutputFrameBuffer[channelIndex];
            }
        }
    }

}

void MixerTrack::setPlaying(bool isPlaying){
    mIsPlaying = isPlaying;
    resetPlayHead();
}

int MixerTrack::addFX(AudioEffect *audioEffect){
    if(mCurrentFXSlot >= FX_SLOTS){
        return -1;
    }

    mAudioEffects[mCurrentFXSlot] = audioEffect;

    return mCurrentFXSlot++;
}

void MixerTrack::applyFXChain() {
    //We first pass data into Output buffer directly, to still get output in case all slots are disabled
    for(int channelIndex = 0; channelIndex < mChannelCount; channelIndex++){
        pFXOutputFrameBuffer[channelIndex] = pFXInputFrameBuffer[channelIndex];
    }
    //Now we process each slot
    for(int fxSlotIndex = 0; fxSlotIndex < mCurrentFXSlot; fxSlotIndex++){
        if(mIsFXSlotEnabled[fxSlotIndex]){
            mAudioEffects[fxSlotIndex]->processAudioFrame(pFXInputFrameBuffer,pFXOutputFrameBuffer,mChannelCount);
            //The output of the current effect is passed as input for the next effect
            for(int channelIndex = 0; channelIndex < mChannelCount; channelIndex++){
                pFXInputFrameBuffer[channelIndex] = pFXOutputFrameBuffer[channelIndex];
            }
        }
    }
}



void MixerTrack::startFadeOut(int milliseconds, float fadeOutFloorDB) {
    if(!mIsPlaying){
        return; //The audio track reached the end and wrapped back to the initial frame. Don't fade out!
    }

    //New fading stream. Increase circular array index and wrap if necessary
    mCurrentFadingStream++;
    if(mCurrentFadingStream >= MAX_FADING_STREAMS){
         mCurrentFadingStream = 0;
    }

    uint_fast64_t totalFadeOutFrames = milliseconds * mSampleRate / 1000.0f;

    mFadeOutStartingFrameIndex[mCurrentFadingStream] = mReadFrameIndex;

    //Check if the fade out will go beyond the end of the audio, and adjust final frame as needed
    if(mFadeOutStartingFrameIndex[mCurrentFadingStream] + totalFadeOutFrames >= mTotalFrames){
        //The last frame fade out frame is the last frame of the audio
        mFadeOutFinalFrameIndex[mCurrentFadingStream] = mTotalFrames;
    }
    else{
        //Calculate final fade out frame
        mFadeOutFinalFrameIndex[mCurrentFadingStream] = mFadeOutStartingFrameIndex[mCurrentFadingStream] + totalFadeOutFrames;
    }

    mFadeOutRelativeReadFrameIndex[mCurrentFadingStream] = 0;
    mFadeOutScalar[mCurrentFadingStream] = powf(10.0f, (fadeOutFloorDB / totalFadeOutFrames) / 20.0f);

    mIsStreamFadingOut[mCurrentFadingStream] = true;
    mIsFadeOutRunning = true;

    //Set playing to false so that the audio rendering is stopped, and only the fade out continues to run
    setPlaying(false);
}

//Private
void MixerTrack::applyFadeOut(float *mixingBuffer, int64_t numFrames) {
    //Mixes the active stream with the fading streams

    bool foundFadingStream = false;
    int64_t streamNumFrames;
    for(int fadingStreamIndex = 0; fadingStreamIndex < MAX_FADING_STREAMS; fadingStreamIndex++){
        //If the stream is fading out then process it, else just continue with the next stream
        if(mIsStreamFadingOut[fadingStreamIndex]){
            foundFadingStream = true;
            // First check if the current stream will go beyond the end of the recording
            if (mFadeOutStartingFrameIndex[fadingStreamIndex]+mFadeOutRelativeReadFrameIndex[fadingStreamIndex] + numFrames
                >= mTotalFrames){
                //The buffer frames exceed the number of frames needed to play the rest of the file
                //Shrink the buffer to only the amount needed for the current stream
                streamNumFrames = mTotalFrames - mFadeOutStartingFrameIndex[fadingStreamIndex] - mFadeOutRelativeReadFrameIndex[fadingStreamIndex];
            }
            else{
                //Use the full buffer
                streamNumFrames = numFrames;
            }

            for (int frameIndex = 0; frameIndex < streamNumFrames; frameIndex++) {
                for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++) {
                    mixingBuffer[(frameIndex*mChannelCount)+channelIndex] +=
                            mAudioFile->mAudioData[((mFadeOutStartingFrameIndex[fadingStreamIndex]+mFadeOutRelativeReadFrameIndex[fadingStreamIndex])*mChannelCount)+channelIndex]
                            * powf(mFadeOutScalar[fadingStreamIndex], mFadeOutRelativeReadFrameIndex[fadingStreamIndex]);
                }
                //Processed all channels of the current frame. Increase index
                mFadeOutRelativeReadFrameIndex[fadingStreamIndex]++;

                if(mFadeOutStartingFrameIndex[fadingStreamIndex] + mFadeOutRelativeReadFrameIndex[fadingStreamIndex]
                   >= mFadeOutFinalFrameIndex[fadingStreamIndex]){
                    //This stream is done fading.
                    mIsStreamFadingOut[fadingStreamIndex] = false;
                    //Next stream
                }
            }
        }
    }
    if(!foundFadingStream){
        //Reached the end of the array without finding a fading stream.
        mIsFadeOutRunning = false;
    }
}

bool MixerTrack::isPlayingOrFading() {
    return mIsPlaying || mIsFadeOutRunning;
}

void MixerTrack::stopPlayingAndFading() {
    setPlaying(false);
    mIsFadeOutRunning = false;
}