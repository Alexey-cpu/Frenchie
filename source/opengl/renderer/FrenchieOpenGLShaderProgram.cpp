#include <FrenchieOpenGLShaderProgram.hpp>
#include <FrenchieLogger.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// STL
#include <filesystem>
#include <iostream>
#include <fstream>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace Frenchie::Renderer::OpenGL;

ShaderProgram::ShaderProgram(unsigned int _ID, const shaders& _Shaders) : m_ID(_ID)
{
    for(auto shader : _Shaders)
    {
        glAttachShader(get_id(), shader->get_id());
    }

    // link
    glLinkProgram(get_id());

    // check for linking errors
    int success = true;
    glGetProgramiv(get_id(), GL_LINK_STATUS, &success);
    
    if (!success) 
    {
        char infoLog[512];
        glGetProgramInfoLog(get_id(), 512, NULL, infoLog);
        Frenchie::Core::Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPENGL::SHADER_PROGRAM::LINK_FAILED\n"));
        Frenchie::Core::Logger::instance()->error(fmt::format("{}\n", std::string(infoLog)));
        return;
    }

    Frenchie::Core::Logger::instance()->info(fmt::format("FRENCHIE::RENDERER::OPENGL::SHADER_PROGRAM::LINK_SUCCEEDED\n"));
}

ShaderProgram::~ShaderProgram(){}

void ShaderProgram::use()
{
    glUseProgram(get_id());
}

void ShaderProgram::unuse()
{
    glUseProgram(0);
}

const unsigned int& ShaderProgram::get_id() const
{
    return m_ID;
}

#define type bool
template<> void ShaderProgram::set_uniform(const std::string& _Name, const type& _Value)
{
    glUniform1i(glGetUniformLocation(get_id(), _Name.c_str()), (int)_Value);
}

#define type int
template<> void ShaderProgram::set_uniform(const std::string& _Name, const type& _Value)
{
    glUniform1i(glGetUniformLocation(get_id(), _Name.c_str()), _Value);
}
#undef type

#define type float
template<> void ShaderProgram::set_uniform(const std::string& _Name, const type& _Value)
{
    glUniform1f(glGetUniformLocation(get_id(), _Name.c_str()), (int)_Value);
}
#undef type

#define type glm::vec2
template<> void ShaderProgram::set_uniform(const std::string& _Name, const type& _Value)
{
    glUniform2fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
}
#undef type

#define type glm::vec3
template<> void ShaderProgram::set_uniform(const std::string& _Name, const type& _Value)
{
    glUniform3fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
}
#undef type

#define type glm::vec4
template<> void ShaderProgram::set_uniform(const std::string& _Name, const type& _Value)
{
    glUniform4fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
}
#undef type

#define type glm::mat2
template<> void ShaderProgram::set_uniform(const std::string& _Name, const type& _Value)
{
    glUniformMatrix2fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}
#undef type

#define type glm::mat3
template<> void ShaderProgram::set_uniform(const std::string& _Name, const type& _Value)
{
    glUniformMatrix3fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}
#undef type

#define type glm::mat4
template<> void ShaderProgram::set_uniform(const std::string& _Name, const type& _Value)
{
    glUniformMatrix4fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}
#undef type