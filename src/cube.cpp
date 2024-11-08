#include "cube.h"

#include <stdexcept>
#include <array>
#include <iostream>
#include <filesystem>


#define GET_TEXTURE_FORMAT(channels) ((channels) == 4 ? GL_RGBA : GL_RGB)
#define GET_TEXTURE_INTERNAL_FORMAT(channels) ((channels) == 4 ? GL_RGBA8 : GL_RGB8)


Cube::Cube(const std::vector<std::string>& scene_paths) {

    for (const auto& scene_path : scene_paths) {
        const std::filesystem::path path{ scene_path };
        images.emplace_back(Image(path));
    }
    assert(images.size() == 6);
    
    // load cube texture
    loadCubemap();

    setupCube();
}



Cube::~Cube() {
    // Clean up the buffers
    glDeleteTextures(1, &m_cube_map_texture);
    glDeleteVertexArrays(1, &m_cube_vao);
    glDeleteBuffers(1, &m_cube_vbo);
}

void Cube::loadCubemap() {

    glGenTextures(1, &m_cube_map_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map_texture);

    // (Right face)
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        0,  // mipmap level
        GL_RGB,  // internal format
        images[0].width, images[0].height,
        0,  // border, must be 0
        GL_RGB,  // format
        GL_UNSIGNED_BYTE,  // data type
        images[0].get_data()
    );

    // (Left face)
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        0,  // mipmap level
        GL_RGB,  // internal format
        images[1].width, images[1].height,
        0,  // border, must be 0
        GL_RGB,  // format
        GL_UNSIGNED_BYTE,  // data type
        images[1].get_data()
    );

    // (Top face)
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        0,  // mipmap level
        GL_RGB,  // internal format
        images[2].width, images[2].height,
        0,  // border, must be 0
        GL_RGB,  // format
        GL_UNSIGNED_BYTE,  // data type
        images[2].get_data()
    );

    // (Bottom face)
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        0,  // mipmap level
        GL_RGB,  // internal format
        images[3].width, images[3].height,
        0,  // border, must be 0
        GL_RGB,  // format
        GL_UNSIGNED_BYTE,  // data type
        images[3].get_data()
    );

    // (Front face)
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        0,  // mipmap level
        GL_RGB,  // internal format
        images[4].width, images[4].height,
        0,  // border, must be 0
        GL_RGB,  // format
        GL_UNSIGNED_BYTE,  // data type
        images[4].get_data()
    );

    // (Back face)
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
        0,  // mipmap level
        GL_RGB,  // internal format
        images[5].width, images[5].height,
        0,  // border, must be 0
        GL_RGB,  // format
        GL_UNSIGNED_BYTE,  // data type
        images[5].get_data()
    );



    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

// A key reference for learning and implementing environment mapping is the article by Joey de Vries on his website LearnOpenGL: 
// https://learnopengl.com/Advanced-OpenGL/Cubemaps


void Cube::setupCube() {
    // Define the vertices for a cube
    float vertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };


    // Generate and bind the Vertex Array Object
    glGenVertexArrays(1, &m_cube_vao);
    glBindVertexArray(m_cube_vao);

    // Generate and bind the Vertex Buffer Object
    glGenBuffers(1, &m_cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set vertex attributes (position and normal)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Position attribute

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // Normal attribute

    // Unbind the VAO
    glBindVertexArray(0);
}

void Cube::draw(Shader& shader, const glm::mat4& modelMatrix, const glm::mat3& normalModelMatrix, const glm::mat4& view, 
    const glm::mat4& projection, const glm::vec3& cameraPos, GLuint textureSlot) {


    GLint old_depth_mode;
    glGetIntegerv(GL_DEPTH_FUNC, &old_depth_mode);

    glDepthFunc(GL_LEQUAL);


    shader.bind();
    glUniformMatrix4fv(shader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(shader.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(shader.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix3fv(shader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));
    glUniform3fv(shader.getUniformLocation("cameraPos"), 1, glm::value_ptr(cameraPos));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map_texture);
    if(textureSlot == GL_TEXTURE1)  glUniform1i(shader.getUniformLocation("sampler_cube"), 1);
    else if(textureSlot == GL_TEXTURE11)    glUniform1i(shader.getUniformLocation("sampler_cube"), 11);

    // Bind the VAO
    glBindVertexArray(m_cube_vao);

    // Draw the cube
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Unbind the VAO
    glBindVertexArray(0);

    glDepthFunc(old_depth_mode);
}
