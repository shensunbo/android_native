#pragma once

#include <string>
#include <android/asset_manager.h>

// ==================== 初始化和清理 ====================

/**
 * 初始化 RendererAPI
 * @param width 窗口宽度
 * @param height 窗口高度
 * @param resourcePrefix 资源目录前缀
 */
void rendererTestInit(int width, int height, const std::string& resourcePrefix = "");

/**
 * 清理 RendererAPI
 */
void rendererTestDeinit();

// ==================== 更新和渲染 ====================

/**
 * 更新场景（每帧调用一次）
 */
void rendererTestUpdate();

/**
 * 渲染场景（每帧调用一次）
 */
void rendererTestRender();

// ==================== 资源管理 ====================

/**
 * 设置 Asset 管理器和内部存储路径
 * @param assetManager Android 的 Asset 管理器
 * @param internalPath 应用私有目录路径
 */
void rendererTestSetAssetManager(AAssetManager* assetManager, const std::string& internalPath);

// ==================== 调试接口 ====================

/**
 * 获取调试信息
 * @return 调试信息字符串
 */
std::string rendererTestGetDebugInfo();

/**
 * 输出调试信息到日志
 */
void rendererTestLogInfo();

/**
 * 记录性能数据（帧时间、FPS 等）
 * @param deltaTime 帧间隔时间（毫秒）
 */
void rendererTestLogPerformance(float deltaTime);
