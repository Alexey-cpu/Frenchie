#pragma once

// Custom
#include <FrenchieShaderProgram.hpp>
#include <FrenchieOpenGLShader.hpp>
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

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class GLShaderProgramInterface
            {
                public:
                typedef GLShaderInterface shader_type;
                typedef std::vector<std::shared_ptr<Shader<shader_type>>> shaders_type;

                GLShaderProgramInterface(unsigned int _ID, const shaders_type& _Shaders) : m_ID(_ID)
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
                
                ~GLShaderProgramInterface(){}

                void use()
                {
                    glUseProgram(get_id());
                }
                
                void unuse()
                {
                    glUseProgram(0);
                }

                const unsigned int& get_id() const
                {
                    return m_ID;
                }

                template<typename T>
                void set_uniform(const std::string& _Name, const T& _Value)
                {
                    // TODO: add logic here !!!
                }

                protected:

                unsigned int m_ID;
            };

            class GLShaderProgram : public ShaderProgram<GLShaderProgramInterface>
            {
                public:
                GLShaderProgram(const GLShaderProgramInterface::shaders_type& _Shaders) : ShaderProgram<GLShaderProgramInterface>(glCreateProgram(), _Shaders){}
                ~GLShaderProgram(){}
            };
        }
    }
}