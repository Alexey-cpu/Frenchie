#include <FrenchieRendererOpenGLShader.hpp>
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
Shader::Shader(const unsigned int& _ID, const std::filesystem::path& _Path) : m_ID(_ID)
{
    Frenchie::Core::Logger::instance()->info("FRENCHIE::RENDERER::OPENGL::SHADER::PROCESSING_SHADER");
    Frenchie::Core::Logger::instance()->info(fmt::format("\t{}", _Path.string()));

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

Shader::~Shader()
{
    glDeleteShader(get_id());
}

const unsigned int& Shader::get_id() const
{
    return m_ID;
}

// GLVertexShader
VertexShader::VertexShader(const std::filesystem::path& _Path) : Shader(glCreateShader(GL_VERTEX_SHADER), _Path){}
VertexShader::~VertexShader(){}

// GLFragmentShader
FragmentShader::FragmentShader(const std::filesystem::path& _Path) : Shader(glCreateShader(GL_FRAGMENT_SHADER), _Path){}
FragmentShader::~FragmentShader(){}

// ShaderProgram
ShaderProgram::ShaderProgram(const std::vector<std::shared_ptr<Shader>>& _Shaders) : 
    m_ID(glCreateProgram())
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

void ShaderProgram::begin()
{
    glUseProgram(get_id());
}

void ShaderProgram::end()
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