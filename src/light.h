//#pragma once
//#include <glm/vec3.hpp>
//
//
//struct Light {
//    glm::vec3 position;
//    glm::vec3 color;
//    glm::vec3 direction;
//};

#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <framework/shader.h>

struct Light {
    glm::vec3 position;
    glm::vec3 color;
    //glm::vec3 direction;
    GLuint light_vao;

    // Constructor
    Light(const glm::vec3& pos, const glm::vec3& col);

    // Render function to visualize the light source
    void renderLightSource(const Shader& shader, const glm::mat4& mvp) const;

    // Destructor to delete VAO
    ~Light();
};