// native-lib.cpp
#include <GLES3/gl3.h>
#include <android/log.h>
#include <jni.h>

extern "C" {
    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeSurfaceCreated(JNIEnv* env, jobject obj) {
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f); // C++ 中设置清屏颜色
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeSurfaceChanged(JNIEnv* env, jobject obj, jint width, jint height) {
        glViewport(0, 0, width, height); // 设置视口
    }

    JNIEXPORT void JNICALL Java_com_example_ndk2_NativeRenderer_nativeDrawFrame(JNIEnv* env, jobject obj) {
        glClear(GL_COLOR_BUFFER_BIT); // 清屏
        // 在此处添加 C++ 渲染代码（如绘制三角形）
    }
}