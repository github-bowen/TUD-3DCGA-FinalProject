#version 410

uniform mat4 projection;
uniform mat4 view;

in vec3 aPos;

out vec3 texCoords;

void main()
{   
    // NOTE: invert x coordinate to view from `inside` of the cube
    texCoords =  aPos;  // vec3(-aPos.x, aPos.y, aPos.z); //

    gl_Position = (projection * view * vec4(aPos, 1.0)).xyww;
} 