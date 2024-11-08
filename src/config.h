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
        {"cube", GL_TEXTURE2},
        //......
		{"minimap", GL_TEXTURE5}
    };

	// ----------------------------------- MINIMAP ------------------------------------

    const glm::vec3 minimapCameraPosition = glm::vec3(0.0f, 20.0f, 0.0f);  // 高处位置
    const glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);           // 目标位置
    const glm::vec3 upDirection = glm::vec3(0.0f, 0.0f, -1.0f);             // 向下的 up 方向
    const glm::mat4 minimapView = glm::lookAt(minimapCameraPosition, targetPosition, upDirection);

    const float size = 10.0f;  // display range
    const glm::mat4 minimapProjection = glm::ortho(-size, size, -size, size, 1.0f, 100.0f);

    const float minimapWidth = 200.0f;
    const float minimapHeight = 200.0f;

    const std::string minimapTexturePath = "resources/scene/bottom.jpg";  // bottom

}

#endif // CONFIG_H