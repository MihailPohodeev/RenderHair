#version 430 core

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

// Структура данных (должна совпадать с C++)
struct Particle {
    vec4 pos; // x, y, z, w
};

// SSBO - Shader Storage Buffer Object
layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

void main() {
    uint id = gl_GlobalInvocationID.x;
    // Пример: двигаем частицу вверх
    particles[id].pos.y += 0.01;
}
