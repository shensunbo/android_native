// renderer_test.cpp
// 用于测试 renderer_api 库的文件

#include <GLES3/gl3.h>
#include <android/log.h>
#include <jni.h>
#include <cassert>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <string>

// Assimp 头文件
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>

#include <vector>
#include <algorithm>
#include <cfloat>
#include <sys/stat.h>

// GLM 头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Renderer API 头文件
#include "renderer_api.h"

#define TAG "RendererTest"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 全局渲染器对象
static RendererAPI* g_renderer = nullptr;
static std::string g_resourcePrefix;
static AAssetManager* g_assetManager = nullptr;

// ==================== 初始化和清理 ====================

void rendererTestInit(int width, int height, const std::string& resourcePrefix) {
    LOGI("Initializing RendererAPI with resolution %dx%d", width, height);
    LOGI("Resource prefix: %s", resourcePrefix.c_str());
    
    g_resourcePrefix = resourcePrefix;
    
    try {
        if (g_renderer != nullptr) {
            LOGD("Renderer already initialized, deleting old instance");
            delete g_renderer;
        }
        
        g_renderer = new RendererAPI();
        g_renderer->init(width, height, resourcePrefix);
        
        LOGI("RendererAPI initialized successfully!");
    } catch (const std::exception& e) {
        LOGE("Failed to initialize RendererAPI: %s", e.what());
        if (g_renderer != nullptr) {
            delete g_renderer;
            g_renderer = nullptr;
        }
    }
}

void rendererTestDeinit() {
    LOGI("Deinitializing RendererAPI");
    
    if (g_renderer != nullptr) {
        try {
            g_renderer->deinit();
            delete g_renderer;
            g_renderer = nullptr;
            LOGI("RendererAPI deinitialized successfully!");
        } catch (const std::exception& e) {
            LOGE("Error during RendererAPI deinitialization: %s", e.what());
        }
    }
}

// ==================== 更新和渲染 ====================

void rendererTestUpdate() {
    if (g_renderer != nullptr) {
        try {
            g_renderer->update();
        } catch (const std::exception& e) {
            LOGE("Error during RendererAPI update: %s", e.what());
        }
    } else {
        LOGD("Renderer not initialized, skipping update");
    }
}

void rendererTestRender() {
    if (g_renderer != nullptr) {
        try {
            // 清屏
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // 启用深度测试
            glEnable(GL_DEPTH_TEST);
            
            // 调用渲染器渲染
            g_renderer->render();
            
        } catch (const std::exception& e) {
            LOGE("Error during RendererAPI render: %s", e.what());
        }
    } else {
        LOGD("Renderer not initialized, skipping render");
    }
}

// ==================== 设置资源路径 ====================

void rendererTestSetAssetManager(AAssetManager* assetManager, const std::string& internalPath) {
    LOGI("Setting asset manager and resource path: %s", internalPath.c_str());
    g_assetManager = assetManager;
    // 确保路径以斜杠结尾
    if (!internalPath.empty() && internalPath.back() != '/') {
        g_resourcePrefix = internalPath + "/";
    } else {
        g_resourcePrefix = internalPath;
    }
    LOGI("Resource path set to: %s", g_resourcePrefix.c_str());
}

// ==================== 获取调试信息 ====================

std::string rendererTestGetDebugInfo() {
    std::string info = "RendererAPI Test Debug Info:\n";
    
    if (g_renderer != nullptr) {
        info += "  Status: Initialized\n";
        info += "  Resource Prefix: " + g_resourcePrefix + "\n";
    } else {
        info += "  Status: Not Initialized\n";
    }
    
    info += "  Assimp Version: " + std::to_string(aiGetVersionMajor()) + "." 
            + std::to_string(aiGetVersionMinor()) + "." 
            + std::to_string(aiGetVersionRevision()) + "\n";
    
    info += "  OpenGL ES available\n";
    
    return info;
}

// ==================== 日志和诊断 ====================

void rendererTestLogInfo() {
    LOGI("=== RendererAPI Test Info ===");
    LOGI("Renderer initialized: %s", (g_renderer != nullptr) ? "Yes" : "No");
    LOGI("Resource prefix: %s", g_resourcePrefix.c_str());
    
    std::string debugInfo = rendererTestGetDebugInfo();
    LOGI("Debug Info:\n%s", debugInfo.c_str());
    
    LOGI("=== End RendererAPI Test Info ===");
}

// ==================== 性能测试 ====================

class PerformanceMonitor {
private:
    std::vector<float> frameTimes;
    int maxSamples;
    
public:
    PerformanceMonitor(int maxSamples = 60) : maxSamples(maxSamples) {}
    
    void addFrameTime(float deltaTime) {
        frameTimes.push_back(deltaTime);
        if (frameTimes.size() > maxSamples) {
            frameTimes.erase(frameTimes.begin());
        }
    }
    
    float getAverageFrameTime() const {
        if (frameTimes.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (float t : frameTimes) {
            sum += t;
        }
        return sum / frameTimes.size();
    }
    
    float getFPS() const {
        float avgTime = getAverageFrameTime();
        if (avgTime <= 0.0f) return 0.0f;
        return 1000.0f / avgTime;  // 假设 avgTime 是毫秒
    }
    
    void logStats() {
        LOGI("Performance Stats:");
        LOGI("  Average Frame Time: %.2f ms", getAverageFrameTime());
        LOGI("  FPS: %.1f", getFPS());
        LOGI("  Samples: %zu/%d", frameTimes.size(), maxSamples);
    }
};

static PerformanceMonitor g_perfMonitor;

void rendererTestLogPerformance(float deltaTime) {
    g_perfMonitor.addFrameTime(deltaTime);
    
    // 每 60 帧输出一次性能信息
    static int frameCount = 0;
    frameCount++;
    
    if (frameCount >= 60) {
        g_perfMonitor.logStats();
        frameCount = 0;
    }
}
