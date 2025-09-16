package com.example.ndk2;

// OpenGLDemoView.java
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.content.res.AssetManager;
import android.content.Context;

public class NativeRenderer implements GLSurfaceView.Renderer {
    static {
        System.loadLibrary("native-lib"); // 加载 C++ 库
    }

    private Context context;

    public NativeRenderer(Context context) {
        this.context = context;
    }

    // Native 方法声明
    public native void nativeSurfaceCreated();
    public native void nativeSurfaceChanged(int width, int height);
    public native void nativeDrawFrame();
    public native void initAssetManager(AssetManager assetManager);

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        AssetManager assetManager = context.getAssets();
        initAssetManager(assetManager);
        nativeSurfaceCreated(); // 调用 C++ 初始化
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        nativeSurfaceChanged(width, height); // 通知 C++ 尺寸变化
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        nativeDrawFrame(); // 调用 C++ 渲染
    }
}