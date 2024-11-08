#pragma once

#include <framework/disable_all_warnings.h>
#include <framework/mesh.h>
#include <framework/shader.h>
DISABLE_WARNINGS_PUSH()
#include <glm/vec3.hpp>
DISABLE_WARNINGS_POP()

#include <exception>
#include <filesystem>
#include <framework/opengl_includes.h>
#include "texture.h"

struct MeshLoadingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

// Alignment directives are to comply with std140 alignment requirements (https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout)
struct GPUMaterial {
    GPUMaterial() = default;
    GPUMaterial(const Material& material);

    alignas(16) glm::vec3 kd{ 1.0f };
	alignas(16) glm::vec3 ks{ 0.0f };
    alignas(16) glm::vec3 albedo{ 1.0f };
    float roughness{ 1.0f };
    float metallic{ 0.0f };
    float ao{ 0.3f };
	float shininess{ 1.0f };
	float transparency{ 1.0f };

    GLuint ubo = 0;

    // 初始化 UBO
    void initUBO() {
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(GPUMaterial), this, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    // 更新材质数据到 UBO
    void updateUBO() {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPUMaterial), this);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    //void initMaps(const std::string& albedoPath) {
    //    albedoMap = new Texture(albedoPath);
    //}
};

class GPUMesh {
public:
    GPUMesh(const Mesh& cpuMesh);
    // Cannot copy a GPU mesh because it would require reference counting of GPU resources.
    GPUMesh(const GPUMesh&) = delete;
    GPUMesh(GPUMesh&&);
    Texture* normalMap = nullptr;
    Texture* albedoMap = nullptr;
    Texture* roughnessMap = nullptr;
    Texture* metallicMap = nullptr;
    Texture* aoMap = nullptr;
    ~GPUMesh();

    // Generate a number of GPU meshes from a particular model file.
    // Multiple meshes may be generated if there are multiple sub-meshes in the file
    static std::vector<GPUMesh> loadMeshGPU(std::filesystem::path filePath, bool normalize = false);

    // Cannot copy a GPU mesh because it would require reference counting of GPU resources.
    GPUMesh& operator=(const GPUMesh&) = delete;
    GPUMesh& operator=(GPUMesh&&);

    bool hasTextureCoords() const;

    // Bind VAO and call glDrawElements.
    void draw(const Shader& drawingShader);

    GPUMaterial material;

private:
    void moveInto(GPUMesh&&);
    void freeGpuMemory();

private:
    static constexpr GLuint INVALID = 0xFFFFFFFF;

    GLsizei m_numIndices { 0 };
    bool m_hasTextureCoords { false };
    GLuint m_ibo { INVALID };
    GLuint m_vbo { INVALID };
    GLuint m_vao { INVALID };
    GLuint m_uboMaterial { INVALID };
};
