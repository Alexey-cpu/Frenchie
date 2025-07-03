#include <FrenchieRendererMesh.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Renderer;

#include <cstddef>
#include <iostream>

// Mesh
Mesh::Mesh(std::vector<Vertex> _Vertexes) : 
    m_Vertexes(_Vertexes)
{
    // generate vertexes indexses
    for(size_t i = 0; i < m_Vertexes.size(); i++) 
        m_Indexes.push_back((int)i);

    // generate axis aligned bounding box (AABB)
    glm::vec3 max = glm::vec3(0.f);
    glm::vec3 min = glm::vec3(0.f);

    for(auto&& vertex : m_Vertexes)
    {
        max = glm::vec3(
            std::max<float>(max.x, vertex.Position.x), 
            std::max<float>(max.y, vertex.Position.y), 
            std::max<float>(max.z, vertex.Position.z));

        min = glm::vec3(
            std::min<float>(min.x, vertex.Position.x), 
            std::min<float>(min.y, vertex.Position.y), 
            std::min<float>(min.z, vertex.Position.z));
    }

    m_AABB = Aabb(min - glm::vec3(0.f), max + glm::vec3(0.f));
}

Mesh::~Mesh()
{
    if(m_VBO != 0)
        glDeleteBuffers(1, &m_VBO);
    
    if(m_EBO != 0)
        glDeleteBuffers(1, &m_EBO);
    
    if(m_VAO != 0) 
        glDeleteVertexArrays(1, &m_VAO);
}

Aabb Mesh::get_aabb() const
{
    return m_AABB;
}

bool Mesh::instantiate()
{
    // create buffers and vertex array
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glGenVertexArrays(1, &m_VAO);

    // bind VAO to remember VBO/EBO configuration and layout
    glBindVertexArray(m_VAO);

    // load vertexes and indexes on GPU
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertexes.size() * sizeof(Vertex), &m_Vertexes[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indexes.size() * sizeof(int),  &m_Indexes[0], GL_DYNAMIC_DRAW);

    // setup attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, UV)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    return true;
}

void Mesh::render()
{
    // bind VAO containing VBO, EBO
    glBindVertexArray(m_VAO);

    // get EBO size
    int bufferSize; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

    // draw EBO
    glDrawArrays(GL_POINTS, 0, bufferSize);
    glDrawElements(GL_TRIANGLES, bufferSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}