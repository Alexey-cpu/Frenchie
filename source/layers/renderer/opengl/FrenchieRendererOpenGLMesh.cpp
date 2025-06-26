#include <FrenchieRendererOpenGLMesh.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Renderer::OpenGL;

// Mesh
Mesh::Mesh(){}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteVertexArrays(1, &m_VAO);
}

bool Mesh::is_instanced() const
{
    return !m_Vertexes.empty() && !m_Indexes.empty();
}

bool Mesh::awake()
{
    if(!is_instanced())
    {
        Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPEN_GL::SETUP_FAILED::MESH_IS_NOT_INSTANCED"));
        return false;
    }

    // create buffers and vertex array
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glGenVertexArrays(1, &m_VAO);

    // bind VAO to remember VBO/EBO configuration and layout
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertexes.size() * sizeof(Vertex), &m_Vertexes[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indexes.size() * sizeof(int),  &m_Indexes[0], GL_DYNAMIC_DRAW);

    // setup attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::Normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::UV)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    return true;
}

void Mesh::frame_start(){}

void Mesh::frame_update(){}

void Mesh::frame_finish()
{
    if(!is_instanced()) 
    {
        Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPEN_GL::RENDER_FAILED::MESH_IS_NOT_INSTANCED"));
        return;
    }

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, (int)m_Indexes.size());
    glDrawElements(GL_TRIANGLES, (int)m_Indexes.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


// Rectangle2D
Rectangle2D::Rectangle2D()
{
    m_Vertexes = 
    {
        // trangle 1
        { glm::vec3(-100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
        { glm::vec3(+100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
        { glm::vec3(-100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },

        // trangle 2
        { glm::vec3(+100.f, +100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
        { glm::vec3(+100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) },
        { glm::vec3(-100.f, -100.f, 0.f), glm::vec3(0.f), glm::vec2(0.f) }
    };

    m_Indexes = 
    {
        // triangle 1
        0, 1, 2,

        // triangle 2
        3, 4, 5
    };
}

Rectangle2D::~Rectangle2D(){}