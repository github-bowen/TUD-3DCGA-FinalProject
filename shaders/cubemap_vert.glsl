#version 410

uniform mat4 projection;
uniform mat4 view;
uniform mat4 modelMatrix;
uniform mat3 normalModelMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 fragNormal;
out vec3 fragPosition;

void main()
{
    vec3 offset = vec3(3.0, 0.0, 0.0);
    fragNormal = normalize(normalModelMatrix * normal);
    fragPosition = vec3(modelMatrix * vec4(position, 1.0));
    gl_Position = projection * view * modelMatrix * vec4(position + offset, 1.0);
}  
