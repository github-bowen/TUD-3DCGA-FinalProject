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

class Scene {
public:
    Scene(const std::vector<std::string>&);
    ~Scene();
    void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view);

private:

    std::vector<Image> images;
    GLuint m_scene_vao, m_scene_vbo, m_scene_cube_map_texture;
    const GLfloat vertices[6 * 6 * 3] = {
        

        // Right face
        1.0f, -1.0f,  1.0f,  // Right top front
        1.0f,  1.0f,  1.0f,  // Right bottom front
        1.0f,  1.0f, -1.0f,  // Right bottom back
        1.0f, -1.0f, -1.0f,  // Right top back
        1.0f, -1.0f,  1.0f,  // Right top front (repeat)
        1.0f,  1.0f, -1.0f,  // Right bottom back (repeat)

        // Left face
        -1.0f, -1.0f, -1.0f, // Left top back
        -1.0f,  1.0f, -1.0f, // Left bottom back
        -1.0f,  1.0f,  1.0f, // Left bottom front
        -1.0f, -1.0f,  1.0f, // Left top front
        -1.0f, -1.0f, -1.0f, // Left top back (repeat)
        -1.0f,  1.0f,  1.0f, // Left bottom front (repeat)

        // Top face
        -1.0f,  1.0f, -1.0f, // Left bottom back
         1.0f,  1.0f, -1.0f, // Right bottom back
         1.0f,  1.0f,  1.0f, // Right bottom front
        -1.0f,  1.0f,  1.0f, // Left bottom front
        -1.0f,  1.0f, -1.0f, // Left bottom back (repeat)
         1.0f,  1.0f,  1.0f, // Right bottom front (repeat)

         // Bottom face
         -1.0f, -1.0f,  1.0f, // Left top front
          1.0f, -1.0f,  1.0f, // Right top front
          1.0f, -1.0f, -1.0f, // Right top back
         -1.0f, -1.0f, -1.0f, // Left top back
         -1.0f, -1.0f,  1.0f, // Left top front (repeat)
          1.0f, -1.0f, -1.0f, // Right top back (repeat)

        

          // Front face
          -1.0f, -1.0f,  1.0f, // Left top front
           1.0f, -1.0f,  1.0f, // Right top front
           1.0f,  1.0f,  1.0f, // Right bottom front
          -1.0f,  1.0f,  1.0f, // Left bottom front
          -1.0f, -1.0f,  1.0f, // Left top front (repeat)
           1.0f,  1.0f,  1.0f, // Right bottom front (repeat)

           // Back face
           -1.0f, -1.0f, -1.0f, // Left top back
            1.0f, -1.0f, -1.0f, // Right top back
            1.0f,  1.0f, -1.0f, // Right bottom back
           -1.0f,  1.0f, -1.0f, // Left bottom back
           -1.0f, -1.0f, -1.0f, // Left top back (repeat)
            1.0f,  1.0f, -1.0f  // Right bottom back (repeat)
    };



    //std::vector<Texture> textures;

    void setupScene();
    void loadCubemap();
};

