package com.example.ndk2;

import android.opengl.GLES30;
import android.util.Log;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class Triangle {
    private static final String TAG = "Triangle";

    // 顶点着色器代码（修正gl_FragColor并添加精度声明）
    private final String vertexShaderCode =
            "uniform mat4 uMVPMatrix;\n" +
            "attribute highp vec4 vPosition;\n" +
            "void main() {\n" +
            "  gl_Position = uMVPMatrix * vPosition;\n" +
            "}";

    // 片段着色器代码（修正为gl_FragColor）
    private final String fragmentShaderCode =
            "precision mediump float;\n" +
            "uniform vec4 vColor;\n" +
            "void main() {\n" +
            "  gl_FragColor = vColor;\n" +
            "}";

    private final FloatBuffer vertexBuffer;
    private final int mProgram;
    private int mPositionHandle;
    private int mColorHandle;
    private int mMVPMatrixHandle;

    // 顶点坐标（每个顶点3个坐标值）
    private static final int COORDS_PER_VERTEX = 3;
    private float[] mTriangleCoords;
    private float[] mColor;

    private final int vertexCount;
    private final int vertexStride = COORDS_PER_VERTEX * 4; // 每个顶点4字节

    public Triangle() {
        this( // 默认构造一个绿色三角形
            new float[] {
                0.0f,  0.5f, 0.0f,   // 顶部
                -0.5f, -0.5f, 0.0f,  // 左下
                0.5f, -0.5f, 0.0f    // 右下
            },
            new float[] { 0.63671875f, 0.76953125f, 0.22265625f, 1.0f } // RGBA
        );
    }

    public Triangle(float[] coords, float[] color) {
        if (coords == null || coords.length % COORDS_PER_VERTEX != 0) {
            throw new IllegalArgumentException("Invalid vertex coordinates");
        }
        if (color == null || color.length != 4) {
            throw new IllegalArgumentException("Color must have 4 components (RGBA)");
        }

        this.mTriangleCoords = coords.clone();
        this.mColor = color.clone();
        this.vertexCount = coords.length / COORDS_PER_VERTEX;

        // 初始化顶点缓冲区
        ByteBuffer bb = ByteBuffer.allocateDirect(coords.length * 4);
        bb.order(ByteOrder.nativeOrder());
        vertexBuffer = bb.asFloatBuffer();
        vertexBuffer.put(coords);
        vertexBuffer.position(0);

        // 编译着色器
        int vertexShader = loadShader(GLES30.GL_VERTEX_SHADER, vertexShaderCode);
        int fragmentShader = loadShader(GLES30.GL_FRAGMENT_SHADER, fragmentShaderCode);

        // 创建OpenGL程序
        mProgram = GLES30.glCreateProgram();
        if (mProgram == 0) {
            throw new RuntimeException("Failed to create OpenGL program");
        }

        // 附加着色器
        GLES30.glAttachShader(mProgram, vertexShader);
        GLES30.glAttachShader(mProgram, fragmentShader);

        // 链接程序
        GLES30.glLinkProgram(mProgram);

        // 检查链接状态
        int[] linkStatus = new int[1];
        GLES30.glGetProgramiv(mProgram, GLES30.GL_LINK_STATUS, linkStatus, 0);
        if (linkStatus[0] != GLES30.GL_TRUE) {
            String errorMsg = GLES30.glGetProgramInfoLog(mProgram);
            GLES30.glDeleteProgram(mProgram);
            throw new RuntimeException("Program link failed: " + errorMsg);
        }

        // 释放着色器资源
        GLES30.glDeleteShader(vertexShader);
        GLES30.glDeleteShader(fragmentShader);
    }

    private int loadShader(int type, String shaderCode) {
        int shader = GLES30.glCreateShader(type);
        if (shader == 0) {
            throw new RuntimeException("Failed to create shader");
        }

        GLES30.glShaderSource(shader, shaderCode);
        GLES30.glCompileShader(shader);

        // 检查编译状态
        int[] compiled = new int[1];
        GLES30.glGetShaderiv(shader, GLES30.GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            String errorMsg = GLES30.glGetShaderInfoLog(shader);
            GLES30.glDeleteShader(shader);
            throw new RuntimeException("Shader compilation failed: " + errorMsg);
        }

        return shader;
    }

    public void draw(float[] mvpMatrix) {
        if (mvpMatrix == null || mvpMatrix.length < 16) {
            throw new IllegalArgumentException("MVP matrix must have at least 16 elements");
        }

        // 使用程序
        GLES30.glUseProgram(mProgram);

        // 获取并启用顶点属性
        mPositionHandle = GLES30.glGetAttribLocation(mProgram, "vPosition");
        GLES30.glEnableVertexAttribArray(mPositionHandle);

        // 设置顶点数据
        GLES30.glVertexAttribPointer(
                mPositionHandle, COORDS_PER_VERTEX,
                GLES30.GL_FLOAT, false,
                vertexStride, vertexBuffer);

        // 设置颜色
        mColorHandle = GLES30.glGetUniformLocation(mProgram, "vColor");
        GLES30.glUniform4fv(mColorHandle, 1, mColor, 0);

        // 设置变换矩阵
        mMVPMatrixHandle = GLES30.glGetUniformLocation(mProgram, "uMVPMatrix");
        GLES30.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);

        // 绘制三角形
        GLES30.glDrawArrays(GLES30.GL_TRIANGLES, 0, vertexCount);

        // 禁用顶点属性
        GLES30.glDisableVertexAttribArray(mPositionHandle);

        // 检查OpenGL错误
        int error;
        while ((error = GLES30.glGetError()) != GLES30.GL_NO_ERROR) {
            Log.e(TAG, "OpenGL error: 0x" + Integer.toHexString(error));
        }
    }

    // 可选：提供修改颜色的方法
    public void setColor(float[] color) {
        if (color == null || color.length != 4) {
            throw new IllegalArgumentException("Color must have 4 components (RGBA)");
        }
        this.mColor = color.clone();
    }
}