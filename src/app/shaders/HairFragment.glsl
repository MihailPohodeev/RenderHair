#version 150

in float vertexType;
out vec4 fragColor;

void main()
{
    vec3 color;

    if (vertexType <= 1.0)
    {
        // Первый сегмент (от корня к середине): плавно переходим от Красного к Зеленому
        // vertexType идет от 0.0 до 1.0
        color = mix(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vertexType);
    }
    else
    {
        // Второй сегмент (от середины к кончику): оставляем его чисто зеленым
        // (или можешь увести в желтый/черный для кастомизации)
        color = vec3(0.0, 1.0, 0.0);
    }

    fragColor = vec4(color, 1.0);
}
