#version 410 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D sprite;

void main()
{
    
    color = texture(sprite, TexCoords);
}  