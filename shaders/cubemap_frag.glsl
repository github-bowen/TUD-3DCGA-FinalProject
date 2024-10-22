#version 410

uniform vec3 cameraPos;
uniform samplerCube sampler_cube;

in vec3 fragNormal;
in vec3 fragPosition;

out vec4 fragColor;

void main()
{             
    vec3 I = normalize(fragPosition - cameraPos);
    vec3 R = reflect(I, normalize(fragNormal));
    fragColor = vec4(texture(sampler_cube, vec3(-R.x, R.yz)).rgb, 1.0);
}