#include <FrenchieRendererOpenGLMesh.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Renderer::OpenGL;

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

void Mesh::setup()
{
    if(!is_instanced())
    {
        Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPEN_GL::SETUP_FAILED::MESH_IS_NOT_INSTANCED"));
        return;
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
}

void Mesh::render()
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