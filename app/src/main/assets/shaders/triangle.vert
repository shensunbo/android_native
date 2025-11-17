#version 300 es
precision highp float;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;  // 添加颜色属性

out vec3 ourNormal;
out vec3 ourColor;

uniform mat4 uModel;       // 模型变换矩阵
uniform mat4 uView;        // 视图矩阵
uniform mat4 uProjection;  // 投影矩阵

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
    ourNormal = mat3(transpose(inverse(uModel))) * aNormal;  // 变换法线
    ourColor = aColor;
}