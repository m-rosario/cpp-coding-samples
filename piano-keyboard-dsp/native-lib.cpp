//
// Created by Miguel on 4/6/2018.
//

#include "NativeSoundManager.h"

//This is executed when file is loaded into memory, as static code.
NativeSoundManager *nativeSoundManager = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_sample_keyboard_managers_JNISoundManager_nativeOnCreate(JNIEnv *env, jobject instance, jobject jAssetManager, jint sampleRate, jint framesPerBurst){
    if(nativeSoundManager == nullptr){
        //First time creation, initialize instance
        AAssetManager *assetManager = AAssetManager_fromJava(env, jAssetManager);
        nativeSoundManager = new NativeSoundManager(assetManager,sampleRate, framesPerBurst);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_sample_keyboard_managers_JNISoundManager_nativeOnStart(JNIEnv *env, jobject instance){
    nativeSoundManager->startAudioStream();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sample_keyboard_managers_JNISoundManager_nativeOnStop(JNIEnv *env, jobject instance){
    nativeSoundManager->pauseAudioStream();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sample_keyboard_managers_JNISoundManager_nativePlayNote(JNIEnv *env, jobject instance, jint noteIndex, jint velocity){
    nativeSoundManager->playNote(noteIndex, velocity);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sample_keyboard_managers_JNISoundManager_nativeEndNote(JNIEnv *env, jobject instance, jint noteIndex, jint velocity){
    nativeSoundManager->endNote(noteIndex, velocity);
}