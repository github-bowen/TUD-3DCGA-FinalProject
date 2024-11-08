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
    Minimap(float width, float height, const std::string& texturePath);
    ~Minimap();

    void draw(Shader& shader, const glm::mat4& modelMatrix, const glm::mat4& view, const glm::mat4& projection);

private:
    GLuint m_texture;
    GLuint m_vao;
    GLuint m_vbo;
    float m_width;
    float m_height;

    void loadTexture(const std::string& texturePath);
    void setupMinimap();
};

#endif // MINIMAP_H
