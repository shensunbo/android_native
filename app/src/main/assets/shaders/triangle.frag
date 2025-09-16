#version 300 es
precision highp float;

out vec4 fragColor;
uniform vec4 uColor;
in vec3 ourColor;

void main() {
    fragColor = vec4(ourColor, 1.0);
}
