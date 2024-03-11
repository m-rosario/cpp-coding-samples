//
// Created by Miguel on 7/27/2018.
//


#include "AudioMixer.h"

void AudioMixer::renderAudio(float *audioData, int64_t numFrames) {

    // Zero out the incoming container array. It will be filled with the rendered PCM data
    for (int frameIndex = 0; frameIndex < numFrames * mChannelCount; ++frameIndex) {
        audioData[frameIndex] = 0;
    }

    for (int trackIndex = 0; trackIndex < mNextFreeTrackIndex; ++trackIndex) {
        //We go through each track and test if there is sound playing, as to avoid processing silent tracks
        //If no track is active, then no processing will be done, and the mixer will output the zeroes we put in above.
        if(mMixerTracks[trackIndex]->isPlayingOrFading()){
            //The track is playing, so now we check against the polyphony settings to only process the most recent plays
            if(mMixerTracks[trackIndex]->getPlayID() > mCurrentPlayID - MAX_POLYPHONY){
                //We pass the temporary mixing buffer to the active track for processing
                mMixerTracks[trackIndex]->renderAudio(mMixingBuffer, numFrames);

                //Now that the mixing buffer contains all the active track data, we pass it on to the output buffer.
                for (int frameIndex = 0; frameIndex < numFrames * mChannelCount; ++frameIndex) {
                    //Note that += essentially mixes the contents of the current frame with the new track frame
                    //This here is the essence of the mixer, since it mixes the current track frame with the same frame for the previous tracks
                    audioData[frameIndex] += mMixingBuffer[frameIndex];
                }
            }
            else{
                //The track is playing, but the play ID is too old according to the polyphony settings. Stop the track
                mMixerTracks[trackIndex]->stopPlayingAndFading();
            }
            //Now repeat with the next active track
        }

    }

    //TODO - Add a MasterTrack to allow applying FX to the mixed audio as a whole (Dynamics Processing)
    //Makeshift compressor //TODO - Study and implement a real version using MasterTrack FX
    for(int frameIndex = 0; frameIndex < numFrames * mChannelCount; frameIndex++){
        //Positive side of wave
        if(audioData[frameIndex] >= COMPRESSOR_THRESHOLD){ //Compressor threshold approximately -3dB. Ratio 1:8
            audioData[frameIndex] = COMPRESSOR_THRESHOLD+((COMPRESSOR_RATIO)*(audioData[frameIndex] - COMPRESSOR_THRESHOLD));
            if(audioData[frameIndex] >= LIMITER_THRESHOLD){
                //Makeshift Limiter. Ratio 1:12
                audioData[frameIndex] = LIMITER_THRESHOLD+((LIMITER_RATIO)*(audioData[frameIndex] - LIMITER_THRESHOLD));
            }
        }
        //Negative side of wave
        else if(audioData[frameIndex] <= -COMPRESSOR_THRESHOLD){ //Compressor threshold approximately -3dB. Ratio 1:8
            audioData[frameIndex] = -COMPRESSOR_THRESHOLD+((COMPRESSOR_RATIO)*(audioData[frameIndex] + COMPRESSOR_THRESHOLD));
            if(audioData[frameIndex] <= -LIMITER_THRESHOLD){
                //Makeshift Limiter. Ratio 1:12
                audioData[frameIndex] = -LIMITER_THRESHOLD+((LIMITER_RATIO)*(audioData[frameIndex] + LIMITER_THRESHOLD));
            }
        }
    }

}

int8_t AudioMixer::addTrack(AudioFile *audioFile){
    int8_t trackIndex = mNextFreeTrackIndex;
    mMixerTracks[mNextFreeTrackIndex++] = new MixerTrack(audioFile,mSampleRate,mChannelCount);
    return trackIndex;
};
