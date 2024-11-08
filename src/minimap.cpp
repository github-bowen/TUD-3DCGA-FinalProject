#include "minimap.h"

#include <stdexcept>
#include <iostream>

// Minimap constructor with specified width, height, and texture path
Minimap::Minimap(float width, float height, const std::string& texturePath)
    : m_width(width), m_height(height) {
    // Load the texture
    loadTexture(texturePath);

    // Set up the vertex data for the minimap
    setupMinimap();
}

// Minimap destructor
Minimap::~Minimap() {
    // Delete the texture, VAO, and VBO
    glDeleteTextures(1, &m_texture);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

// Load texture from the specified path
void Minimap::loadTexture(const std::string& texturePath) {
    // Load the texture using the Image class
    Image image(texturePath);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Determine the format and internal format based on the number of channels
    GLenum format = (image.channels == 4) ? GL_RGBA : GL_RGB;
    GLenum internalFormat = (image.channels == 4) ? GL_RGBA8 : GL_RGB8;

    // Upload the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.get_data());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Set up the minimap's vertex data
void Minimap::setupMinimap() {
    const float vertices[] = {
        // Positions          // Texture Coordinates
         m_width,  m_height, 0.0f, 1.0f, 1.0f,
         m_width, -m_height, 0.0f, 1.0f, 0.0f,
        -m_width, -m_height, 0.0f, 0.0f, 0.0f,
        -m_width,  m_height, 0.0f, 0.0f, 1.0f
    };

    const GLuint indices[] = { 0, 1, 3, 1, 2, 3 };

    // Generate and bind the VAO and VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);

    // Set VBO data
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

// Draw the minimap using the specified shader, model matrix, view matrix, and projection matrix
void Minimap::draw(Shader& shader, const glm::mat4& modelMatrix, const glm::mat4& view, const glm::mat4& projection) {
    shader.bind();

    // Set MVP matrices
    glUniformMatrix4fv(shader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(shader.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(shader.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Bind the texture
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(shader.getUniformLocation("texture"), 0);

    // Bind the VAO and draw
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
