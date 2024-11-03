#version 410 core
out vec4 FragColor;

in vec3 fragWorldPos;
in vec2 fragTexCoords;
in vec3 fragLightPosTangent;
in vec3 fragViewPosTangent;
in vec3 fragPosTangent;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{           
    // Retrieve normal from normal map and transform to [-1, 1] range
    vec3 mappedNormal = texture(normalMap, fragTexCoords).rgb;
    mappedNormal = normalize(mappedNormal * 2.0 - 1.0);  
    
    // Retrieve base color from texture
    vec3 baseColor = texture(diffuseMap, fragTexCoords).rgb;
    
    // Ambient lighting
    vec3 ambientLight = 0.2 * baseColor;
    
    // Diffuse lighting
    vec3 lightDirection = normalize(fragLightPosTangent - fragPosTangent);
    float diffuseFactor = max(dot(lightDirection, mappedNormal), 0.0);
    vec3 diffuseLight = diffuseFactor * baseColor;
    
    
    // Combine lighting components
    FragColor = vec4(ambientLight + diffuseLight, 1.0);
}
