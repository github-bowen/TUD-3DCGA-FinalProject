#ifndef MINIMAP_H
#define MINIMAP_H

#include <framework/shader.h>
#include <framework/opengl_includes.h>
#include <framework/image.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <string>


class Minimap {
public:
    // Constructor that accepts width, height, and texture size for the minimap
    Minimap(float width, float height, int textureWidth, int textureHeight);

    // Destructor to clean up OpenGL resources
    ~Minimap();

    // Method to create a texture and framebuffer for the minimap
    void createMinimapTexture(int width, int height);

    // Method to set up vertex data for rendering the minimap
    void setupMinimap();

    // Method to set up the high-altitude camera for minimap view
    void setupHighAltitudeCamera(glm::mat4& view, glm::mat4& projection);

    // Method to render the scene to the minimap texture
    // void renderSceneToMinimapTexture(const Shader& shader, const glm::mat4& view, const glm::mat4& projection);

    // Method to draw the minimap to the screen
    void draw(Shader& shader, const glm::mat4& modelMatrix, const glm::mat4& view, const glm::mat4& projection);

    GLuint getFramebuffer() const { return m_framebuffer; }

private:
    GLuint m_texture;          // The texture to store the minimap
    GLuint m_framebuffer;      // The framebuffer used to render the minimap texture
    GLuint m_vao, m_vbo;       // VAO and VBO for rendering the minimap quad
    float m_width, m_height;   // Dimensions of the minimap quad
    int m_textureWidth, m_textureHeight;  // Dimensions of the minimap texture
};

#endif // MINIMAP_H
