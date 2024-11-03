#version 410 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 offset;
uniform vec4 color;

void main()
{
    float scale = 0.07f;
    TexCoords = vertex.zw;
    
    gl_Position = projection * view * model * vec4((vertex.xy * scale) + offset, 2.0, 1.0);
}