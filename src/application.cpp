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
#include <stb/stb_image.h>


#include "config.h"
#include "scene.h"
#include "camera.h"
#include "cube.h"
#include "wall.h"
#include "light.h"
#include "robot_arm.h"
#include "bezier_curve.h"
#include "HDR.h"
#include "particle_generator.h"
#include "minimap.h"



constexpr int bufferSize = 512;
bool IBL = false;
bool HDRupdated = true;

std::vector<Light> lights{};
size_t selectedLightIndex = 0;

enum class ShadingMode {
    simple,
    PBR
};
ShadingMode shadingMode = ShadingMode::simple;

bool albedoTex = false;
bool roughnessTex = false;
bool metallicTex = false;
bool aoTex = false;
bool normalMapping = false;

class Application {
public:
    Texture* albedoMap = nullptr;
    Texture* roughnessMap = nullptr;
    Texture* metallicMap = nullptr;
    Texture* aoMap = nullptr;
    Texture* normalMap = nullptr;
    Texture* equiMap = nullptr;

    std::string albedoPath = "resources/textures/gold-nugget1_albedo.png";
    std::string roughnessPath = "resources/textures/gold-nugget1_roughness.png";
    std::string metallicPath = "resources/textures/gold-nugget1_metallic.png";
    std::string aoPath = "resources/textures/gold-nugget1_ao.png";
    std::string normalPath = "resources/textures/gold-nugget1_normal-dx.png";
    std::string equiPath = "resources/equirectangular/Footprint_Court_8k_TMap.jpg";
    //std::string equiPath = "resources/equirectangular/modern_house_indoors.png"; 
    //std::string equiPath = "resources/equirectangular/Milkyway_BG.jpg";
    //std::string equiPath = "resources/equirectangular/Road_to_MonumentValley_8k.jpg";
    //std::string equiPath = "resources/equirectangular/Frozen_Waterfall_HiRes_TMap.jpg";

    char albedoBuffer[bufferSize];
    char roughnessBuffer[bufferSize];
    char metallicBuffer[bufferSize];
    char aoBuffer[bufferSize];
    char normalBuffer[bufferSize];
    char equiBuffer[bufferSize];

    Application()
        : m_window("Final Project", glm::ivec2(1024, 1024), OpenGLVersion::GL41)
        , m_texture(RESOURCE_ROOT TEXTURE_PATH, false), m_bezierCurve(true, 0.0), 
          m_cameras{
            Camera { &m_window, glm::vec3(1.2f, 1.1f, 0.9f), -glm::vec3(1.2f, 1.1f, 0.9f) }, // Main camera
            Camera { &m_window, glm::vec3(-1, 10, -1), -glm::vec3(-1, 10, -1) }          // New camera
        }      

    {
        lights.push_back(Light(glm::vec3(0.5f, 1.0f, 0.3f), glm::vec3(1.0f, 1.0f, 1.0f)));
        lights.push_back(Light(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
        lights.push_back(Light(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
        for (int i = 0; i < 7; ++i) {
            lights.push_back(Light(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
        }


        strncpy(albedoBuffer, albedoPath.c_str(), bufferSize - 1);
        albedoBuffer[bufferSize - 1] = '\0';
        strncpy(roughnessBuffer, roughnessPath.c_str(), bufferSize - 1);
        roughnessBuffer[bufferSize - 1] = '\0';
        strncpy(metallicBuffer, metallicPath.c_str(), bufferSize - 1);
        metallicBuffer[bufferSize - 1] = '\0';
        strncpy(aoBuffer, aoPath.c_str(), bufferSize - 1);
        aoBuffer[bufferSize - 1] = '\0';
        strncpy(normalBuffer, normalPath.c_str(), bufferSize - 1);
        normalBuffer[bufferSize - 1] = '\0';
        strncpy(equiBuffer, equiPath.c_str(), bufferSize - 1);
        equiBuffer[bufferSize - 1] = '\0';

        albedoMap = new Texture(albedoPath, false);
        roughnessMap = new Texture(roughnessPath, false);
        metallicMap = new Texture(metallicPath, false);
        aoMap = new Texture(aoPath, false);
        normalMap = new Texture(normalPath, false);
        equiMap = new Texture(equiPath, true);

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

        std::vector<std::string> onItemStrings;
        std::vector<size_t> onItemIndices;
        std::vector<std::string> offItemStrings;
        std::vector<size_t> offItemIndices;

        for (size_t i = 0; i < lights.size(); i++) {
            auto string = "Light " + std::to_string(i);
            if (glm::length(lights[i].color) > 0.001f) {
                onItemStrings.push_back(string);
                onItemIndices.push_back(i);
            }
            else {
                offItemStrings.push_back(string);
                offItemIndices.push_back(i);
            }
        }
        std::vector<const char*> onItemCStrings;
        for (const auto& string : onItemStrings) {
            onItemCStrings.push_back(string.c_str());
        }
        std::vector<const char*> offItemCStrings;
        for (const auto& string : offItemStrings) {
            offItemCStrings.push_back(string.c_str());
        }
        int tempSelectedOnItem = -1;
        int tempSelectedOffItem = -1;
        ImGui::Text("On Lights");
        if (ImGui::ListBox("##onLightsList", &tempSelectedOnItem, onItemCStrings.data(), (int)onItemCStrings.size(), 3) && tempSelectedOnItem != -1) {
            selectedLightIndex = onItemIndices[tempSelectedOnItem];
        }
        ImGui::Text("Off Lights");
        if (ImGui::ListBox("##offLightsList", &tempSelectedOffItem, offItemCStrings.data(), (int)offItemCStrings.size(), 3) && tempSelectedOffItem != -1) {
            selectedLightIndex = offItemIndices[tempSelectedOffItem];
        }


        //std::cout << "current camera index: " << config::activeCameraIndex << std::endl;

        ImGui::DragFloat3("LightPos", glm::value_ptr(lights[selectedLightIndex].position), 0.01f, -10.0, 10.0, "%.2f");
        ImGui::ColorEdit3("LightColor", &lights[selectedLightIndex].color[0]);

        ImGui::Separator();
        ImGui::Text("Shading");
        const char* shadingModes[] = { "Simple", "PBR" };
        int currentShading = static_cast<int>(shadingMode);
        if (ImGui::Combo("Shading Mode", &currentShading, shadingModes, IM_ARRAYSIZE(shadingModes))) {
            shadingMode = static_cast<ShadingMode>(currentShading);
        }
        if (shadingMode == ShadingMode::simple) {
            GPUMaterial& material = m_meshes[0].material;
            for (auto& mesh : m_meshes)
                material = mesh.material;
            ImGui::Checkbox("Normal Mapping", &normalMapping);
            ImGui::InputText("Normal Map", normalBuffer, bufferSize);
            if (ImGui::Button("Load Normal")) {
                delete normalMap;
                normalPath = normalBuffer;
                normalMap = new Texture(normalPath, false);
                for (auto& mesh : m_meshes) { mesh.normalMap = normalMap; }
            }
            ImGui::Separator();
            ImGui::Text("Material Properties");
            ImGui::ColorEdit3("Diffuse Color (Kd)", glm::value_ptr(material.kd));
            ImGui::ColorEdit3("Specular Color (Ks)", glm::value_ptr(material.ks));
            ImGui::SliderFloat("Shininess", &material.shininess, 0.0f, 50.0f);
            material.updateUBO();
        }
        else if (shadingMode == ShadingMode::PBR) {
            GPUMaterial& material = m_meshes[0].material;
            for (auto& mesh : m_meshes)
                material = mesh.material;
            ImGui::Checkbox("Image Based Lighting", &IBL);
            ImGui::InputText("HDR Map", equiBuffer, bufferSize);
            if (ImGui::Button("Load HDR")) {
                delete equiMap;
                equiPath = equiBuffer;
                equiMap = new Texture(equiPath, true);
                m_HDR.equiMap = equiMap;
                HDRupdated = true;
            }
            ImGui::Checkbox("Normal Mapping", &normalMapping);
            ImGui::InputText("Normal Map", normalBuffer, bufferSize);
            if (ImGui::Button("Load Normal")) {
                delete normalMap;
                normalPath = normalBuffer;
                normalMap = new Texture(normalPath, false);
                for (auto& mesh : m_meshes) { mesh.normalMap = normalMap; }
            }
            ImGui::Separator();
            ImGui::Text("Material Properties");
            ImGui::Checkbox("Albedo Texture", &albedoTex);
            ImGui::InputText("Albedo Map", albedoBuffer, bufferSize);
            if (ImGui::Button("Load Albedo")) {
                delete albedoMap;
                albedoPath = albedoBuffer;
                albedoMap = new Texture(albedoPath, false);
                for (auto& mesh : m_meshes) { mesh.albedoMap = albedoMap; }
            }
            ImGui::ColorEdit3("Albedo", glm::value_ptr(material.albedo));

            ImGui::Checkbox("Roughness Texture", &roughnessTex);
            ImGui::InputText("Roughness Map", roughnessBuffer, bufferSize);
            if (ImGui::Button("Load Roughness")) {
                delete roughnessMap;
                roughnessPath = roughnessBuffer;
                roughnessMap = new Texture(roughnessPath, false);
                for (auto& mesh : m_meshes) { mesh.roughnessMap = roughnessMap; }
            }
            ImGui::SliderFloat("Roughness", &material.roughness, 0.0f, 1.0f);

            ImGui::Checkbox("Metallic Texture", &metallicTex);
            ImGui::InputText("Metallic Map", metallicBuffer, bufferSize);
            if (ImGui::Button("Load Metallic")) {
                delete metallicMap;
                metallicPath = metallicBuffer;
                metallicMap = new Texture(metallicPath, false);
                for (auto& mesh : m_meshes) { mesh.metallicMap = metallicMap; }
            }
            ImGui::SliderFloat("Metallic", &material.metallic, 0.0f, 1.0f);

            ImGui::Checkbox("Ao Texture", &aoTex);
            ImGui::InputText("Ao Map", aoBuffer, bufferSize);
            if (ImGui::Button("Load Ao")) {
                delete aoMap;
                aoPath = aoBuffer;
                aoMap = new Texture(aoPath, false);
                for (auto& mesh : m_meshes) { mesh.aoMap = aoMap; }
            }
            ImGui::SliderFloat("AO", &material.ao, 0.0f, 1.0f);
            material.updateUBO();
        }

        /*ImGui::End();
        ImGui::Begin("UI Panel 2");*/

		ImGui::Separator();
		ImGui::Text("Arm Segment Controls");
		ImGui::Checkbox("See Robot Arm", &see_robot_arm);
		ImGui::Checkbox("Animate", &animate);
		for (size_t i = 0; i < armSegments.size(); ++i) {
			ImGui::PushID(static_cast<int>(i)); // Use a unique ID for each segment

			ImGui::Text("Segment %zu", i + 1);

			// RotateX Slider - Convert angle to degrees for user-friendly adjustment
			float rotateXDegrees = glm::degrees(armSegments[i].rotationX);
			if (ImGui::SliderFloat("Rotation (degrees)", &rotateXDegrees, -360.0f, 360.0f)) {
				armSegments[i].rotationX = glm::radians(rotateXDegrees);
			}

			// Size Sliders - Allow individual adjustment of each component
			ImGui::SliderFloat3("Size", &armSegments[i].boxSize[0], 0.1f, 5.0f);

			ImGui::Separator(); // Add a separator between each segment control
			ImGui::PopID();
		}

        /* Bezier Curve Components*/
		// Minimal Features of Bezier Curve
        ImGui::Separator();
        ImGui::Text("Bezier Path Controls (A bezier path of 3 cubic bezier curves)");
        ImGui::Checkbox("Show Bezier Path", &m_bezierCurve.showBezierPath);

        ImGui::BeginDisabled(m_bezierCurve.showBezierPath == false);
            ImGui::SliderFloat("Path Progress [0, 1]", &m_bezierCurve.time, 0.0f, 1.0f);

		// Extra Features of Bezier Curve
		    ImGui::Checkbox("Automatically Moving at Constant Speed", &m_bezierCurve.movingAtConstantSpeed);
            ImGui::BeginDisabled(m_bezierCurve.movingAtConstantSpeed == false);
                ImGui::SliderFloat("Speed", &m_bezierCurve.speed, 0.01f, 0.2f);
            ImGui::EndDisabled();
        ImGui::EndDisabled();

        ImGui::Separator();
        ImGui::Checkbox("See Particle Effect", &see_particle_effect);
        // Control for position
        /*ImGui::Text("Position");
        ImGui::SliderFloat("Position X", &position.x, -10.0f, 10.0f);
        ImGui::SliderFloat("Position Y", &position.y, -10.0f, 10.0f);*/

        ImGui::End();
    }

    void __init_callback() {
        m_window.registerKeyCallback([this](int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS)
                onKeyPressed(key, mods);
            else if (action == GLFW_RELEASE)
                onKeyReleased(key, mods);

            if (key == '\\' && action == GLFW_PRESS) {
                config::show_imgui = !config::show_imgui;
            }
            const bool shiftPressed = m_window.isKeyPressed(GLFW_KEY_LEFT_SHIFT) || m_window.isKeyPressed(GLFW_KEY_RIGHT_SHIFT);
            if (action != GLFW_RELEASE)
                return;
            switch (key) {
            case GLFW_KEY_L: {
                Camera& currentView = m_cameras[config::activeCameraIndex];
                glm::vec3 newLightPos = currentView.cameraPos();
                if (shiftPressed) {
                    lights[selectedLightIndex].position = newLightPos;
                    if(lights[selectedLightIndex].color == glm::vec3(0.0))
                        lights[selectedLightIndex].color = glm::vec3(1.0);
                }
                else
                    lights[selectedLightIndex].color = glm::vec3(0.0);
                return;
            }
            default:
                return;
            };
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

            ShaderBuilder robotBuilder;
            robotBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/robot_arm_vert.glsl");
            robotBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/robot_arm_frag.glsl");
            m_robotShader = robotBuilder.build();

            ShaderBuilder pbrBuilder;
            pbrBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/shader_vert.glsl");
            pbrBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/pbr_frag.glsl");
            m_pbrShader = pbrBuilder.build();

            ShaderBuilder HDRcubeBuilder;
            HDRcubeBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/HDRcube_vert.glsl");
            HDRcubeBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/HDRcube_frag.glsl");
            m_HDRcubeShader = HDRcubeBuilder.build();

            ShaderBuilder HDRsceneBuilder;
            HDRsceneBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/HDRscene_vert.glsl");
            HDRsceneBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/HDRscene_frag.glsl");
            m_HDRsceneShader = HDRsceneBuilder.build();

            ShaderBuilder HDRconvBuilder;
            HDRconvBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/HDRcube_vert.glsl");
            HDRconvBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/HDRconv_frag.glsl");
            m_HDRconvShader = HDRconvBuilder.build();

            ShaderBuilder HDRpreBuilder;
            HDRpreBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/HDRcube_vert.glsl");
            HDRpreBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/HDRpre_frag.glsl");
            m_HDRpreShader = HDRpreBuilder.build();

            ShaderBuilder brdfBuilder;
            brdfBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/BRDF_vert.glsl");
            brdfBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/BRDF_frag.glsl");
            m_brdfShader = brdfBuilder.build();

            ShaderBuilder particleBuilder;
            particleBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/particle_vert.glsl");
            particleBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/particle_frag.glsl");
            m_particleShader = particleBuilder.build();

            ShaderBuilder minimapBuilder;
            minimapBuilder.addStage(GL_VERTEX_SHADER, RESOURCE_ROOT "shaders/minimap_vert.glsl");
            minimapBuilder.addStage(GL_FRAGMENT_SHADER, RESOURCE_ROOT "Shaders/minimap_frag.glsl");
            m_minimapShader = minimapBuilder.build();

        } catch (ShaderLoadingException e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void update()
    {
        int dummyInteger = 0; // Initialized to 0

        for (unsigned int i = 0; i < nr_particles; ++i)
            particles.push_back(Particle());
        Particles = new ParticleGenerator(500);

        
        glm::vec2 offset(0.0f, 0.0f);        // Offset for initial particle spawning position
        unsigned int newParticles = 20;      // Number of new particles to spawn per frame
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        glm::mat4 minimapView, minimapProjection;
        m_minimap.setupHighAltitudeCamera(minimapView, minimapProjection);


        while (!m_window.shouldClose()) {
            // This is your game loop
            // Put your real-time logic and rendering in here
            
            if (HDRupdated) {
                m_HDR.equiMap = equiMap;
                m_HDR.equiToCube(m_HDRcubeShader);
                m_HDR.cubeToConv(m_HDRconvShader);
                m_HDR.cubeToPre(m_HDRpreShader);
                m_HDR.brdfLUT(m_brdfShader);
                HDRupdated = false;
            }
            glm::ivec2 windowSize = m_window.getFrameBufferSize();
            int viewportSize = std::min(windowSize.x, windowSize.y);
            int x = (windowSize.x - viewportSize) / 2;
            int y = (windowSize.y - viewportSize) / 2;


            glViewport(x, y, viewportSize, viewportSize);

            m_window.updateInput();
            m_cameras[config::activeCameraIndex].updateInput();

            imgui();

            // Clear the screen
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// Render to framebuffer for minimap
            glBindFramebuffer(GL_FRAMEBUFFER, m_minimap.getFramebuffer());

            int minimapViewportSize = 200;
            int minimapX = windowSize.x - minimapViewportSize - 20;
            int minimapY = windowSize.y - minimapViewportSize - 20;

            glViewport(minimapX, minimapY, minimapViewportSize, minimapViewportSize);

            m_minimap.draw(m_minimapShader, config::m_modelMatrix, minimapView, minimapProjection);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(x, y, viewportSize, viewportSize);

            // ...
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
            //glEnable(GL_BLEND);

            const glm::mat4 view = m_cameras[config::activeCameraIndex].viewMatrix();
            const glm::mat4 mvpMatrix = config::m_projectionMatrix * view * config::m_modelMatrix;
            // Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
            // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
            const glm::mat3 normalModelMatrix = config::normalModelMatrix;


		



            // render scene: remove translation from the view matrix
            glm::mat4 sceneView = glm::mat4(glm::mat3(view));
            //m_scene.draw(m_sceneShader, config::m_projectionMatrix, sceneView, config::textureSlots.at("scene"));
            if(!IBL)    m_scene.draw(m_sceneShader, config::m_projectionMatrix, sceneView);
            else if(IBL)    m_HDR.renderScene(m_HDRsceneShader, config::m_projectionMatrix, view);

            Camera& currentCamera = m_cameras[config::activeCameraIndex];
            glm::vec3 cameraPos = currentCamera.cameraPos();
            if (!IBL)   m_cube.draw(m_cubeShader, config::m_modelMatrix, config::normalModelMatrix, view, config::m_projectionMatrix, cameraPos, GL_TEXTURE1);
            else if(IBL)    m_cube.draw(m_cubeShader, config::m_modelMatrix, config::normalModelMatrix, view, config::m_projectionMatrix, cameraPos, GL_TEXTURE11);

            // Assuming you want to rotate around the x-axis by 90 degrees
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the x-axis
            model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::translate(model, glm::vec3(0.0, 0.0, -2.0));
            m_wall.draw(m_wallShader, config::m_projectionMatrix, view, model, cameraPos, lights[selectedLightIndex].position);



            if (see_particle_effect) {
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;
                // Update particles
                Particles->Update(deltaTime, position, velocity, newParticles, offset);
                // Draw particles
                Particles->Draw(m_particleShader, config::m_projectionMatrix, view, model, offset);
            }




            
            if (see_robot_arm) {
                static bool wasAnimating = false;
                if (animate) {

                    static float lastFrameTime = glfwGetTime();
                    static float elapsedTime = 0.0f;

                    // Detect when 'animate' is switched to true and reset elapsedTime
                    if (!wasAnimating) {
                        elapsedTime = 0.0f;  // Reset the elapsed time when animation starts
                        wasAnimating = true; // Mark that we are now animating
                    }

                    float currentFrameTime = glfwGetTime();
                    float deltaTime = currentFrameTime - lastFrameTime;
                    lastFrameTime = currentFrameTime;

                    // Animation sequence
                    if (elapsedTime < 5.0f) {
                        elapsedTime += deltaTime;
                        armSegments[0].animate(deltaTime * 20);
                    }
                    else if (elapsedTime >= 5.0f && elapsedTime < 10.0f) {
                        elapsedTime += deltaTime;
                        armSegments[1].animate(deltaTime * 20);
                    }
                    else if (elapsedTime >= 10.0f && elapsedTime < 15.0f) {
                        elapsedTime += deltaTime;
                        armSegments[2].animate(deltaTime * 20);
                    }
                    else {
                        elapsedTime = 0.0f; // Reset the sequence after all segments have animated
                    }

                    std::vector<glm::mat4> transforms = dummy.computeTransformMatrix(armSegments);
                    for (size_t i = 0; i < transforms.size(); ++i) {
                        dummy.draw(m_robotShader, config::m_modelMatrix, config::m_projectionMatrix, view, transforms[i]);
                    }
                }
                else {
                    // Reset wasAnimating flag when 'animate' is false
                    wasAnimating = false;

                    std::vector<glm::mat4> transformMatrices = dummy.computeTransformMatrix(armSegments);
                    for (const auto& transform : transformMatrices) {
                        dummy.draw(m_robotShader, config::m_modelMatrix, config::m_projectionMatrix, view, transform);
                    }
                }
            }

           
            // Bezier curves
			if (m_bezierCurve.showBezierPath) {
                // FIXME: directly apply bezier curve at the first light
                

				if (m_bezierCurve.movingAtConstantSpeed) {
					m_bezierCurve.time += m_bezierCurve.speed * 0.01f;
					if (m_bezierCurve.time > 1.0f) {
						m_bezierCurve.time = 0.0f;
					}
				}

                lights[0].position = m_bezierCurve.getCurrentPointOnPath();
			}                  


			// PBR shading

            m_pbrShader.bind();
            glUniform3fv(m_pbrShader.getUniformLocation("cameraPos"), 1, glm::value_ptr(cameraPos));
            int numLights = lights.size();
            glUniform1i(m_pbrShader.getUniformLocation("numLights"), numLights);
            for (size_t i = 0; i < lights.size(); i++) {
                std::string lightPosName = "lights[" + std::to_string(i) + "].position";
                std::string lightColorName = "lights[" + std::to_string(i) + "].color";
                std::string lightDirName = "lights[" + std::to_string(i) + "].direction";
                glUniform3fv(m_pbrShader.getUniformLocation(lightPosName.c_str()), 1, glm::value_ptr(lights[i].position));
                glUniform3fv(m_pbrShader.getUniformLocation(lightColorName.c_str()), 1, glm::value_ptr(lights[i].color));
                //glUniform3fv(m_pbrShader.getUniformLocation(lightDirName.c_str()), 1, glm::value_ptr(lights[i].direction));
            }

            int shading;
            if (shadingMode == ShadingMode::simple) {shading = 0;  IBL = false;}
            else if (shadingMode == ShadingMode::PBR)    shading = 1;
            glUniform1i(m_pbrShader.getUniformLocation("shadingMode"), shading);
            for (GPUMesh& mesh : m_meshes) {
                mesh.albedoMap = albedoMap;
                mesh.roughnessMap = roughnessMap;
                mesh.metallicMap = metallicMap;
                mesh.aoMap = aoMap;
                mesh.normalMap = normalMap;
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

                    glUniform1i(m_pbrShader.getUniformLocation("IBL"), IBL);
                    glActiveTexture(GL_TEXTURE13);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, m_HDR.irradianceMap);
                    glUniform1i(m_pbrShader.getUniformLocation("irradianceMap"), 13);
                    glActiveTexture(GL_TEXTURE15);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, m_HDR.prefilterMap);
                    glUniform1i(m_pbrShader.getUniformLocation("prefilterMap"), 15);
                    glActiveTexture(GL_TEXTURE16);
                    glBindTexture(GL_TEXTURE_2D, m_HDR.brdfTex);
                    glUniform1i(m_pbrShader.getUniformLocation("brdfTex"), 16);
                }
                mesh.draw(m_pbrShader);
            }

            for (const Light& light : lights) {
                if(light.color != glm::vec3(0.0))
                    light.renderLightSource(m_lightShader, mvpMatrix);
            }


            //int minimapViewportSize = 200;
            //int minimapX = windowSize.x - minimapViewportSize - 20;
            //int minimapY = windowSize.y - minimapViewportSize - 20;

            //glViewport(minimapX, minimapY, minimapViewportSize, minimapViewportSize);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST); // Disable depth test for 2D rendering
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer (back to the screen)

            //glViewport(x, y, viewportSize, viewportSize);
            glViewport(0, 0, m_minimap.getTextureWidth(), m_minimap.getTextureHeight());

            m_minimap.drawQuadWithTexture(m_minimapShader); // Render the minimap quad with the texture

			glEnable(GL_DEPTH_TEST); // Re-enable depth test


            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cout << "OpenGL error: " << error << std::endl;
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
    Shader m_robotShader;
    Shader m_lightShader;
    Shader m_pbrShader;
    Shader m_HDRcubeShader;
    Shader m_HDRsceneShader;
    Shader m_HDRconvShader;
    Shader m_HDRpreShader;
    Shader m_brdfShader;
    Shader m_particleShader;
	Shader m_minimapShader;

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
    ArmSegment dummy;
    std::vector<ArmSegment> armSegments{
        ArmSegment { glm::radians(-40.0f), glm::vec3(1, 1, 3) },
        ArmSegment { glm::radians(30.0f), glm::vec3(1.0f, 0.6f, 2) },
        ArmSegment { glm::radians(40.0f), glm::vec3(0.3f, 0.3f, 1) }
    };
    bool animate{ false };
    bool see_robot_arm{ false };
    HDR m_HDR{};
    BezierCurve m_bezierCurve {true, 0.0};
	Minimap m_minimap { 300.0f, 300.0f, 256, 256 };
    ParticleGenerator* Particles;
    unsigned int nr_particles = 500;
    std::vector<Particle> particles;
    bool see_particle_effect{ false };
    glm::vec2 position{ 0.1f, 0.26f }; // Initial position at screen center
    glm::vec2 velocity{ -1.5f, 0.0f };   // Initial velocity with slight upward motion
    
};

int main()
{
    Application app;
    app.update();

    return 0;
}
