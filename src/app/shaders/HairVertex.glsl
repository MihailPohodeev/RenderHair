#version 460

in vec4 vertex;

out float v_type_raw;
out vec3 v_pos_raw;

void main()
{
    v_type_raw = vertex.w;
    v_pos_raw = vertex.xyz;
    gl_Position = vec4(v_pos_raw, 1.0);
}
