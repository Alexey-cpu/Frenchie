#pragma once

#include <FrenchieAbstractShader.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class VertexShader : public AbstractShader
            {
            public:
                VertexShader(const std::filesystem::path& _Path) : 
                    AbstractShader(glCreateShader(GL_VERTEX_SHADER)), m_Path(_Path)
                {
                    // load
                    std::ifstream ifstream(m_Path);
                    m_Source = std::string(
                        (std::istreambuf_iterator<char>(ifstream)), 
                        (std::istreambuf_iterator<char>()));

                    if(m_Source.empty())
                    {
                        Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPENGL::VERTEX_SHADER::LOAD_FAILED\n"));
                        Logger::instance()->error(fmt::format("{}\n", m_Path.string()));
                    }
                }

                virtual ~VertexShader()
                {
                    glDeleteShader(get_id());
                }
                
                virtual bool compile() override
                {
                    // compile
                    int status = 1;
                    const char* shaderSourceCode = m_Source.c_str();
                    glShaderSource(get_id(), 1, &shaderSourceCode, nullptr);
                    glCompileShader(get_id());
                    glGetShaderiv(get_id(), GL_COMPILE_STATUS, &status);

                    if(!status)
                    {
                        char infoLog[512];
                        glGetShaderInfoLog(get_id(), 512, nullptr, infoLog);
                        Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPENGL::VERTEX_SHADER::COMPILATION_FAILED\n"));
                        Logger::instance()->error(fmt::format("{}", std::string(infoLog)));
                        return false;
                    }

                    Logger::instance()->info(fmt::format("FRENCHIE::RENDERER::OPENGL::VERTEX_SHADER::COMPILATION_SUCCEEDED\n"));
                    return true;
                }

                protected:
                    std::string m_Source;
                    const std::filesystem::path m_Path;
            };
        }
    }
}