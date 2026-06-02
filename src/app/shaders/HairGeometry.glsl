#version 460

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in float v_type_raw[];
in vec3 v_pos_raw[];

uniform mat4 viewProjMatrix;
uniform mat4 worldMatrix;
uniform vec3 cameraPosition;
uniform float hairWidth;
uniform float texScaleY;

out float vertex_type;
out vec3 world_pos;
out vec2 uv;
out vec3 hair_tangent;

void main()
{
    vec3 p0 = v_pos_raw[0];
    vec3 p1 = v_pos_raw[1];

    vec3 hair_dir = normalize(p1 - p0);

    vec3 center = (p0 + p1) * 0.5;
    vec3 eye_dir = normalize(cameraPosition - center);

    // 3. Биллбординг: вектор "вбок"
    vec3 right = normalize(cross(hair_dir, eye_dir));
    vec3 offset = right * (hairWidth * 0.5);

    float uv_y0 = v_type_raw[0] * texScaleY;
    float uv_y1 = v_type_raw[1] * texScaleY;

    // Вершина 1: Лево-Низ (у точки p0)
    world_pos = p0 - offset;
    vertex_type = v_type_raw[0];
    uv = vec2(0.0, uv_y0);
    hair_tangent = hair_dir;
    gl_Position = viewProjMatrix * vec4(world_pos, 1.0); // Переводим уже мировую позицию в клип-спейс
    EmitVertex();

    // Вершина 2: Право-Низ (у точки p0)
    world_pos = p0 + offset;
    vertex_type = v_type_raw[0];
    uv = vec2(1.0, uv_y0);
    hair_tangent = hair_dir;
    gl_Position = viewProjMatrix * vec4(world_pos, 1.0);
    EmitVertex();

    // Вершина 3: Лево-Верх (у точки p1)
    world_pos = p1 - offset;
    vertex_type = v_type_raw[1];
    uv = vec2(0.0, uv_y1);
    hair_tangent = hair_dir;
    gl_Position = viewProjMatrix * vec4(world_pos, 1.0);
    EmitVertex();

    // Вершина 4: Право-Верх (у точки p1)
    world_pos = p1 + offset;
    vertex_type = v_type_raw[1];
    uv = vec2(1.0, uv_y1);
    hair_tangent = hair_dir;
    gl_Position = viewProjMatrix * vec4(world_pos, 1.0);
    EmitVertex();

    EndPrimitive();
}
