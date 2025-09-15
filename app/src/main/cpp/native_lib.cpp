// native-lib.cpp
#include <GLES3/gl3.h>
#include <android/log.h>
#include <jni.h>
#include <cassert>

#define TAG "NativeRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 顶点着色器源码
static const char* vertexShaderSource = 
"#version 300 es\n"
"layout(location = 0) in vec3 aPosition;\n"
"layout(location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main() {\n"
"    gl_Position = vec4(aPosition, 1.0);\n"
"    ourColor = aColor;\n"
"}\n";

static const char* fragmentShaderSource = 
"#version 300 es\n"
"precision mediump float;\n"
"out vec4 fragColor;\n"
"uniform vec4 uColor;\n"
"in vec3 ourColor;\n"
"void main() {\n"
"    fragColor = vec4(ourColor, 1.0);\n"
"}\n";

static GLuint program;
static GLuint vao;
static GLuint vbo;

void  create() {
    LOGI("nativeSurfaceCreated called");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // C++ 中设置清屏颜色
    // 创建和编译顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        LOGE("Vertex shader compilation failed: %s", infoLog);
        assert(0);
    } else {
        LOGI("Vertex shader compiled successfully");
    }

    // 创建和编译片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        LOGE("Fragment shader compilation failed: %s", infoLog);
        assert(0);
    } else {
        LOGI("Fragment shader compiled successfully");
    }

    // 创建着色器程序
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        LOGE("Shader program linking failed: %s", infoLog);
        assert(0);
    } else {
        LOGI("Shader program linked successfully");
    }

    // 删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 三角形顶点数据（三个点）
    GLfloat vertices[] = {
            -0.9f, -0.8f, 0.0f,  1.0f, 0.0f, 0.0f,// 左下角
            0.9f, -0.8f, 0.0f,   1.0f, 1.0f, 0.0f,// 右下角
            0.0f,  0.7f, 0.0f,0.0f, 1.0f, 1.0f// 顶部中点
    };

    // 创建并绑定VAO和VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    LOGI("nativeSurfaceCreated completed");

}

void  update(int width, int height) {
    glViewport(0, 0, width, height); // 设置视口
    LOGI("nativeSurfaceChanged called, width: %d, height: %d", width, height);
}

void draw() {
    // 清屏（黑色背景）
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 使用着色器程序
    glUseProgram(program);

    // 设置颜色（红色）
    GLint colorLoc = glGetUniformLocation(program, "uColor");
    glUniform4f(colorLoc, 0.0f, 0.0f, 1.0f, 1.0f);

    // 绘制三角形
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3); // 改为绘制三角形
}