#version 410 core

out vec4 FragColor;

in vec2 fragTexCoord;

uniform sampler2D m_texture;

void main() {
    FragColor = texture(m_texture, fragTexCoord);
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
