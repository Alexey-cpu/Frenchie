#include <FrenchieRendererOpenGLShader.hpp>

#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreLogger.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// STL
#include <fstream>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace Frenchie::Renderer::OpenGL;

// Shader
ShaderLoader::ShaderLoader(const std::filesystem::path& _Path)
{
    Frenchie::Core::Logger::instance()->info("FRENCHIE::RENDERER::OPENGL::SHADER::PROCESSING_SHADER");
    Frenchie::Core::Logger::instance()->info(fmt::format("\t{}", _Path.string()));

    // check shader format
    auto extention = Frenchie::Core::get_file_extention(_Path);

    if(m_Extentions.find(extention) == m_Extentions.end())
    {
        Frenchie::Core::Logger::instance()->error(fmt::format("\tFRENCHIE::RENDERER::OPENGL::SHADER::LOAD_FAILED"));
        Frenchie::Core::Logger::instance()->error(fmt::format("\tFRENCHIE::RENDERER::OPENGL::SHADER::UNSUPPORTED_SHADER_FORMAT"));
        return;
    }

    if(extention == ".vert") 
        m_ID = glCreateShader(GL_VERTEX_SHADER);
    // else if(extention == ".tesc") 
    //     m_ID = glCreateShader(GL_FRAGMENT_SHADER);
    // else if(extention == ".tese") 
    //     m_ID = glCreateShader(GL_FRAGMENT_SHADER);
    // else if(extention == ".geom") 
    //     m_ID = glCreateShader(GL_GEOMETRY_SHADER);
    // else if(extention == ".frag") 
    //     m_ID = glCreateShader(GL_FRAGMENT_SHADER);
    else if(extention == ".frag") 
        m_ID = glCreateShader(GL_FRAGMENT_SHADER);

    // load
    std::ifstream ifstream(_Path);
    std::string source = std::string(
        (std::istreambuf_iterator<char>(ifstream)), 
        (std::istreambuf_iterator<char>()));

    if(source.empty())
    {
        Frenchie::Core::Logger::instance()->error(fmt::format("\tFRENCHIE::RENDERER::OPENGL::SHADER::LOAD_FAILED"));
        return;
    }
    
    Frenchie::Core::Logger::instance()->info(fmt::format("\tFRENCHIE::RENDERER::OPENGL::SHADER::LOAD_SUCCEEDED"));

    // compile
    int status = 1;
    const char* shaderSourceCode = source.c_str();
    glShaderSource(get_id(), 1, &shaderSourceCode, nullptr);
    glCompileShader(get_id());
    glGetShaderiv(get_id(), GL_COMPILE_STATUS, &status);

    if(!status)
    {
        char infoLog[512];
        glGetShaderInfoLog(get_id(), 512, nullptr, infoLog);
        Frenchie::Core::Logger::instance()->error(fmt::format("\tFRENCHIE::RENDERER::OPENGL::SHADER::COMPILATION_FAILED"));
        return;
    }

    Frenchie::Core::Logger::instance()->info(fmt::format("\tFRENCHIE::RENDERER::OPENGL::SHADER::COMPILATION_SUCCEEDED"));
}

ShaderLoader::~ShaderLoader()
{
    glDeleteShader(get_id());
}

const unsigned int& ShaderLoader::get_id() const
{
    return m_ID;
}

// ShaderProgram
Shader::Shader(const std::vector<std::shared_ptr<ShaderLoader>>& _Shaders) : m_ID(glCreateProgram())
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

Shader::~Shader(){}

void Shader::begin()
{
    glUseProgram(get_id());
}

void Shader::end()
{
    glUseProgram(0);
}

const unsigned int& Shader::get_id() const
{
    return m_ID;
}

template<> void Shader::set_uniform(const std::string& _Name, const bool& _Value)
{
    glUniform1i(glGetUniformLocation(get_id(), _Name.c_str()), (int)_Value);
}

template<> void Shader::set_uniform(const std::string& _Name, const int& _Value)
{
    glUniform1i(glGetUniformLocation(get_id(), _Name.c_str()), _Value);
}

template<> void Shader::set_uniform(const std::string& _Name, const float& _Value)
{
    glUniform1f(glGetUniformLocation(get_id(), _Name.c_str()), _Value);
}

template<> void Shader::set_uniform(const std::string& _Name, const glm::vec2& _Value)
{
    glUniform2fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
}

template<> void Shader::set_uniform(const std::string& _Name, const glm::vec3& _Value)
{
    glUniform3fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
}

template<> void Shader::set_uniform(const std::string& _Name, const glm::vec4& _Value)
{
    glUniform4fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
}

template<> void Shader::set_uniform(const std::string& _Name, const glm::mat2& _Value)
{
    glUniformMatrix2fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}

template<> void Shader::set_uniform(const std::string& _Name, const glm::mat3& _Value)
{
    glUniformMatrix3fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}

template<> void Shader::set_uniform(const std::string& _Name, const glm::mat4& _Value)
{
    glUniformMatrix4fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}