// native-lib.cpp
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

#define TAG "NativeRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 模型数据结构
struct ModelData {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLsizei indexCount;
    bool loaded;
    float scale;  // 模型缩放比例
};

static GLuint program;
static GLuint vao;
static GLuint vbo;
static AAssetManager* assetManager;
static ModelData ballModel = {0, 0, 0, 0, false, 1.0f};
static float rotationAngle = 0.0f;  // 旋转角度
static std::string internalStoragePath;  // 内部存储路径

static void testAssimp();
static bool loadModelFromFile(const char* filePath, ModelData* modelData);
static std::string getFilePath(const char* relativePath);

static std::string readShaderFromAsset(const char* filePath) {
    // 构建完整的文件路径
    std::string fullPath = getFilePath(filePath);
    
    // 从文件系统读取
    FILE* f = fopen(fullPath.c_str(), "rb");
    if (!f) {
        LOGE("Failed to open shader file: %s", fullPath.c_str());
        return "";
    }
    
    // 获取文件大小
    fseek(f, 0, SEEK_END);
    size_t fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    // 读取内容
    std::string content;
    content.resize(fileSize);
    fread(&content[0], 1, fileSize, f);
    fclose(f);
    
    return content;
}

void  create() {
    LOGI("nativeSurfaceCreated called");

    std::string vertexCode = readShaderFromAsset("shaders/triangle.vert");
    std::string fragmentCode = readShaderFromAsset("shaders/triangle.frag");
    
    if (vertexCode.empty() || fragmentCode.empty()) {
        LOGE("Failed to load shaders");
        return;
    }

    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

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

    // 三角形顶点数据（位置 + 法线 + 颜色）
    GLfloat vertices[] = {
            // 位置                   // 法线                // 颜色
            -0.9f, -0.8f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  // 左下角 - 红色
            0.9f, -0.8f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  // 右下角 - 绿色
            0.0f,  0.7f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f   // 顶部 - 蓝色
    };

    // 创建并绑定VAO和VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 设置顶点属性指针 - 位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 设置顶点属性指针 - 法线
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 设置顶点属性指针 - 颜色
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    LOGI("nativeSurfaceCreated completed");
    
    // 加载 ball.fbx 模型（从私有目录）
    std::string modelPath = getFilePath("model/ball.fbx");
    LOGI("Loading ball.fbx model from: %s", modelPath.c_str());
    
    if (loadModelFromFile(modelPath.c_str(), &ballModel)) {
        LOGI("Ball model loaded successfully!");
    } else {
        LOGE("Failed to load ball model!");
    }

}

void  update(int width, int height) {
    glViewport(0, 0, width, height); // 设置视口
    LOGI("nativeSurfaceChanged called, width: %d, height: %d", width, height);
}

void draw() {
    // 清屏（灰色背景）
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 使用着色器程序
    glUseProgram(program);
    
    // 更新旋转角度
    rotationAngle += 0.5f;  // 每帧旋转0.5度
    if (rotationAngle >= 360.0f) {
        rotationAngle -= 360.0f;
    }
    
    // 创建变换矩阵
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // 如果模型已加载，渲染模型；否则渲染三角形
    if (ballModel.loaded) {
        // 设置模型变换：缩小并旋转
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(ballModel.scale));
        
        // 设置视图矩阵（相机后退一些）
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        
        // 设置透视投影
        projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
        
        // 传递矩阵到着色器
        GLint modelLoc = glGetUniformLocation(program, "uModel");
        GLint viewLoc = glGetUniformLocation(program, "uView");
        GLint projLoc = glGetUniformLocation(program, "uProjection");
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // 绘制球模型
        glBindVertexArray(ballModel.vao);
        glDrawElements(GL_TRIANGLES, ballModel.indexCount, GL_UNSIGNED_INT, 0);
    } else {
        // 三角形也使用矩阵变换
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
        projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
        
        GLint modelLoc = glGetUniformLocation(program, "uModel");
        GLint viewLoc = glGetUniformLocation(program, "uView");
        GLint projLoc = glGetUniformLocation(program, "uProjection");
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // 绘制三角形
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

void initAssetManager(AAssetManager* assetPtr, const char* storagePath){
    assert(assetPtr);
    assert(storagePath);

    assetManager = assetPtr;
    internalStoragePath = storagePath;

    LOGI("initAssetManager success");
    LOGI("Internal storage path: %s", internalStoragePath.c_str());
    
    // 测试 Assimp 库
    testAssimp();
}

// Assimp 测试函数
static void testAssimp() {
    LOGI("Testing Assimp library...");
    
    // 获取 Assimp 版本信息
    unsigned int major = aiGetVersionMajor();
    unsigned int minor = aiGetVersionMinor();
    unsigned int revision = aiGetVersionRevision();
    
    LOGI("Assimp version: %u.%u.%u", major, minor, revision);
    
    // 创建 Assimp Importer 对象测试
    Assimp::Importer importer;
    LOGI("Assimp Importer created successfully!");
    
    // 获取支持的导入格式数量
    size_t formatCount = importer.GetImporterCount();
    LOGI("Assimp supports %zu import formats", formatCount);
    
    LOGI("Assimp library test completed successfully!");
}

// 获取文件的完整路径（私有目录中）
static std::string getFilePath(const char* relativePath) {
    return internalStoragePath + "/" + relativePath;
}

// 从文件路径加载模型（跨平台标准方式）
static bool loadModelFromFile(const char* filePath, ModelData* modelData) {
    LOGI("Loading model from file: %s", filePath);
    
    // 检查文件是否存在
    FILE* f = fopen(filePath, "rb");
    if (!f) {
        LOGE("File not found: %s", filePath);
        return false;
    }
    fclose(f);
    
    // 使用 Assimp 从文件路径加载（跨平台标准方式）
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        filePath,
        aiProcess_Triangulate | 
        aiProcess_FlipUVs | 
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices
    );
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOGE("Assimp error: %s", importer.GetErrorString());
        return false;
    }
    
    LOGI("Model loaded successfully! Meshes: %u", scene->mNumMeshes);
    
    if (scene->mNumMeshes == 0) {
        LOGE("No meshes found in model");
        return false;
    }
    
    // 处理第一个网格
    aiMesh* mesh = scene->mMeshes[0];
    LOGI("Mesh vertices: %u, faces: %u", mesh->mNumVertices, mesh->mNumFaces);
    
    // 计算模型边界盒以确定缩放比例
    glm::vec3 minPos(FLT_MAX), maxPos(-FLT_MAX);
    
    // 提取顶点数据（位置 + 法线 + 颜色）
    std::vector<float> vertices;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // 位置
        float x = mesh->mVertices[i].x;
        float y = mesh->mVertices[i].y;
        float z = mesh->mVertices[i].z;
        
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
        
        // 更新边界盒
        minPos.x = std::min(minPos.x, x);
        minPos.y = std::min(minPos.y, y);
        minPos.z = std::min(minPos.z, z);
        maxPos.x = std::max(maxPos.x, x);
        maxPos.y = std::max(maxPos.y, y);
        maxPos.z = std::max(maxPos.z, z);
        
        // 法线
        if (mesh->HasNormals()) {
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
        } else {
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
        }
        
        // 颜色（从顶点颜色或材质）
        if (mesh->HasVertexColors(0)) {
            vertices.push_back(mesh->mColors[0][i].r);
            vertices.push_back(mesh->mColors[0][i].g);
            vertices.push_back(mesh->mColors[0][i].b);
        } else {
            // 默认白色，会使用光照效果
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);
        }
    }
    
    // 计算模型尺寸和缩放比例
    glm::vec3 size = maxPos - minPos;
    float maxSize = std::max(std::max(size.x, size.y), size.z);
    modelData->scale = 1.0f / maxSize;  // 缩放到单位大小
    
    LOGI("Model size: %.2f x %.2f x %.2f, scale: %.3f", size.x, size.y, size.z, modelData->scale);
    
    // 提取索引数据
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    
    LOGI("Extracted vertices: %zu, indices: %zu", vertices.size() / 9, indices.size());
    
    // 创建 VAO, VBO, EBO
    glGenVertexArrays(1, &modelData->vao);
    glGenBuffers(1, &modelData->vbo);
    glGenBuffers(1, &modelData->ebo);
    
    glBindVertexArray(modelData->vao);
    
    // 上传顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, modelData->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // 上传索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelData->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // 设置顶点属性 - 位置 (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 设置顶点属性 - 法线 (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 设置顶点属性 - 颜色 (location = 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    modelData->indexCount = indices.size();
    modelData->loaded = true;
    
    LOGI("Model uploaded to GPU successfully!");
    return true;
}