#pragma once

// Custom
#include <FrenchieAbstractShader.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        class AbstractShaderProgram
        {
            public:

            AbstractShaderProgram(unsigned int _ID) : m_ID(_ID){}
            virtual ~AbstractShaderProgram(){}

            bool link(const std::vector<std::shared_ptr<AbstractShader>>& _Shaders)
            {
                for(auto& shader : _Shaders)
                {
                    if(shader->compile()) 
                        attach_shader(shader);
                }

                return link_program();
            }

            virtual void use() = 0;
            virtual void unuse() = 0;

            // getters
            const unsigned int& get_id() const
            {
                return m_ID;
            }

            // setters
            virtual void set_bool(const std::string& _Name, bool _Value) const = 0;
            virtual void set_int(const std::string& _Name, int _Value) const = 0;
            virtual void set_float(const std::string& _Name, float _Value) const = 0;
            virtual void set_vec2(const std::string& _Name, const glm::vec2& _Value) const = 0;
            virtual void set_vec3(const std::string& _Name, const glm::vec3& _Value) const = 0;
            virtual void set_vec4(const std::string& _Name, const glm::vec4& _Value) const = 0;
            virtual void set_mat2(const std::string& _Name, const glm::mat2& _Value) const = 0;
            virtual void set_mat3(const std::string& _Name, const glm::mat3& _Value) const = 0;
            virtual void set_mat4(const std::string& _Name, const glm::mat4& _Value) const = 0;

            protected:
            
                const unsigned int m_ID = 0;

                virtual bool attach_shader(const std::shared_ptr<AbstractShader>&) = 0;
                virtual bool link_program() = 0;
        };
    }
}