#version 410 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


in vec2 TexCoords; // Receive texture coordinates from vertex shader
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D sceneTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 texColor = texture(sceneTexture, TexCoords).rgb;


    vec3 normal = normalize(Normal);  
    vec3 ambient = 0.2 * texColor;   
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diff = max(dot(lightDir, normal), 0.0) * texColor;
    vec3 result = ambient + diff;


    // Threshold to find bright areas
    float brightness = max(texColor.r, max(texColor.g, texColor.b));
    if (brightness > 1.0) {
        BrightColor = vec4(result, 1.0); // Output color if bright
    } else {
        BrightColor = vec4(0.0); // No output for dark areas
    }
    FragColor = vec4(result, 1.0);
}