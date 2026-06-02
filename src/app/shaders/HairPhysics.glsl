#version 460 core

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout(std140, binding = 0) uniform PhysicsParams {
    mat4 world_matrix;
    vec4 gravity;
    float delta_time;
    float hair_segment_length;
    uint nodes_per_hair;
    uint total_hairs;
    uint is_first_frame;
};

layout(std430, binding = 1) buffer CurrentPositions {
    vec4 current_positions[];
};

layout(std430, binding = 2) buffer PreviousPositions {
    vec4 previous_positions[];
};

layout(std430, binding = 3) readonly buffer LocalRoots {
    vec4 local_roots[];
};

void main() {
    uint hair_index = gl_GlobalInvocationID.x;
    if (hair_index >= total_hairs) { return; }

    uint root_node_index = hair_index * nodes_per_hair;

    if (is_first_frame > 0u) {
        for (uint i = 0u; i < nodes_per_hair; ++i) {
            uint node_index = root_node_index + i;
            vec4 local_pos = local_roots[node_index];
            vec4 world_pos = world_matrix * vec4(local_pos.xyz, 1.0);

            current_positions[node_index] = world_pos;
            previous_positions[node_index] = world_pos;
        }
        return;
    }

    vec4 global_root_pos = world_matrix * vec4(local_roots[root_node_index].xyz, 1.0);
    current_positions[root_node_index] = global_root_pos;
    previous_positions[root_node_index] = global_root_pos;

    for (uint i = 1u; i < nodes_per_hair; ++i) {
        uint node_index = root_node_index + i;

        vec4 current_pos = current_positions[node_index];
        vec4 prev_pos = previous_positions[node_index];

        vec3 velocity = current_pos.xyz - prev_pos.xyz;
        vec3 new_pos = current_pos.xyz + velocity + gravity.xyz * (delta_time * delta_time);

        previous_positions[node_index] = current_pos;
        current_positions[node_index] = vec4(new_pos, 1.0);
    }

    const uint constraint_iterations = 4u;
    for (uint iter = 0u; iter < constraint_iterations; ++iter) {
        for (uint i = 0u; i < nodes_per_hair - 1u; ++i) {
            uint idx_A = root_node_index + i;
            uint idx_B = root_node_index + i + 1u;

            vec3 pos_A = current_positions[idx_A].xyz;
            vec3 pos_B = current_positions[idx_B].xyz;

            vec3 delta = pos_B - pos_A;
            float current_length = length(delta);

            if (current_length > 0.0001) {
                float diff = (segment_length - current_length) / current_length;
                vec3 correction = delta * diff * 0.5;
                if (i == 0u) {
                    current_positions[idx_B] = vec4(pos_B + correction * 2.0, 1.0);
                } else {
                    current_positions[idx_A] = vec4(pos_A - correction, 1.0);
                    current_positions[idx_B] = vec4(pos_B + correction, 1.0);
                }
            }
        }
    }
}
