#version 330 core
in vec4 vertex;
in vec2 uv0;

uniform mat4 worldViewProj;

out vec2 vUv;

void main() {
    vUv = uv0;
    gl_Position = worldViewProj * vertex;
}
