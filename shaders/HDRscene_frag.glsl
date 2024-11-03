#version 410
out vec4 FragColor;

in vec3 localPos;
  
uniform samplerCube environmentMap;
  
void main()
{
    vec3 envColor = texture(environmentMap, localPos).rgb;
    //vec3 envColor = textureLod(environmentMap, localPos, 4.0).rgb;
    
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
  
    FragColor = vec4(envColor, 1.0);
    //FragColor = vec4(vec3(1.0, 0.0, 0.0), 1.0);
}