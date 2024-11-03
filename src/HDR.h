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
    void initBuffer();
    void renderCube();
    void renderQuad();
    void equiToCube(Shader& shader);
    void cubeToConv(Shader& shader);
    void cubeToPre(Shader& shader);
    void brdfLUT(Shader& shader);
    void renderScene(Shader& shader, const glm::mat4& projection, const glm::mat4& view);

    GLuint envCubemap;
    GLuint irradianceMap;
    GLuint prefilterMap;
    GLuint brdfTex;

private:
    GLuint captureFBO;
    GLuint captureRBO;
    GLuint cubeVAO, cubeVBO;
    GLuint quadVAO, quadVBO;
    glm::mat4 captureProjection;
    glm::mat4 captureViews[6];
};