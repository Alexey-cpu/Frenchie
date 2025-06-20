#pragma once

// Custom
#include <FrenchieObject.hpp>
#include <FrenchieLogger.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// FMT
#include <fmt/format.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        template<typename... Args>
        class Shader
        {
        public:
            Shader(){}
            virtual ~Shader(){}
        };

        template<typename IShader, typename... Args>
        class Shader<IShader, Args...> : public Object, Shader<Args...>
        {
            public:

                Shader(const std::filesystem::path& _Path) :
                    Object(static_cast<IShader*>(this)->construct()), 
                    m_Shader(static_cast<IShader*>(this))
                {
                    if(!m_Shader->load(_Path))
                    {
                        Logger::error(fmt::format("FRENCHIE::SHADER::ERROR::COULD_NOT_LOAD\n"));
                        Logger::error(fmt::format("{}\n", _Path.wstring()));
                        Logger::error(fmt::format("{}\n", m_Shader->message()));
                        return;
                    }

                    if(!m_Shader->compile(get_id()))
                    {
                        Logger::error(fmt::format("FRENCHIE::SHADER::ERROR::COULD_NOT_COMPILE\n"));
                        Logger::error(fmt::format("{}\n", _Path.wstring()));
                        Logger::error(fmt::format("{}\n", m_Shader->message()));
                    }
                }

                virtual ~Shader()
                {
                    m_Shader->destroy(get_id());
                }

                std::string get_source_code() const
                {
                    return m_Shader->get_source_code();
                }

            private:
                const IShader* m_Shader = nullptr;
        };

        template<class IShaderProgram>
        class ShaderProgram : public Object
        {
        public:

            ShaderProgram(std::vector<std::shared_ptr<Object>> _Shaders) :
                Object(static_cast<IShaderProgram*>(this)->construct()), 
                m_ShaderProgram(static_cast<IShaderProgram*>(this))
            {
                for(auto shader : _Shaders)
                {
                    if(dynamic_cast<Shader<>*>(shader)) 
                        m_ShaderProgram->attach(shader);
                }
            }

            virtual ~ShaderProgram()
            {
                m_ShaderProgram->destroy(get_id());
            }

            // setters
            void set_bool(const std::string& _Name, bool _Value) const
            {
                m_ShaderProgram->set_bool(_Name, _Value);
            }

            void set_int(const std::string& _Name, int _Value) const
            {
                m_ShaderProgram->set_int(_Name, _Value);
            }

            void set_float(const std::string& _Name, float _Value) const
            {
                m_ShaderProgram->set_float(_Name, _Value);
            }

            void set_vec2(const std::string& _Name, const glm::vec2& _Value) const
            {
                m_ShaderProgram->set_vec2(_Name, _Value);
            }

            void set_vec3(const std::string& _Name, const glm::vec3& _Value) const
            {
                m_ShaderProgram->set_vec3(_Name, _Value);
            }

            void set_vec4(const std::string& _Name, const glm::vec4& _Value) const
            {
                m_ShaderProgram->set_vec4(_Name, _Value);
            }

            void set_mat2(const std::string& _Name, const glm::mat2& _Value) const
            {
                m_ShaderProgram->set_mat2(_Name, _Value);
            }

            void set_mat3(const std::string& _Name, const glm::mat3& _Value) const
            {
                m_ShaderProgram->set_mat3(_Name, _Value);
            }

            void set_mat4(const std::string& _Name, const glm::mat4& _Value) const
            {
                m_ShaderProgram->set_mat4(_Name, _Value);
            }

            void execute()
            {
                m_ShaderProgram->execute();
            }

        private:
            const IShaderProgram* m_ShaderProgram = nullptr;
        };
    }
}

namespace Frenchie
{
    namespace OpenGL
    {
        class Shader : public Object
        {
        public:
            
            Shader(unsigned int _Type, const std::filesystem::path& _Path) : 
                Object(glCreateShader(_Type)), m_Type(_Type)
            {
                std::string shader = load(std::filesystem::path(_Path));

                if(shader.empty())
                {
                    m_Status = 0;
                    m_StatusMessage.append(fmt::format("OpenGL::Shader::LOAD_FAILED\n"));
                    return;
                }
                else
                {
                    m_StatusMessage.append(fmt::format("OpenGL::Shader::LOAD_SUCCEEDED\n"));
                }

                const char* vertexShaderSource = shader.c_str();
                glShaderSource(get_id(), 1, &vertexShaderSource, NULL);

                // compile
                glCompileShader(get_id());
                glGetShaderiv(get_id(), GL_COMPILE_STATUS, &m_Status);

                if(!m_Status)
                {
                    char infoLog[512];
                    glGetShaderInfoLog(get_id(), 512, NULL, infoLog);
                    m_StatusMessage.append(fmt::format("OpenGL::Shader::COMPILATION_FAILED\n{0}", infoLog));
                }
                else 
                {
                    m_StatusMessage.append(fmt::format("OpenGL::Shader::COMPILATION_SUCCEEDED\n"));
                }
            }

            virtual ~Shader()
            {
                glDeleteShader(get_id());
            }

            // getters
            int get_status() const
            {
                return m_Status;
            }

            std::string get_status_message() const
            {
                return m_StatusMessage;
            }

        protected:

            int          m_Type          = 0;
            int          m_Status        = 0;
            std::string  m_StatusMessage = std::string();

            // service methods
            std::string load(const std::filesystem::path& _Path)
            {
                std::ifstream ifs(_Path);
                return std::string((std::istreambuf_iterator<char>(ifs)),
                            (std::istreambuf_iterator<char>()));
            }
        };

        class VertexShader : public Shader
        {
        public:
            VertexShader(const std::filesystem::path& _Path) : 
                Shader(GL_VERTEX_SHADER, _Path){}

            virtual ~VertexShader(){}
        };

        class FragmetShader : public Shader
        {
        public:
            
            FragmetShader(const std::filesystem::path& _Path) : 
                Shader(GL_FRAGMENT_SHADER, _Path){}

            virtual ~FragmetShader(){}
        };

        class ShaderProgram : public Object
        {
        public:

            ShaderProgram(std::vector<std::shared_ptr<Shader>> _Shaders) : 
                Object(glCreateProgram())
            {
                // attach successfully loaded and compiled shaders
                for(auto& shader : _Shaders)
                {
                    if(shader->get_status())
                        glAttachShader(get_id(), shader->get_id());

                    m_StatusMessage.append(shader->get_status_message());
                }

                // link program
                glLinkProgram(get_id());
                glGetProgramiv(get_id(), GL_LINK_STATUS, &m_Status);

                if(!m_Status)
                {
                    char infoLog[512];
                    glGetProgramInfoLog(get_id(), 512, NULL, infoLog);
                    m_StatusMessage.append(fmt::format("OpenGL::Material::LINK_FAILED\n{0}", infoLog));
                }
                else 
                {
                    m_StatusMessage.append(fmt::format("OpenGL::Material::LINK_SUCCEEDED\n"));
                }
            }

            virtual ~ShaderProgram()
            {
                glDeleteProgram(get_id());
            }

            // getters
            int get_status() const
            {
                return m_Status;
            }

            std::string get_status_message() const
            {
                return m_StatusMessage;
            }

            void set_bool(const std::string &name, bool _Value) const
            {         
                glUniform1i(glGetUniformLocation(get_id(), name.c_str()), (int)_Value); 
            }

            void set_int(const std::string &name, int _Value) const
            { 
                glUniform1i(glGetUniformLocation(get_id(), name.c_str()), _Value); 
            }

            void set_float(const std::string &name, float _Value) const
            { 
                glUniform1f(glGetUniformLocation(get_id(), name.c_str()), _Value); 
            }

            void set_vec2(const std::string &name, const glm::vec2 &_Value) const
            { 
                glUniform2fv(glGetUniformLocation(get_id(), name.c_str()), 1, &_Value[0]); 
            }

            void set_vec3(const std::string &name, const glm::vec3 &_Value) const
            { 
                glUniform3fv(glGetUniformLocation(get_id(), name.c_str()), 1, &_Value[0]); 
            }

            void set_vec4(const std::string &name, const glm::vec4 &_Value) const
            { 
                glUniform4fv(glGetUniformLocation(get_id(), name.c_str()), 1, &_Value[0]); 
            }

            void set_mat2(const std::string &name, const glm::mat2 &mat) const
            {
                glUniformMatrix2fv(glGetUniformLocation(get_id(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
            }

            void set_mat3(const std::string &name, const glm::mat3 &mat) const
            {
                glUniformMatrix3fv(glGetUniformLocation(get_id(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
            }

            void set_mat4(const std::string &name, const glm::mat4 &mat) const
            {
                glUniformMatrix4fv(glGetUniformLocation(get_id(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
            }

            void use()
            {
                glUseProgram(get_id());
            }

        protected:

            int          m_Status        = 0;
            unsigned int m_Adress        = 0;
            std::string  m_StatusMessage = std::string();
        };
    }
}