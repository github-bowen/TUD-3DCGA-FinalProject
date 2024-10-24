#include "wall.h"

#include <stdexcept>
#include <array>
#include <iostream>
#include <filesystem>
#include <stb/stb_image.h>

Wall::Wall() {


    setupWall();
}

Wall::~Wall() {
    // Clean up the buffers
    glDeleteVertexArrays(1, &m_quad_vao);
    glDeleteBuffers(1, &m_quad_vbo);
}

unsigned int Wall::loadWallmap(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Wall::setupWall() {
    // Define corner positions of the wall, texture coordinates, and normal vector
    glm::vec3 topLeftCorner(-1.0f, 1.0f, 0.0f);
    glm::vec3 bottomLeftCorner(-1.0f, -1.0f, 0.0f);
    glm::vec3 bottomRightCorner(1.0f, -1.0f, 0.0f);
    glm::vec3 topRightCorner(1.0f, 1.0f, 0.0f);

    glm::vec2 topLeftUV(0.0f, 1.0f);
    glm::vec2 bottomLeftUV(0.0f, 0.0f);
    glm::vec2 bottomRightUV(1.0f, 0.0f);
    glm::vec2 topRightUV(1.0f, 1.0f);

    glm::vec3 wallNormal(0.0f, 0.0f, 1.0f);

    // Tangent and bitangent calculations for the two triangles forming the quad
    auto tbnFirstTriangle = calculateTBN(topLeftCorner, bottomLeftCorner, bottomRightCorner, topLeftUV, bottomLeftUV, bottomRightUV);
    auto tbnSecondTriangle = calculateTBN(topLeftCorner, bottomRightCorner, topRightCorner, topLeftUV, bottomRightUV, topRightUV);

    // Prepare vertex data
    float wallVertices[84]; // 6 vertices * (3 pos + 3 norm + 2 uv + 3 tan + 3 bitan) = 14 attributes
    prepareVertexData(wallVertices, topLeftCorner, bottomLeftCorner, bottomRightCorner, topRightCorner,
        wallNormal, topLeftUV, bottomLeftUV, bottomRightUV, topRightUV,
        tbnFirstTriangle, tbnSecondTriangle);

    // Create VAO and VBO for the wall
    configureBuffers(wallVertices, sizeof(wallVertices));
}

std::tuple<glm::vec3, glm::vec3> Wall::calculateTBN(const glm::vec3& vertexA, const glm::vec3& vertexB, const glm::vec3& vertexC,
    const glm::vec2& uvA, const glm::vec2& uvB, const glm::vec2& uvC) {

    glm::vec3 edge1 = vertexB - vertexA;
    glm::vec3 edge2 = vertexC - vertexA;
    glm::vec2 deltaUV1 = uvB - uvA;
    glm::vec2 deltaUV2 = uvC - uvA;

    float determinant = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
    float invDet = 1.0f / determinant;

    auto calculateComponent = [&](float deltaUVx1, float deltaUVx2, const glm::vec3& edgeComp1, const glm::vec3& edgeComp2) {
        return invDet * (deltaUVx2 * edgeComp1 - deltaUVx1 * edgeComp2);
        };

    glm::vec3 tangent = calculateComponent(deltaUV1.y, deltaUV2.y, edge1, edge2);
    glm::vec3 bitangent = calculateComponent(-deltaUV1.x, deltaUV2.x, edge1, edge2);

    return { tangent, bitangent };
}

void Wall::prepareVertexData(float* vertices, const glm::vec3& topLeft, const glm::vec3& topRight,
    const glm::vec3& bottomLeft, const glm::vec3& bottomRight, const glm::vec3& normal,
    const glm::vec2& uvTopLeft, const glm::vec2& uvTopRight, const glm::vec2& uvBottomLeft, const glm::vec2& uvBottomRight,
    const std::tuple<glm::vec3, glm::vec3>& tbn1, const std::tuple<glm::vec3, glm::vec3>& tbn2) {

    const glm::vec3 positions[] = { topLeft, topRight, bottomLeft, topLeft, bottomLeft, bottomRight };
    const glm::vec2 uvs[] = { uvTopLeft, uvTopRight, uvBottomLeft, uvTopLeft, uvBottomLeft, uvBottomRight };
    const glm::vec3 tangents[] = { std::get<0>(tbn1), std::get<0>(tbn1), std::get<0>(tbn1), std::get<0>(tbn2), std::get<0>(tbn2), std::get<0>(tbn2) };
    const glm::vec3 bitangents[] = { std::get<1>(tbn1), std::get<1>(tbn1), std::get<1>(tbn1), std::get<1>(tbn2), std::get<1>(tbn2), std::get<1>(tbn2) };

    for (int i = 0; i < 6; ++i) {
        const glm::vec3& pos = positions[i];
        const glm::vec2& uv = uvs[i];
        const glm::vec3& tangent = tangents[i];
        const glm::vec3& bitangent = bitangents[i];

        int offset = i * 14; // Each vertex has 14 float attributes (position:3, normal:3, uv:2, tangent:3, bitangent:3)

        // Position
        vertices[offset] = pos.x;
        vertices[offset + 1] = pos.y;
        vertices[offset + 2] = pos.z;
        // Normal (same for all vertices)
        vertices[offset + 3] = normal.x;
        vertices[offset + 4] = normal.y;
        vertices[offset + 5] = normal.z;
        // Texcoords
        vertices[offset + 6] = uv.x;
        vertices[offset + 7] = uv.y;
        // Tangent
        vertices[offset + 8] = tangent.x;
        vertices[offset + 9] = tangent.y;
        vertices[offset + 10] = tangent.z;
        // Bitangent
        vertices[offset + 11] = bitangent.x;
        vertices[offset + 12] = bitangent.y;
        vertices[offset + 13] = bitangent.z;
    }
}

void Wall::configureBuffers(float* vertices, size_t size) {
    glGenVertexArrays(1, &m_quad_vao);
    glGenBuffers(1, &m_quad_vbo);

    glBindVertexArray(m_quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    // Sizes of each attribute (position: 3, normal: 3, texcoord: 2, tangent: 3, bitangent: 3)
    GLint attributeSizes[] = { 3, 3, 2, 3, 3 };
    size_t offset = 0;

    for (GLuint i = 0; i < 5; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attributeSizes[i], GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(offset * sizeof(float)));
        offset += attributeSizes[i];
    }
}

void Wall::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, const glm::vec3& viewPos, const glm::vec3& lightPos) {


    GLint old_depth_mode;
    glGetIntegerv(GL_DEPTH_FUNC, &old_depth_mode);

    glDepthFunc(GL_LEQUAL);


    shader.bind();
    glUniformMatrix4fv(shader.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(shader.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(shader.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(shader.getUniformLocation("viewPos"), 1, glm::value_ptr(viewPos));
    glUniform3fv(shader.getUniformLocation("lightPos"), 1, glm::value_ptr(lightPos));

    //glActiveTexture(textureSlot);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map_texture);
    //glUniform1i(shader.getUniformLocation("sampler_cube"), 0);

    unsigned int diffuseMap = loadWallmap("resources/textures/brickwall.jpg");
    unsigned int normalMap = loadWallmap("resources/textures/brickwall_normal.jpg");


    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, normalMap);


    glUniform1i(shader.getUniformLocation("diffuseMap"), 3);
    glUniform1i(shader.getUniformLocation("normalMap"), 4);


    glBindVertexArray(m_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glDepthFunc(old_depth_mode);
}
