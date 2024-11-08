#include "particle_generator.h"

#include <stdexcept>
#include <array>
#include <iostream>
#include <filesystem>
#include <stb/stb_image.h>

ParticleGenerator::ParticleGenerator(unsigned int amount)
    : amount(amount)
{
    particleMap = loadmap("resources/textures/water.jpg");
    init();
}

ParticleGenerator::~ParticleGenerator() {
    // Clean up the buffers
    glDeleteVertexArrays(1, &particle_vao);
    glDeleteBuffers(1, &particle_vbo);
}



void ParticleGenerator::init()
{
    const int circleVertexCount = 36; // number of segments for the circle
    std::vector<float> circleVertices;

    // Generate circle vertices
    for (int i = 0; i < circleVertexCount; ++i)
    {
        float angle = 2.0f * glm::pi<float>() * float(i) / float(circleVertexCount);
        circleVertices.push_back(cos(angle)); // x
        circleVertices.push_back(sin(angle)); // y
        circleVertices.push_back(0.0f);                // z
        circleVertices.push_back(1.0f);                // texture coordinates (u, v)
    }

    // Create and set up the vertex array object and vertex buffer object
    glGenVertexArrays(1, &particle_vao);
    glGenBuffers(1, &particle_vbo);
    glBindVertexArray(particle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, particle_vbo);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    for (unsigned int i = 0; i < amount; ++i)
        particles.push_back(Particle());
}

// A key reference for learning and implementing particle effects is the article by Joey de Vries on his website LearnOpenGL: 
// https://learnopengl.com/In-Practice/2D-Game/Particles

unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    // first search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < amount; ++i) {
        if (particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}


void ParticleGenerator::respawnParticle(Particle& particle, glm::vec2 position, glm::vec2 velocity, glm::vec2 offset)
{
    
    float coneAngle = glm::radians(30.0f);

    
    float randomAngle = (static_cast<float>(rand()) / RAND_MAX) * 2 * coneAngle - coneAngle;

   
    glm::vec2 direction = glm::vec2(
        cos(randomAngle) * velocity.x - sin(randomAngle) * velocity.y,
        sin(randomAngle) * velocity.x + cos(randomAngle) * velocity.y
    );

    
    particle.Position = position + offset;
    particle.Color = glm::vec4(1.0f, 0.5f + ((rand() % 100) / 100.0f) * 0.5f, 0.0f, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = direction * glm::length(velocity);  
    particle.Radius = 1.0f;
}




void ParticleGenerator::Update(float dt, glm::vec2 position, glm::vec2 velocity, unsigned int newParticles, glm::vec2 offset)
{
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = firstUnusedParticle();
        respawnParticle(particles[unusedParticle], position, velocity, offset);
    }

    for (unsigned int i = 0; i < amount; ++i)
    {
        Particle& p = particles[i];
        p.Life -= dt;
        if (p.Life > 0.0f)
        {
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 2.5f;
            p.Velocity.y += 3.0f * dt;

            
            p.Radius += dt; 
        }
    }
}


void ParticleGenerator::Draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, const glm::vec2& offset)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader.bind();

    for (Particle& particle : particles) 
    {
        if (particle.Life > 0.0f)
        {
            // Pass the offset, radius, and life to the shader
            glUniform2fv(shader.getUniformLocation("offset"), 1, glm::value_ptr(particle.Position));
            glUniform1f(shader.getUniformLocation("radius"), 1.0f); 
            glUniform1f(shader.getUniformLocation("life"), particle.Life); // Particle's life

            glUniformMatrix4fv(shader.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(shader.getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(shader.getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));

            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, particleMap);
            glUniform1i(shader.getUniformLocation("sprite"), 7);

            glBindVertexArray(particle_vao);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 36); // Draw the circle using triangle fan
            glBindVertexArray(0);
        }
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


unsigned int ParticleGenerator::loadmap(char const* path)
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