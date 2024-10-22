// Scene.cpp
#include "scene.h"

#include <stdexcept>
#include <array>
#include <iostream>
#include <filesystem>


#define GET_TEXTURE_FORMAT(channels) ((channels) == 4 ? GL_RGBA : GL_RGB)
#define GET_TEXTURE_INTERNAL_FORMAT(channels) ((channels) == 4 ? GL_RGBA8 : GL_RGB8)


// load scene images, initialize vao vbo
Scene::Scene(const std::vector<std::string>& scene_paths) {

    for (const auto& scene_path : scene_paths) {
        const std::filesystem::path path{ scene_path };
        images.emplace_back(path);
    }

    // load cube texture
    loadCubemap();

    // Set up the VAO and VBO of the cube
    setupScene();
}

Scene::~Scene() {
    glDeleteVertexArrays(1, &m_scene_vao);
    glDeleteBuffers(1, &m_scene_vbo);
    // TODO
}

void Scene::loadCubemap() {

    glGenTextures(1, &m_scene_cube_map_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_scene_cube_map_texture);

    // 对正面 (Right face)
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

    // 对背面 (Left face)
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

    // 对顶部 (Top face)
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

    // 对底部 (Bottom face)
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

    // 对前面 (Front face)
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

    // 对后面 (Back face)
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

// Set up the cube's vertex data
void Scene::setupScene() {
    // Set up VAO and VBO for positions
    glGenVertexArrays(1, &m_scene_vao);
    glGenBuffers(1, &m_scene_vbo);
    glBindVertexArray(m_scene_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_scene_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

// Draw the scene cube
void Scene::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, GLuint textureSlot) {
    GLint old_depth_mode;
    glGetIntegerv(GL_DEPTH_FUNC, &old_depth_mode);

    glDepthFunc(GL_LEQUAL);


    shader.bind();
    glUniformMatrix4fv(shader.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(shader.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));

    // Bind the texture unit and set the sampler uniform
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_scene_cube_map_texture);
    glUniform1i(shader.getUniformLocation("sampler_cube"), 0);

    
    glBindVertexArray(m_scene_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(old_depth_mode);
}
