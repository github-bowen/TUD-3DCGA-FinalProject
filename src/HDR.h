#pragma once
#include <glm/glm.hpp>
#include <string>
#include "framework/shader.h"
#include "texture.h"

class HDR {
public:
    HDR();
    ~HDR();

    Texture* equiMap = nullptr;
    void renderCube();
    void equiToCube(Shader& shader);
    void cubeToConv(Shader& shader);
    void renderScene(Shader& shader, const glm::mat4& projection, const glm::mat4& view);
    void initBuffer();
    GLuint envCubemap;
    GLuint irradianceMap;

private:
    GLuint captureFBO;
    GLuint captureRBO;
    GLuint cubeVAO, cubeVBO;
    glm::mat4 captureProjection;
    glm::mat4 captureViews[6];
};