#pragma once

#include <FrenchieShader.hpp>
#include <FrenchieLogger.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class GLShaderInterface
            {
                public:

                GLShaderInterface(const unsigned int& _ID, const std::filesystem::path& _Path) : m_ID(_ID)
                {
                    // load
                    std::ifstream ifstream(_Path);
                    std::string source = std::string(
                        (std::istreambuf_iterator<char>(ifstream)), 
                        (std::istreambuf_iterator<char>()));

                    if(source.empty())
                    {
                        Frenchie::Core::Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPENGL::SHADER::LOAD_FAILED\n"));
                        Frenchie::Core::Logger::instance()->error(fmt::format("{}\n", _Path.string()));
                        return;
                    }
                    else
                    {
                        Frenchie::Core::Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPENGL::SHADER::LOAD_SUCCEEDED\n"));
                        Frenchie::Core::Logger::instance()->error(fmt::format("{}\n", _Path.string()));
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
                        Frenchie::Core::Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPENGL::SHADER::COMPILATION_FAILED\n"));
                        Frenchie::Core::Logger::instance()->error(fmt::format("{}", std::string(infoLog)));
                    }
                    else
                    {
                        Frenchie::Core::Logger::instance()->info(fmt::format("FRENCHIE::RENDERER::OPENGL::SHADER::COMPILATION_SUCCEEDED\n"));
                    }
                }
                
                ~GLShaderInterface()
                {
                    glDeleteShader(get_id());
                }

                const unsigned int& get_id() const
                {
                    return m_ID;
                }

                private:
                    unsigned int m_ID;
            };

            class GLVertexShader : public Shader<GLShaderInterface>
            {
            public:
                GLVertexShader(const std::filesystem::path& _Path) :  Shader<GLShaderInterface>(glCreateShader(GL_VERTEX_SHADER), _Path){}
                ~GLVertexShader(){}
            };

            class GLFragmentShader : public Shader<GLShaderInterface>
            {
            public:
                GLFragmentShader(const std::filesystem::path& _Path) : Shader<GLShaderInterface>(glCreateShader(GL_FRAGMENT_SHADER), _Path){}
                ~GLFragmentShader(){}
            };
        }
    }
}