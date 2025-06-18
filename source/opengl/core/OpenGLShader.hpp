#pragma once

// Custom
#include <OpenGLObject.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// STL
#include <filesystem>
#include <iostream>
#include <fstream>
#include <memory>

// FMT
#include <fmt/format.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

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
            glShaderSource(get_adress(), 1, &vertexShaderSource, NULL);

            // compile
            glCompileShader(get_adress());
            glGetShaderiv(get_adress(), GL_COMPILE_STATUS, &m_Status);

            if(!m_Status)
            {
                char infoLog[512];
                glGetShaderInfoLog(get_adress(), 512, NULL, infoLog);
                m_StatusMessage.append(fmt::format("OpenGL::Shader::COMPILATION_FAILED\n{0}", infoLog));
            }
            else 
            {
                m_StatusMessage.append(fmt::format("OpenGL::Shader::COMPILATION_SUCCEEDED\n"));
            }
        }

        virtual ~Shader()
        {
            glDeleteShader(get_adress());
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
                    glAttachShader(get_adress(), shader->get_adress());

                m_StatusMessage.append(shader->get_status_message());
            }

            // link program
            glLinkProgram(get_adress());
            glGetProgramiv(get_adress(), GL_LINK_STATUS, &m_Status);

            if(!m_Status)
            {
                char infoLog[512];
                glGetProgramInfoLog(get_adress(), 512, NULL, infoLog);
                m_StatusMessage.append(fmt::format("OpenGL::Material::LINK_FAILED\n{0}", infoLog));
            }
            else 
            {
                m_StatusMessage.append(fmt::format("OpenGL::Material::LINK_SUCCEEDED\n"));
            }
        }

        virtual ~ShaderProgram()
        {
            glDeleteProgram(get_adress());
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

        void set_bool(const std::string &name, bool value) const
        {         
            glUniform1i(glGetUniformLocation(get_adress(), name.c_str()), (int)value); 
        }

        void set_int(const std::string &name, int value) const
        { 
            glUniform1i(glGetUniformLocation(get_adress(), name.c_str()), value); 
        }

        void set_float(const std::string &name, float value) const
        { 
            glUniform1f(glGetUniformLocation(get_adress(), name.c_str()), value); 
        }

        void set_vec2(const std::string &name, const glm::vec2 &value) const
        { 
            glUniform2fv(glGetUniformLocation(get_adress(), name.c_str()), 1, &value[0]); 
        }

        void set_vec3(const std::string &name, const glm::vec3 &value) const
        { 
            glUniform3fv(glGetUniformLocation(get_adress(), name.c_str()), 1, &value[0]); 
        }

        void set_vec4(const std::string &name, const glm::vec4 &value) const
        { 
            glUniform4fv(glGetUniformLocation(get_adress(), name.c_str()), 1, &value[0]); 
        }

        void set_mat2(const std::string &name, const glm::mat2 &mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(get_adress(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        void set_mat3(const std::string &name, const glm::mat3 &mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(get_adress(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        void set_mat4(const std::string &name, const glm::mat4 &mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(get_adress(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

    protected:

        int          m_Status        = 0;
        std::string  m_StatusMessage = std::string();
    };
};