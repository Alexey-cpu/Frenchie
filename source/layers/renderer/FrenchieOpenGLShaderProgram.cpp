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

ShaderProgram::ShaderProgram(
    unsigned int _ID, 
    const std::vector<std::shared_ptr<Shader>>& _Shaders) : m_ID(_ID)
{
    for(auto shader : _Shaders)
    {
        if(shader != nullptr)
            glAttachShader(get_id(), shader->get_id());
    }

    int success = true;
    glLinkProgram(get_id());
    glGetProgramiv(get_id(), GL_LINK_STATUS, &success);
    
    if (!success) 
    {
        char infoLog[512];
        glGetProgramInfoLog(get_id(), 512, NULL, infoLog);
        Frenchie::Core::Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPENGL::SHADER_PROGRAM::LINK_FAILED\n"));
        Frenchie::Core::Logger::instance()->error(fmt::format("{}\n", std::string(infoLog)));
    }

    else 
    {
        Frenchie::Core::Logger::instance()->info(fmt::format("FRENCHIE::RENDERER::OPENGL::SHADER_PROGRAM::LINK_SUCCEEDED\n"));
    }
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


template<> void ShaderProgram::set_uniform(const std::string& _Name, const bool& _Value)
{
    glUniform1i(glGetUniformLocation(get_id(), _Name.c_str()), (int)_Value);
}

template<> void ShaderProgram::set_uniform(const std::string& _Name, const int& _Value)
{
    glUniform1i(glGetUniformLocation(get_id(), _Name.c_str()), _Value);
}

template<> void ShaderProgram::set_uniform(const std::string& _Name, const float& _Value)
{
    glUniform1f(glGetUniformLocation(get_id(), _Name.c_str()), _Value);
}

template<> void ShaderProgram::set_uniform(const std::string& _Name, const glm::vec2& _Value)
{
    glUniform2fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
}

template<> void ShaderProgram::set_uniform(const std::string& _Name, const glm::vec3& _Value)
{
    glUniform3fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
}

template<> void ShaderProgram::set_uniform(const std::string& _Name, const glm::vec4& _Value)
{
    glUniform4fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
}

template<> void ShaderProgram::set_uniform(const std::string& _Name, const glm::mat2& _Value)
{
    glUniformMatrix2fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}

template<> void ShaderProgram::set_uniform(const std::string& _Name, const glm::mat3& _Value)
{
    glUniformMatrix3fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}

template<> void ShaderProgram::set_uniform(const std::string& _Name, const glm::mat4& _Value)
{
    glUniformMatrix4fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}