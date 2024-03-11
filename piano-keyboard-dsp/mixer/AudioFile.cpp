#include "AudioFile.h"

AudioFile * AudioFile::loadFromAssets(AAssetManager *assetManager, const char *filename, int32_t sampleRate, int8_t trackChannels) {

    //Load audio files
    AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);

    if (asset == nullptr){
        return nullptr;
    }

    // Get the length of the track
    off_t trackLengthInBytes = AAsset_getLength(asset);

    // Load it into memory
    const float *audioBuffer = static_cast<const float*>(AAsset_getBuffer(asset));

    if (audioBuffer == nullptr){
        return nullptr;
    }

    uint8_t bytesPerFrame = sizeof(float);
    uint64_t totalFrames = static_cast<uint64_t>(trackLengthInBytes / (bytesPerFrame * trackChannels));
    return new AudioFile(audioBuffer, totalFrames, sampleRate, trackChannels);
}
