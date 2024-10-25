#version 410

uniform samplerCube sampler_cube;

in vec3 texCoords;

out vec4 fragColor;

void main()
{    
    fragColor = texture(sampler_cube, texCoords);
}