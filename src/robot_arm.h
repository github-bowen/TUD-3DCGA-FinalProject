#pragma once
// Suppress warnings in third-party code.
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <vector>
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


class ArmSegment {
public:
    
    
    ArmSegment();

    ArmSegment(float angle, const glm::vec3& size);

    ~ArmSegment();

    glm::mat4 rotationMatrix(float angle, const glm::vec3& axis);
    glm::mat4 translationMatrix(const glm::vec3& translation);
    glm::mat4 scaleMatrix(const glm::vec3& scale);
    std::vector<glm::mat4> computeTransformMatrix(const std::vector<ArmSegment>& armSegments);
    void draw(Shader& shader, const glm::mat4& modelMatrix, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transform);
    void animate(float deltaTime);

    glm::vec3 boxSize; // Dimensions of the arm segment
    float rotationX { 0.0f };   // Rotation angle around X-axis

private:

    GLuint m_robot_vao, m_robot_vbo;
    void setupArmSegment();  

    float vertices[6 * 6 * 6] = {
        // positions          // normals
        -0.5f, -0.5f, -0.0f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.0f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.0f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.0f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  1.0f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  1.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  1.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  1.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.0f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.0f,  0.0f,  1.0f,  0.0f
    };
    
};
