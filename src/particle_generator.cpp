#include "particle_generator.h"

#include <stdexcept>
#include <array>
#include <iostream>
#include <filesystem>
#include <stb/stb_image.h>

ParticleGenerator::ParticleGenerator(unsigned int amount)
    : amount(amount)
{
    particleMap = loadmap("resources/textures/particle.png");
    init();
}

ParticleGenerator::~ParticleGenerator() {
    // Clean up the buffers
    glDeleteVertexArrays(1, &particle_vao);
    glDeleteBuffers(1, &particle_vbo);
}


void ParticleGenerator::init()
{
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &particle_vao);
    glGenBuffers(1, &particle_vbo);
    glBindVertexArray(particle_vao);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, particle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // create this->amount default particle instances
    for (unsigned int i = 0; i < amount; ++i)
        particles.push_back(Particle());
}

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
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = velocity * 0.1f;
}


void ParticleGenerator::Update(float dt, glm::vec2 position, glm::vec2 velocity, unsigned int newParticles, glm::vec2 offset)
{
    // add new particles 
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = firstUnusedParticle();
        respawnParticle(particles[unusedParticle], position, velocity, offset);
    }
    // update all particles
    for (unsigned int i = 0; i < amount; ++i)
    {
        Particle& p = particles[i];
        p.Life -= dt; // reduce life
        if (p.Life > 0.0f)
        {	// particle is alive, thus update
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 2.5f;
        }
    }
}

// render all particles
void ParticleGenerator::Draw(Shader& shader, const glm::mat4& projection, const glm::vec2& offset, const glm::vec4& color)
{
    // use additive blending to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader.bind();
    for (Particle particle : particles)
    {
        if (particle.Life > 0.0f)
        {
            glUniform2fv(shader.getUniformLocation("offset"), 1, glm::value_ptr(particle.Position));
            glUniform4fv(shader.getUniformLocation("color"), 1, glm::value_ptr(particle.Color));
            glUniformMatrix4fv(shader.getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
           

            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, particleMap);
            glUniform1i(shader.getUniformLocation("sprite"), 7);

            glBindVertexArray(particle_vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // don't forget to reset to default blending mode
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