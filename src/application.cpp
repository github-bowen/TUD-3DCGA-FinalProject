//#include "Image.h"
#include "mesh.h"
#include "texture.h"
// Always include window first (because it includes glfw, which includes GL which needs to be included AFTER glew).
// Can't wait for modules to fix this stuff...
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glad/glad.h>
// Include glad before glfw3
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <imgui/imgui.h>
DISABLE_WARNINGS_POP()
#include <framework/shader.h>
#include <framework/window.h>
#include <functional>
#include <iostream>
#include <vector>

#include "config.h"
#include "scene.h"
#include "camera.h"
#include "cube.h"
#include "wall.h"
#include "light.h"

std::vector<Light> lights{};
size_t selectedLightIndex = 0;

bool albedoTex = false;
bool roughnessTex = false;
bool metallicTex = false;
bool aoTex = false;
bool normalMapping = false;

class Application {
public:
    Application()
        : m_window("Final Project", glm::ivec2(1024, 1024), OpenGLVersion::GL41)
        , m_texture(RESOURCE_ROOT TEXTURE_PATH), 
        m_cameras {
            Camera { &m_window, glm::vec3(1.2f, 1.1f, 0.9f), -glm::vec3(1.2f, 1.1f, 0.9f) }, // Main camera
            Camera { &m_window, glm::vec3(-1, 10, -1), -glm::vec3(-1, 10, -1) }          // New camera
        }
    {
        lights.push_back(Light(glm::vec3(0.5f, 1.0f, 0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        this->__init_callback();
        this->__init_meshes();
        this->__init_shader();
    }

    void imgui() {

        if (!config::show_imgui)
            return;

        ImGui::Begin("Assignment 2");
        // ImGui::InputInt("This is an integer input", &dummyInteger); // Use ImGui::DragInt or ImGui::DragFloat for larger range of numbers.
        // ImGui::Text("Value is: %i", dummyInteger); // Use C printf formatting rules (%i is a signed integer)
        ImGui::Checkbox("Use material if no texture", &m_useMaterial);

        const char* cameraNames[] = { "Camera 1", "Camera 2" };

        ImGui::Text("Select Camera");
        ImGui::ListBox("##cameraList", &config::activeCameraIndex, cameraNames, IM_ARRAYSIZE(cameraNames));
        //std::cout << "current camera index: " << config::activeCameraIndex << std::endl;

        ImGui::Separator();
        ImGui::Text("Lights");

        std::vector<std::string> itemStrings = {};
        for (size_t i = 0; i < lights.size(); i++) {
            auto string = "Light " + std::to_string(i);
            itemStrings.push_back(string);
        }
        std::vector<const char*> itemCStrings = {};
        for (const auto& string : itemStrings) {
            itemCStrings.push_back(string.c_str());
        }
        int tempSelectedItem = static_cast<int>(selectedLightIndex);
        if (ImGui::ListBox("Lights", &tempSelectedItem, itemCStrings.data(), (int)itemCStrings.size(), 4)) {
            selectedLightIndex = static_cast<size_t>(tempSelectedItem);
        }
        ImGui::DragFloat3("LightPos", glm::value_ptr(lights[selectedLightIndex].position), 0.01f, -10.0, 10.0, "%.2f");

        ImGui::Separator();
        ImGui::Text("Material Properties");

        // 假设第一个 `GPUMesh` 的材质作为示例
        GPUMaterial& material = m_meshes[0].material;

        // 使用 ImGui 控件来修改材质属性
        ImGui::Checkbox("Normal Mapping", &normalMapping);
        ImGui::Checkbox("Albedo Texture", &albedoTex);
        ImGui::ColorEdit3("Albedo", glm::value_ptr(material.albedo));
        ImGui::Checkbox("Roughness Texture", &roughnessTex);
        ImGui::SliderFloat("Roughness", &material.roughness, 0.0f, 1.0f);
        ImGui::Checkbox("Metallic Texture", &metallicTex);
        ImGui::SliderFloat("Metallic", &material.metallic, 0.0f, 1.0f);
        ImGui::Checkbox("Ao Texture", &aoTex);
        ImGui::SliderFloat("AO", &material.ao, 0.0f, 1.0f);
        material.updateUBO();

        ImGui::End();
    }

    void __init_callback() {
        m_window.registerKeyCallback([this](int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS)
                onKeyPressed(key, mods);
            else if (action == GLFW_RELEASE)
                onKeyReleased(key, mods);
            });
        m_window.registerMouseMoveCallback(std::bind(&Application::onMouseMove, this, std::placeholders::_1));
        m_window.registerMouseButtonCallback([this](int button, int action, int mods) {
            if (action == GLFW_PRESS)
                onMouseClicked(button, mods);
            else if (action == GLFW_RELEASE)
                onMouseReleased(button, mods);
            });
    }

    void __init_meshes() {
        m_meshes = GPUMesh::loadMeshGPU(RESOURCE_ROOT MESH_PATH);
    }

    void __init_shader() {
        try {
            ShaderBuilder defaultBuilder;
            defaultBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shader_vert.glsl");
            defaultBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "shaders/shader_frag.glsl");
            m_defaultShader = defaultBuilder.build();

            ShaderBuilder shadowBuilder;
            shadowBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shadow_vert.glsl");
            shadowBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/shadow_frag.glsl");
            m_shadowShader = shadowBuilder.build();

            // Any new shaders can be added below in similar fashion.
            // ==> Don't forget to reconfigure CMake when you do!
            //     Visual Studio: PROJECT => Generate Cache for ComputerGraphics
            //     VS Code: ctrl + shift + p => CMake: Configure => enter
            // ....
            ShaderBuilder lightBuilder;
            lightBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/light_vertex.glsl");
            lightBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/light_frag.glsl");
            m_lightShader = lightBuilder.build();
            ShaderBuilder sceneBuilder;
            sceneBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/scene_vert.glsl");
            sceneBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/scene_frag.glsl");
            m_sceneShader = sceneBuilder.build();
            ShaderBuilder cubeBuilder;
            cubeBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/cubemap_vert.glsl");
            cubeBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/cubemap_frag.glsl");
            m_cubeShader = cubeBuilder.build();
            ShaderBuilder wallBuilder;
            wallBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/normalmap_vert.glsl");
            wallBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/normalmap_frag.glsl");
            m_wallShader = wallBuilder.build();
            ShaderBuilder pbrBuilder;
            pbrBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shader_vert.glsl");
            pbrBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/pbr_frag.glsl");
            m_pbrShader = pbrBuilder.build();

        } catch (ShaderLoadingException e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void update()
    {
        int dummyInteger = 0; // Initialized to 0
        while (!m_window.shouldClose()) {
            // This is your game loop
            // Put your real-time logic and rendering in here
            m_window.updateInput();
            m_cameras[config::activeCameraIndex].updateInput();

            imgui();

            // Clear the screen
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // ...
            glEnable(GL_DEPTH_TEST);

            const glm::mat4 view = m_cameras[config::activeCameraIndex].viewMatrix();
            const glm::mat4 mvpMatrix = config::m_projectionMatrix * view * config::m_modelMatrix;
            // Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
            // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
            const glm::mat3 normalModelMatrix = config::normalModelMatrix;

            // render scene: remove translation from the view matrix
            glm::mat4 sceneView = glm::mat4(glm::mat3(view));
            m_scene.draw(m_sceneShader, config::m_projectionMatrix, sceneView, config::textureSlots.at("scene"));

            Camera& currentCamera = m_cameras[config::activeCameraIndex];
            glm::vec3 cameraPos = currentCamera.cameraPos();
            m_cube.draw(m_cubeShader, config::m_modelMatrix, config::normalModelMatrix, view, 
                config::m_projectionMatrix, cameraPos, config::textureSlots.at("cube"));

            //lights.push_back(Light(glm::vec3(0.5f, 1.0f, 0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            for (const Light& light : lights) {
                light.renderLightSource(m_lightShader, mvpMatrix);
            }

            // Assuming you want to rotate around the x-axis by 90 degrees
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the x-axis
            model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::translate(model, glm::vec3(0.0, 0.0, -2.0));
            m_wall.draw(m_wallShader, config::m_projectionMatrix, view, model, cameraPos, lights[selectedLightIndex].position);

            m_pbrShader.bind();
            glUniform3fv(m_pbrShader.getUniformLocation("cameraPos"), 1, glm::value_ptr(cameraPos));

            int numLights = static_cast<int>(lights.size());
            glUniform1i(m_pbrShader.getUniformLocation("numLights"), numLights);
            for (size_t i = 0; i < lights.size(); i++) {
                std::string lightPosName = "lights[" + std::to_string(i) + "].position";
                std::string lightColorName = "lights[" + std::to_string(i) + "].color";
                std::string lightDirName = "lights[" + std::to_string(i) + "].direction";
                glUniform3fv(m_pbrShader.getUniformLocation(lightPosName.c_str()), 1, glm::value_ptr(lights[i].position));
                glUniform3fv(m_pbrShader.getUniformLocation(lightColorName.c_str()), 1, glm::value_ptr(lights[i].color));
                glUniform3fv(m_pbrShader.getUniformLocation(lightDirName.c_str()), 1, glm::value_ptr(lights[i].direction));
            }
            for (GPUMesh& mesh : m_meshes) {
                glUniformMatrix4fv(m_pbrShader.getUniformLocation("mvpMatrix"), 1, GL_FALSE, glm::value_ptr(mvpMatrix));
                //Uncomment this line when you use the modelMatrix (or fragmentPosition)
                //glUniformMatrix4fv(m_defaultShader.getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
                glUniformMatrix3fv(m_pbrShader.getUniformLocation("normalModelMatrix"), 1, GL_FALSE, glm::value_ptr(normalModelMatrix));
                if (mesh.hasTextureCoords()) {
                    m_texture.bind(config::textureSlots.at("mesh"));
                    glUniform1i(m_pbrShader.getUniformLocation("colorMap"), 0);
                    glUniform1i(m_pbrShader.getUniformLocation("hasTexCoords"), GL_TRUE);
                    glUniform1i(m_pbrShader.getUniformLocation("useMaterial"), GL_FALSE);
                } else {
                    glUniform1i(m_pbrShader.getUniformLocation("hasTexCoords"), GL_FALSE);
                    glUniform1i(m_pbrShader.getUniformLocation("useMaterial"), m_useMaterial);
                    
                    glUniform1i(m_pbrShader.getUniformLocation("normalMapping"), normalMapping);
                    glUniform1i(m_pbrShader.getUniformLocation("albedoTex"), albedoTex);
                    glUniform1i(m_pbrShader.getUniformLocation("roughnessTex"), roughnessTex);
                    glUniform1i(m_pbrShader.getUniformLocation("metallicTex"), metallicTex);
                    glUniform1i(m_pbrShader.getUniformLocation("aoTex"), aoTex);
                }
                mesh.draw(m_pbrShader);
            }

            // Processes input and swaps the window buffer
            m_window.swapBuffers();
        }

    }

    // In here you can handle key presses
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyPressed(int key, int mods)
    {
        //std::cout << "Key pressed: " << key << std::endl;
    }

    // In here you can handle key releases
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyReleased(int key, int mods)
    {
        //std::cout << "Key released: " << key << std::endl;
    }

    // If the mouse is moved this function will be called with the x, y screen-coordinates of the mouse
    void onMouseMove(const glm::dvec2& cursorPos)
    {
        //std::cout << "Mouse at position: " << cursorPos.x << " " << cursorPos.y << std::endl;
    }

    // If one of the mouse buttons is pressed this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseClicked(int button, int mods)
    {
        //std::cout << "Pressed mouse button: " << button << std::endl;
    }

    // If one of the mouse buttons is released this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseReleased(int button, int mods)
    {
        //std::cout << "Released mouse button: " << button << std::endl;
    }

private:
    Window m_window;
    Camera m_cameras[2];

    // Shader for default rendering and for depth rendering
    Shader m_defaultShader;
    Shader m_shadowShader;
    Shader m_sceneShader;
    Shader m_cubeShader;
    Shader m_wallShader;
    Shader m_lightShader;
    Shader m_pbrShader;

    std::vector<GPUMesh> m_meshes;
    Texture m_texture;
    bool m_useMaterial { true };

    // Projection and view matrices for you to fill in and use
    // FIXED: Moved to src/config.h
    /*glm::mat4 m_projectionMatrix = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 30.0f);
    glm::mat4 m_viewMatrix = glm::lookAt(glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec3(0, 1, 0));
    glm::mat4 m_modelMatrix { 1.0f };*/
    Scene m_scene{ config::scene_paths };
    Cube m_cube{ config::scene_paths };
    Wall m_wall{};
};

int main()
{
    Application app;
    app.update();

    return 0;
}
