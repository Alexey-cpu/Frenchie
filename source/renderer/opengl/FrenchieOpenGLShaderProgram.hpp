#pragma once

// Custom
#include <FrenchieAbstractShaderProgram.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class ShaderProgram : public AbstractShaderProgram
            {
                public:

                ShaderProgram() : AbstractShaderProgram(glCreateProgram()){}
                
                virtual ~ShaderProgram()
                {
                    glDeleteProgram(get_id());
                }
                
                virtual void use() override
                {
                    glUseProgram(get_id());
                }

                virtual void unuse() override
                {
                    glUseProgram(0);
                }

                // setters
                virtual void set_bool(const std::string& _Name, bool _Value) const override
                {
                    glUniform1i(glGetUniformLocation(get_id(), _Name.c_str()), (int)_Value); 
                }
                
                virtual void set_int(const std::string& _Name, int _Value) const override
                {
                    glUniform1i(glGetUniformLocation(get_id(), _Name.c_str()), _Value);
                }
                
                virtual void set_float(const std::string& _Name, float _Value) const override
                {
                    glUniform1f(glGetUniformLocation(get_id(), _Name.c_str()), _Value);
                }
                
                virtual void set_vec2(const std::string& _Name, const glm::vec2& _Value) const override
                {
                    glUniform2fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]); 
                }
                
                virtual void set_vec3(const std::string& _Name, const glm::vec3& _Value) const override
                {
                    glUniform3fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
                }
                
                virtual void set_vec4(const std::string& _Name, const glm::vec4& _Value) const override
                {
                    glUniform4fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, &_Value[0]);
                }
                
                virtual void set_mat2(const std::string& _Name, const glm::mat2& _Value) const override
                {
                    glUniformMatrix2fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
                }
                
                virtual void set_mat3(const std::string& _Name, const glm::mat3& _Value) const override
                {
                    glUniformMatrix3fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
                }
                
                virtual void set_mat4(const std::string& _Name, const glm::mat4& _Value) const override
                {
                    glUniformMatrix4fv(glGetUniformLocation(get_id(), _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
                }

                protected:

                virtual bool attach_shader(const std::shared_ptr<AbstractShader>& _Shader) override
                {
                    if(_Shader == nullptr) 
                        return false;
                    
                    glAttachShader(get_id(), _Shader->get_id());
                    return true;
                }
                
                virtual bool link_program() override
                {
                    // link
                    glLinkProgram(get_id());

                    // check for linking errors
                    int success = true;
                    glGetProgramiv(get_id(), GL_LINK_STATUS, &success);
                    
                    if (!success) 
                    {
                        char infoLog[512];
                        glGetProgramInfoLog(get_id(), 512, NULL, infoLog);
                        Logger::instance()->error(fmt::format("FRENCHIE::RENDERER::OPENGL::SHADER_PROGRAM::LINK_FAILED\n"));
                        Logger::instance()->error(fmt::format("{}\n", std::string(infoLog)));
                        return false;
                    }

                    Logger::instance()->info(fmt::format("FRENCHIE::RENDERER::OPENGL::SHADER_PROGRAM::LINK_SUCCEEDED\n"));

                    return true;
                }
            };
        }
    }
}