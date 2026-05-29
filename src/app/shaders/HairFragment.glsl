#version 460

in float vertex_type;
in vec3 world_pos;
in vec2 uv;

uniform vec3 top_color;
uniform vec3 down_color;
uniform int nodes_per_hair;

uniform vec4 uLightPos;
uniform vec4 uLightDiffuse;
uniform vec4 uAmbientColor;

uniform sampler2D hairTexture;

out vec4 frag_color;

void main()
{
    vec4 tex_element = texture(hairTexture, uv);
    float hair_mask = tex_element.r;

    // Смешиваем базовый цвет
    vec3 base_color = mix(top_color, down_color, vertex_type);

    // Накладываем маску на базовый цвет, чтобы выделить отдельные волоски
    base_color *= hair_mask;

    // 2. Расчет освещения
    vec3 light_dir = normalize(uLightPos.xyz - world_pos);
    vec3 fakeNormal = vec3(0.0, 1.0, 0.0);
    float diffuse_factor = max(dot(fakeNormal, light_dir), 0.0);

    vec3 final_diffuse = uLightDiffuse.rgb * base_color * diffuse_factor;
    vec3 final_ambient = uAmbientColor.rgb * base_color;

    // 3. Формируем выходной цвет.
    // Альфа-канал равен маске из текстуры: где текстура была черной — там полная прозрачность.
    frag_color = vec4(final_ambient + final_diffuse, hair_mask);
}
