# RendererAPI 测试模块使用指南

## 概述

`renderer_test.cpp` 和 `renderer_test.h` 提供了一个完整的测试框架来使用 `renderer_api` 库。

## 主要功能

### 1. 初始化和清理

```cpp
// 初始化渲染器
rendererTestInit(width, height, resourcePrefix);

// 清理渲染器
rendererTestDeinit();
```

### 2. 更新和渲染

```cpp
// 每帧调用：更新场景状态
rendererTestUpdate();

// 每帧调用：渲染场景
rendererTestRender();
```

### 3. 资源管理

```cpp
// 设置 Asset 管理器和资源路径
rendererTestSetAssetManager(assetManager, internalPath);
```

### 4. 调试和性能监控

```cpp
// 获取调试信息
std::string info = rendererTestGetDebugInfo();

// 输出调试信息到 Logcat
rendererTestLogInfo();

// 记录性能数据（帧时间、FPS）
rendererTestLogPerformance(deltaTime);
```

## 使用示例

### 与 native_lib.cpp 集成

可以在 `native_lib.cpp` 中添加以下代码来并行运行两个渲染器：

```cpp
#include "renderer_test.h"

// 在 create() 函数中初始化
void create() {
    // 原有代码...
    
    // 初始化测试渲染器
    rendererTestInit(screenWidth, screenHeight, internalStoragePath);
}

// 在 draw() 函数中渲染
void draw() {
    // 可以选择使用原有渲染器或测试渲染器
    
    // 方案 1: 使用原有渲染器
    // ... 原有渲染代码 ...
    
    // 方案 2: 使用测试渲染器
    rendererTestUpdate();
    rendererTestRender();
}

// 在清理时调用
void cleanup() {
    rendererTestDeinit();
}
```

## 类和功能说明

### RendererAPI 类

- `init(width, height, resourcePrefix)` - 初始化渲染器
- `deinit()` - 清理资源
- `update()` - 更新场景逻辑
- `render()` - 渲染场景

### PerformanceMonitor 类

用于监控渲染性能：

- `addFrameTime(deltaTime)` - 添加帧时间数据
- `getAverageFrameTime()` - 获取平均帧时间（毫秒）
- `getFPS()` - 获取当前 FPS
- `logStats()` - 输出性能统计

## 编译配置

CMakeLists.txt 已配置：

```cmake
# 导入 Renderer API 库
add_library(render_explore_by_opengl SHARED IMPORTED)
set_target_properties(render_explore_by_opengl PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/librender_explore_by_opengl.so)

# 链接库
target_link_libraries(${CMAKE_PROJECT_NAME} 
    render_explore_by_opengl
    assimp
    android
    log
    GLESv3)
```

## 预期输出

### Logcat 日志示例

```
I/RendererTest: Initializing RendererAPI with resolution 1080x1920
I/RendererTest: Resource prefix: /data/data/com.example.ndk2/files
I/RendererTest: RendererAPI initialized successfully!
I/RendererTest: Performance Stats:
I/RendererTest:   Average Frame Time: 16.67 ms
I/RendererTest:   FPS: 60.0
I/RendererTest:   Samples: 60/60
```

## 常见问题

### Q: 如何切换使用原有渲染器或测试渲染器？

A: 在 `draw()` 函数中添加条件判断：

```cpp
bool USE_RENDERER_API = false;  // 切换开关

void draw() {
    if (USE_RENDERER_API) {
        rendererTestRender();
    } else {
        // 原有渲染代码
    }
}
```

### Q: 如何调试库加载失败？

A: 查看 Logcat 中的错误日志：

```
E/RendererTest: Error during RendererAPI init: ...
```

### Q: 性能数据如何解读？

A: 
- 平均帧时间 16.67 ms = 60 FPS
- 平均帧时间 33.33 ms = 30 FPS
- 检查 Samples 数量确保有足够的数据

## 文件列表

- `renderer_test.h` - 头文件，包含所有公共接口
- `renderer_test.cpp` - 实现文件，包含所有功能的实现
- `renderer_api.h` - Renderer API 库的头文件（来自第三方库）

## 依赖关系

```
renderer_test.cpp
├── renderer_api.h (第三方库头文件)
├── native_lib.h (如需集成)
├── assimp (第三方模型加载库)
└── glm (数学库)
```
