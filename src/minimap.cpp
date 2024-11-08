#include "minimap.h"
#include <stdexcept>
#include <iostream>

// Minimap constructor with specified width, height, and texture size
Minimap::Minimap(float width, float height, int textureWidth, int textureHeight)
    : m_width(width), m_height(height), m_textureWidth(textureWidth), m_textureHeight(textureHeight) {
    // Create framebuffer and texture for the minimap
    createMinimapTexture(textureWidth, textureHeight);

    // Set up the vertex data for the minimap
    setupMinimap();
}

// Minimap destructor
Minimap::~Minimap() {
    // Delete the texture, framebuffer, VAO, and VBO
    glDeleteTextures(1, &m_texture);
    glDeleteFramebuffers(1, &m_framebuffer);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

// Create the framebuffer and texture for the minimap
void Minimap::createMinimapTexture(int width, int height) {
    // Create framebuffer
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // Create texture to store the rendered result
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Set texture parameters
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Minimap framebuffer is not complete!" << std::endl;
    }

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Set up the minimap's vertex data
void Minimap::setupMinimap() {
    const float vertices[] = {
        // Positions           // Texture Coordinates
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

// Set up the camera for a high-altitude top-down view for minimap
void Minimap::setupHighAltitudeCamera(glm::mat4& view, glm::mat4& projection) {
    // Define camera settings for high-altitude top-down view
    glm::vec3 cameraPosition(0.0f, 100.0f, 0.0f); // High up position
    glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);    // Look at the center
    glm::vec3 up(0.0f, 0.0f, 1.0f);               // Up direction

    // Set up the view and projection matrices
    view = glm::lookAt(cameraPosition, cameraTarget, up);
    projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, -1.0f, 1.0f);
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
    glUniform1i(shader.getUniformLocation("m_texture"), 0);

    // Bind the VAO and draw
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Minimap::drawQuadWithTexture(Shader& shader) {
    // Assuming you have a quad with VAO/VBO set up for 2D rendering
    glBindTexture(GL_TEXTURE_2D, m_texture); // Bind the minimap texture

    // Set up shader for 2D drawing
    shader.bind();
    glUniform1i(shader.getUniformLocation("m_texture"), 0); // Set texture uniform

    // Render the quad (with appropriate transformations)
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

