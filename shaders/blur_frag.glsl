#version 410 core
out vec4 color;

in vec2 TexCoords; // Receive texture coordinates from vertex shader
uniform sampler2D brightTexture;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
    vec2 texOffset = 1.0 / textureSize(brightTexture, 0); // Gets size of a single pixel
    vec3 result = texture(brightTexture, TexCoords).rgb * 0.227027; // Center sample

    if(horizontal)
     {
         for(int i = 1; i < 5; ++i)
         {
            result += texture(brightTexture, TexCoords + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
            result += texture(brightTexture, TexCoords - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
             result += texture(brightTexture, TexCoords + vec2(0.0, texOffset.y * i)).rgb * weight[i];
             result += texture(brightTexture, TexCoords - vec2(0.0, texOffset.y * i)).rgb * weight[i];
         }
     }

    color = vec4(result, 1.0); // Output blurred color
}