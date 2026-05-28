#version 150

in float vertexType;

uniform vec3 top_color;
uniform vec3 down_color;
uniform int nodes_per_hair;

out vec4 fragColor;

void main()
{
    vec3 color;

    if (vertexType <= float(nodes_per_hair - 1))
    {
        // Первый сегмент (от корня к середине): плавно переходим от Красного к Зеленому
        // vertexType идет от 0.0 до 1.0
        color = mix(top_color, down_color, vertexType / nodes_per_hair);
    }
    else
    {
        // Второй сегмент (от середины к кончику): оставляем его чисто зеленым
        // (или можешь увести в желтый/черный для кастомизации)
        color = down_color;
    }

    fragColor = vec4(color, 1.0);
}
