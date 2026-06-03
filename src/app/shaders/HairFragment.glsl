#version 460

in float vertex_type;
in vec3 world_pos;
in vec2 uv;
in vec3 hair_tangent;

uniform vec3 top_color;
uniform vec3 down_color;
uniform int nodes_per_hair;

uniform vec4 uLightPos;
uniform vec4 uLightDiffuse;
uniform vec4 uAmbientColor;
uniform vec3 cameraPosition;

uniform vec3 specular_color;
uniform float specular_power;

uniform sampler2D hairTexture;

out vec4 frag_color;

void main()
{
    vec3 base_color = mix(top_color, down_color, vertex_type);

    vec3 T = normalize(hair_tangent);
    vec3 E = normalize(cameraPosition - world_pos);
    vec3 L = normalize(uLightPos.xyz - world_pos);

    float dotTL = dot(T, L);
    float dotTE = dot(T, E);

    float sinTL = sqrt(max(0.0, 1.0 - dotTL * dotTL));
    float sinTE = sqrt(max(0.0, 1.0 - dotTE * dotTE));

    float diffuse_factor = sinTL;
    vec3 final_diffuse = uLightDiffuse.rgb * base_color * diffuse_factor;

    float kajiya_cos = dotTL * dotTE + sinTL * sinTE;
    float specular_factor = pow(max(0.0, kajiya_cos), specular_power);

    vec3 mixed_specular_color = mix(specular_color, base_color, 0.80);
    vec3 final_specular = uLightDiffuse.rgb * mixed_specular_color * specular_factor;

    vec3 final_ambient = uAmbientColor.rgb * base_color;

    vec3 total_color = (final_ambient + final_diffuse) + final_specular;

    frag_color = vec4(total_color, 1.0);
}

