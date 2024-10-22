#ifndef CONFIG_H
#define CONFIG_H

#include <framework/opengl_includes.h>

#include <array>
#include <map>

#define TEXTURE_PATH "resources/textures/checkerboard.png"
#define MESH_PATH "resources/meshes/dragon.obj"

namespace config {

    // ----------------------------------- GUI ------------------------------------

	bool show_imgui = true;

    // ----------------------------------- VIEW ------------------------------------

    
    int activeCameraIndex = 0;  // switch between 2 cameras

    const glm::mat4 m_projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

    const glm::mat4 m_modelMatrix = glm::mat4(1.0f);

    //const glm::mat4 mvpMatrix = m_projectionMatrix * cameras[activeCameraIndex].viewMatrix() * m_modelMatrix;

    const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(m_modelMatrix));

    // ----------------------------------- SCENE ------------------------------------

    const std::vector<std::string> scene_paths = {
        // NOTE: Make sure the order unchanged
        "resources/scene/front.jpg",
        "resources/scene/back.jpg",
        "resources/scene/top.jpg",
        "resources/scene/bottom.jpg",
        "resources/scene/right.jpg",
        "resources/scene/left.jpg",

        
    };

    // ----------------------------------- TEXTURE ------------------------------------

    const std::map<std::string, GLuint> textureSlots = {
        {"mesh", GL_TEXTURE0},
        {"scene", GL_TEXTURE1},
        {"cube", GL_TEXTURE2}
    };
}

#endif // CONFIG_H