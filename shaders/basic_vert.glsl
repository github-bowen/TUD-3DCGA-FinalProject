#version 410 core
layout (location = 0) in vec3 position; // Vertex position
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords; // Texture coordinates

out vec2 TexCoords; // Pass texture coordinates to fragment shader
out vec3 FragPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{
    
    vec3 offset = vec3(-5.0, 0.0, 0.0);
    FragPos = vec3(model * vec4(position, 1.0)); 
    TexCoords = texCoords; // Assign texture coordinates

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * normal);

    gl_Position = projection * view * model * vec4(position + offset, 1.0);
}





