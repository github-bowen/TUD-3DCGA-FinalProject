#include "robot_arm.h"
// Suppress warnings in third-party code.
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <list>
#include <stdexcept>
#include <array>
#include <iostream>
#include <filesystem>


ArmSegment::ArmSegment() : rotationX(0.0f), boxSize(1.0f, 1.0f, 1.0f) {
    setupArmSegment();
}

ArmSegment::ArmSegment(float angle, const glm::vec3& size)
    : rotationX(angle), boxSize(size) {
    setupArmSegment();
}


ArmSegment::~ArmSegment() {
    // Clean up the buffers
    glDeleteVertexArrays(1, &m_robot_vao);
    glDeleteBuffers(1, &m_robot_vbo);
}

glm::mat4 ArmSegment::rotationMatrix(float angle, const glm::vec3& axis)
{
    return glm::rotate(glm::identity<glm::mat4>(), angle, axis);
}

glm::mat4 ArmSegment::translationMatrix(const glm::vec3& translation)
{
    return glm::translate(glm::identity<glm::mat4>(), translation);
}

glm::mat4 ArmSegment::scaleMatrix(const glm::vec3& scale)
{
    return glm::scale(glm::identity<glm::mat4>(), scale);
}

std::vector<glm::mat4> ArmSegment::computeTransformMatrix(const std::vector<ArmSegment>& armSegments)
{
    // Create a vector to hold the transformation matrices for each arm segment
    std::vector<glm::mat4> transforms;

    // Create a list to store the arm segments' pointers for iterative transformation
    std::list<const ArmSegment*> seglist;

    // Define the axis of rotation (assuming rotation is around the x-axis)
    glm::vec3 axis{ 1, 0, 0 };

    // Initialize the rotation matrix as identity
    glm::mat4 rotate = glm::identity<glm::mat4>();


    for (const ArmSegment& armSegment : armSegments) {
        glm::mat4 matrix = glm::identity<glm::mat4>();

        glm::mat4 scale = scaleMatrix(armSegment.boxSize);

        rotate = rotationMatrix(armSegment.rotationX, axis) * rotate;

        glm::mat4 translationall = glm::identity<glm::mat4>();
        float translateangle = 0.0;

        // Iterate over previously processed arm segments to accumulate translation values
        for (const ArmSegment* segment : seglist) {
            // Accumulate the rotation angles of previous segments
            translateangle += segment->rotationX;

            // Apply the translation based on the accumulated rotation
            translationall = translationMatrix({ 0,segment->boxSize.z * std::sin(-translateangle),segment->boxSize.z * std::cos(translateangle) }) * translationall;
        }

        seglist.push_back(&armSegment);

        matrix = translationall * rotate * scale;

        // Add the resulting transformation matrix to the list of transforms
        transforms.push_back(matrix);
    }

    return transforms;
}




void ArmSegment::setupArmSegment() {
    // Define the vertices for a cube
    


    // Generate and bind the Vertex Array Object
    glGenVertexArrays(1, &m_robot_vao);
    glBindVertexArray(m_robot_vao);

    // Generate and bind the Vertex Buffer Object
    glGenBuffers(1, &m_robot_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_robot_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set vertex attributes (position and normal)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Position attribute

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // Normal attribute

    // Unbind the VAO
    glBindVertexArray(0);
}

void ArmSegment::draw(Shader& shader, const glm::mat4& modelMatrix, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transform) {

    GLint old_depth_mode;
    glGetIntegerv(GL_DEPTH_FUNC, &old_depth_mode);

    glDepthFunc(GL_LEQUAL);


    shader.bind();
    glUniformMatrix4fv(shader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(shader.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(shader.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(shader.getUniformLocation("transform"), 1, GL_FALSE, glm::value_ptr(transform));

    glBindVertexArray(m_robot_vao);

    // Draw the cube
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Unbind the VAO
    glBindVertexArray(0);

    glDepthFunc(old_depth_mode);

}


void ArmSegment::animate(float deltaTime) {
    static float rotationSpeed = 10.0f;  
    static bool rotationDirection = true;  

    
    if (rotationDirection) {
        rotationX += glm::radians(rotationSpeed) * deltaTime;
        
        if (rotationX >= glm::radians(90.0f)) {
            rotationX = glm::radians(90.0f);  
            rotationDirection = false;  
        }
    }
    else {
        rotationX -= glm::radians(rotationSpeed) * deltaTime;
        
        if (rotationX <= glm::radians(-90.0f)) {
            rotationX = glm::radians(-90.0f); 
            rotationDirection = true;  
        }
    }
}