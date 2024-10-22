#pragma once
#include "texture.h"

#include <framework/shader.h>
#include <framework/opengl_includes.h>
#include <framework/image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <string>
#include <map>


class Cube {
public:
    Cube(const std::vector<std::string>&);
    ~Cube();

    void draw(Shader& shader, const glm::mat4& modelMatrix, const glm::mat3& normalModelMatrix, const glm::mat4& view, const glm::vec3& cameraPos, GLuint);

private:
    std::vector<Image> images;
    GLuint m_cube_vao, m_cube_vbo, m_cube_map_texture;

    void setupCube();
    void loadCubemap();
};