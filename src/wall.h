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


class Wall {
public:
    Wall();
    ~Wall();

    void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, const glm::vec3& viewPos, const glm::vec3& lightPos);

private:
    //std::vector<Image> images;
    GLuint m_quad_vao, m_quad_vbo;

    void setupWall();
    unsigned int loadWallmap(char const* path);
};