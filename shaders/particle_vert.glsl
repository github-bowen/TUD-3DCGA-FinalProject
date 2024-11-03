#version 410 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 offset;
uniform vec4 color;
uniform float radius; // New uniform for the radius
uniform float life;   // New uniform for the particle's life


void main()
{
    float scale = 0.07f;
    TexCoords = vertex.zw;
    
    // Calculate the size based on life; you can adjust the growth factor
    float size = scale * radius * (life > 0.0 ? life : 0.0); // Prevent negative size
    gl_Position = projection * view * model * vec4((vertex.xy * size) + offset, 2.0, 1.0);
}