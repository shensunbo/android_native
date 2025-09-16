package com.example.ndk2;

// OpenGLDemoView.java
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class OpenGLDemoView extends GLSurfaceView {
    private NativeRenderer renderer;

    public OpenGLDemoView(Context context) {
        super(context);
        init(context);
    }

    public OpenGLDemoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    private void init(Context context) {
        // Set OpenGL ES 3.0 context
        setEGLContextClientVersion(3);
        
        // Create and set renderer
        renderer = new NativeRenderer(context);
        setRenderer(renderer);
        
        // Render only when needed (for better performance)
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }
}