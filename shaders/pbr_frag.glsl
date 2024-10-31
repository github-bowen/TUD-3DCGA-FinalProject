#version 410

#define MAX_LIGHTS 10
const float PI = 3.14159265359;

out vec4 FragColor;
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
in mat3 TBN;

layout(std140) uniform Material // Must match the GPUMaterial defined in src/mesh.h
{
    vec3 kd;
	vec3 ks;
    vec3 albedo;
    float roughness;
    float metallic;
    float ao;
    float shininess;
	float transparency;
};

uniform sampler2D normalMap;
uniform sampler2D albedoMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D aoMap;
uniform bool normalMapping;
uniform bool albedoTex;
uniform bool roughnessTex;
uniform bool metallicTex;
uniform bool aoTex;

struct Light{
    vec3 position;
    vec3 color;
    //vec3 direction;
};
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform vec3 cameraPos;

uniform int shadingMode;
uniform bool useMaterial;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

void main()
{
    vec3 N = normalize(fragNormal);
    if(normalMapping){
        N = texture(normalMap, fragTexCoord).rgb;
        N = N*2.0 - 1.0;
        N = normalize(TBN * N);
    }
    vec3 V = normalize(cameraPos - fragPosition);
    vec3 localAlbedo = albedo;
    float localRoughness = roughness;
    float localMetallic = metallic;
    float localAo = ao;

    if(albedoTex){localAlbedo = pow(texture(albedoMap, fragTexCoord).rgb, vec3(2.2));}
    if(roughnessTex){localRoughness = texture(roughnessMap, fragTexCoord).r;}
    if(metallicTex){localMetallic = texture(metallicMap, fragTexCoord).r;}
    if(aoTex){localAo = texture(aoMap, fragTexCoord).r;}

    vec3 specula = vec3(1.0);
    vec3 Lo = vec3(0.0);
    vec3 bPhongSpecular = vec3(0.0);
    vec3 simpleLam = vec3(0.0);
    for(int i=0; i<numLights; ++i){
        //PBR computation
        vec3 L = normalize(lights[i].position - fragPosition);
        vec3 H = normalize(V + L);

        float distance = length(lights[i].position - fragPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lights[i].color * attenuation;

        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, localAlbedo, localMetallic);

        float NDF = DistributionGGX(N, H, localRoughness);        
        float G = GeometrySmith(N, V, L, localRoughness);      
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - localMetallic;
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;  
        
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * localAlbedo / PI + specular) * radiance * NdotL; 

        //Simple computation
        float bPhongSpec = pow(max(dot(N, H), 0.0), shininess);
        float lamDiff = max(dot(L, N), 0.0);
        bPhongSpecular += ks * bPhongSpec * lights[i].color;
        simpleLam += kd * lamDiff * lights[i].color;
    }

    vec3 ambient = vec3(0.03) * localAlbedo * localAo;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
    vec3 simpleColor = bPhongSpecular + simpleLam;

    if (useMaterial) {
        if(shadingMode == 0)    FragColor = vec4(simpleColor, 1.0);
        else if(shadingMode == 1)   FragColor = vec4(color, 1.0);
    }
    else { FragColor = vec4(N, 1); }
}