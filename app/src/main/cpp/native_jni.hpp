#pragma once

// native-lib.cpp
#include <jni.h>
#include "native_lib.hpp"

extern "C" {
    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeSurfaceCreated(JNIEnv* env, jobject obj) {
        create();
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeSurfaceChanged(JNIEnv* env, jobject obj, jint width, jint height) {
        update(width, height);
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeDrawFrame(JNIEnv* env, jobject obj) {
        draw();
    }
}