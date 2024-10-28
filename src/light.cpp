#include "light.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

Light::Light(const glm::vec3& pos, const glm::vec3& col, const glm::vec3& dir)
    : position(pos), color(col), direction(dir)
{
    glGenVertexArrays(1, &light_vao);
    glBindVertexArray(light_vao);
    glBindVertexArray(0);
}

Light::~Light() {
    glDeleteVertexArrays(1, &light_vao);
}

void Light::renderLightSource(const Shader& shader, const glm::mat4& mvp) const {
    glm::vec4 screenPos = mvp * glm::vec4(position, 1.0f);

    shader.bind();
    glUniform4fv(shader.getUniformLocation("pos"), 1, glm::value_ptr(screenPos));
    glUniform3fv(shader.getUniformLocation("color"), 1, glm::value_ptr(color));

    glPointSize(20.0f);
    glBindVertexArray(light_vao);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
}