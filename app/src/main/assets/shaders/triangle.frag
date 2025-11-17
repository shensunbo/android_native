#version 300 es
precision highp float;

out vec4 fragColor;
in vec3 ourNormal;
in vec3 ourColor;

void main() {
    // 简单的光照效果
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    vec3 normal = normalize(ourNormal);
    float diff = max(dot(normal, lightDir), 0.0);
    
    // 环境光 + 漫反射，使用顶点颜色
    vec3 ambient = 0.3 * ourColor;
    vec3 diffuse = diff * ourColor;
    
    fragColor = vec4(ambient + diffuse, 1.0);
}
