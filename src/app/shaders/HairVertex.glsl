#version 150

in vec4 vertex;
uniform mat4 worldViewProjMatrix;

out float vertexType;

void main()
{
    vertexType = vertex.w;
    vec4 cleanVertex = vec4(vertex.xyz, 1.0);
    gl_Position = worldViewProjMatrix * cleanVertex;
}
