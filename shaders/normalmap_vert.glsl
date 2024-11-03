#version 410 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 fragWorldPos;
out vec2 fragTexCoords;
out vec3 fragLightPosTangent;
out vec3 fragViewPosTangent;
out vec3 fragPosTangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // Transform vertex position to world space
    fragWorldPos = vec3(model * vec4(vertexPos, 1.0));
    fragTexCoords = texCoords;
    
    // Construct TBN matrix
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * vertexNormal);
    T -= dot(T, N) * N;
    T = normalize(T);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    fragLightPosTangent = TBN * lightPos;
    fragViewPosTangent = TBN * viewPos;
    fragPosTangent = TBN * fragWorldPos;

    // Calculate final vertex position
    gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}
