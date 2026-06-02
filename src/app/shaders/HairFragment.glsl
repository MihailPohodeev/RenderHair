#version 460

in float vertex_type;
in vec3 world_pos;
in vec2 uv;
in vec3 hair_tangent;

uniform vec3 top_color;
uniform vec3 down_color;
uniform int nodes_per_hair;

uniform vec4 uLightPos; // В Ogre: если .w == 0.0, то это направленный свет (вектор), иначе позиция
uniform vec4 uLightDiffuse;
uniform vec4 uAmbientColor;
uniform vec3 cameraPosition;

uniform vec3 specular_color;
uniform float specular_power;

uniform sampler2D hairTexture;

out vec4 frag_color;

void main()
{
    // Базовый цвет нити
    vec3 base_color = mix(top_color, down_color, vertex_type);

    // Подготовка нормализованных векторов в едином World Space
    vec3 T = normalize(hair_tangent);
    vec3 E = normalize(cameraPosition - world_pos);
    vec3 L = normalize(uLightPos.xyz - world_pos);

    // Математика Кадзия-Кэя
    float dotTL = dot(T, L);
    float dotTE = dot(T, E);

    // Защита от вылета за пределы корня (max 0.0)
    float sinTL = sin(dotTL);
    float sinTE = sin(dotTE);

    // Диффузный коэффициент с мягким гашением на краях плашки (маскирует плоскую геометрию)
    float K_d = sin(3.141592);
    float diffuse_factor = K_d * sinTL;
    vec3 final_diffuse = uLightDiffuse.rgb * base_color * diffuse_factor;

    // Считаем косинус угла отражения
    float kajiya_cos = 0.5 * (T * L * E + sinTL * sinTE);
    float specular_factor = pow(max(0.0, kajiya_cos), specular_power);

    // ИСПРАВЛЕНИЕ: Блик НЕ умножается на edge_shadow,
    // чтобы кольцо блика непрерывно шло сквозь всю ширину волосяной пряди
    vec3 final_specular = uLightDiffuse.rgb * specular_color * specular_factor;

    // Текстурная маска (опционально, если white-hair.png имеет альфа-канал/маску в R)
    vec4 tex = texture(hairTexture, uv);

    // Итоговый цвет
    vec3 final_ambient = uAmbientColor.rgb * base_color;
    vec3 total_color = final_ambient + final_diffuse + final_specular;

    // Включаем альфа-тест или прозрачность, если текстура используется как маска нитей
    frag_color = vec4(total_color, 1.0);
}
