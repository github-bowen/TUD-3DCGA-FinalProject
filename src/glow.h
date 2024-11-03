#include <glm/glm.hpp>
#include <framework/shader.h>

class Glow {
public:
    Glow(); // Constructor
    ~Glow(); // Destructor

    

    // Renders the light box
    void drawLightBox(Shader& shader, const glm::mat4& model, const glm::mat4& view,
        const glm::mat4& projection, const glm::vec3& lightColor, const glm::vec3& viewPos);
    void setupQuad();

private:
    GLuint m_box_vao, m_box_vbo, m_quad_vao, m_quad_vbo; // Vertex Array Object

    // Initializes the box VAO/VBO
    void setupBox();
    
};