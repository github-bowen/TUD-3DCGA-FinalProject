#ifndef CONFIG_H
#define CONFIG_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TEXTURE_PATH "resources/textures/checkerboard.png"
#define MESH_PATH "resources/meshes/dragon.obj"

namespace config {

	bool show_imgui = true;


	bool isTopView = false;  // switch between 2 cameras

    const glm::mat4 m_projectionMatrix = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 30.0f);

    /* Main Camera View */
    const glm::mat4 m_viewMatrix = glm::lookAt(
        glm::vec3(-1, 1, -1), // camera position
        glm::vec3(0),         // target position
        glm::vec3(0, 1, 0)    // up postion
    );

    const glm::mat4 m_modelMatrix = glm::mat4(1.0f);

    /* Main MVP */
    const glm::mat4 mvpMatrix = m_projectionMatrix * m_viewMatrix * m_modelMatrix;

    const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(m_modelMatrix));

    /* Top Camera View */
    const glm::mat4 topViewMatrix = glm::lookAt(
        glm::vec3(-1, 4, -1),   // camera position 
        glm::vec3(0, 0, 0),   // target postion
        glm::vec3(0, 1, 0)    // up postion
    );

    /* Top MVP */
    const glm::mat4 topViewMvpMatrix = m_projectionMatrix * topViewMatrix * m_modelMatrix;

}

#endif // CONFIG_H