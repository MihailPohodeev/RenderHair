#version 330 core
uniform sampler2D texMap;
in vec2 vUv;
out vec4 fragColor;

void main() {
    // vec3 col = vec3(1) - texMap;
    fragColor = texture(texMap, vUv);
}
