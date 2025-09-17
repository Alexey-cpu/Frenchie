#include <FrenchieRendererShader.hpp>

#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreLogger.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// STL
#include <fstream>
#include <iostream>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        class ShaderLoader
        {
        public:

            ShaderLoader(const unsigned int _ID, const std::filesystem::path& _Path) : m_ID(_ID)
            {
                Frenchie::Core::Logger::instance()->info("------------------------------------------------------------------------------");
                Frenchie::Core::Logger::instance()->info(fmt::format("FRENCHIE::RENDERER::SHADER"));
                Frenchie::Core::Logger::instance()->info("------------------------------------------------------------------------------");
                Frenchie::Core::Logger::instance()->info(fmt::format("Processing: {}", _Path.string()));

                // load
                std::ifstream ifsream(_Path);

                std::string source = 
                    std::string(
                        (std::istreambuf_iterator<char>(ifsream)), 
                        (std::istreambuf_iterator<char>()));

                if(source.empty())
                {
                    Frenchie::Core::Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::SHADER::LOAD_FAILED"));
                    return;
                }

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
                    Frenchie::Core::Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::SHADER::COMPILATION_FAILED"));
                    Frenchie::Core::Logger::instance()->error(fmt::format("{}", std::string(infoLog)));
                    return;
                }
            }

            virtual ~ShaderLoader()
            {
                if(get_id())
                    glDeleteShader(get_id());
            }

            const unsigned int& get_id() const
            {
                return m_ID;
            }

        private:
            const unsigned int m_ID;
        };
    }
}

using namespace Frenchie::Renderer;

// ShaderProgram
Shader::Shader(const std::filesystem::path& _Vertex, const std::filesystem::path& _Fragment) : 
    m_VertexShaderPath(_Vertex), 
    m_FragmentShaderPath(_Fragment){}

Shader::~Shader()
{
    if(get_id() != 0)
        glDeleteProgram(get_id());
}

bool Shader::instantiate()
{
    if(get_id() != 0) 
        return true;

    m_ID = glCreateProgram();
    glAttachShader(get_id(), ShaderLoader(glCreateShader(GL_VERTEX_SHADER), m_VertexShaderPath).get_id());
    glAttachShader(get_id(), ShaderLoader(glCreateShader(GL_FRAGMENT_SHADER), m_FragmentShaderPath).get_id());

    int success = true;
    glLinkProgram(get_id());
    glGetProgramiv(get_id(), GL_LINK_STATUS, &success);
    
    if (success)
        return true;

    char shaderLinkingLog[512];
    glGetProgramInfoLog(get_id(), 512, NULL, shaderLinkingLog);
    Frenchie::Core::Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::SHADER::LINK_FAILED\n"));
    Frenchie::Core::Logger::instance()->error(fmt::format("{}", std::string(shaderLinkingLog)));

    return false;
}

void Shader::use()
{
    glUseProgram(get_id());
}

void Shader::unuse()
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