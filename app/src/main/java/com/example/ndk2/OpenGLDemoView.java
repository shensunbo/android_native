package com.example.ndk2;

// OpenGLDemoView.java
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class OpenGLDemoView extends GLSurfaceView {
    private NativeRenderer renderer;

    public OpenGLDemoView(Context context) {
        super(context);
        init();
    }

    public OpenGLDemoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    private void init() {
        // Set OpenGL ES 3.0 context
        setEGLContextClientVersion(3);
        
        // Create and set renderer
        renderer = new NativeRenderer();
        setRenderer(renderer);
        
        // Render only when needed (for better performance)
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }
}