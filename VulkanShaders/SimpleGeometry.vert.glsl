#version 460
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 uvMap;

struct Vertex
{
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
    vec4 color;
};

layout (buffer_reference, std430) readonly buffer VertexBuffer
{
    Vertex vertices[];
};

layout (push_constant) uniform constants
{
    mat4 model;
    VertexBuffer vertexBuffer;
}PushConstants;

void main()
{
    Vertex vertex = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = vec4(vertex.position, 1.0f);
    outColor = vertex.color;
    uvMap.x = vertex.uv_x;
    uvMap.y = vertex.uv_y;
}