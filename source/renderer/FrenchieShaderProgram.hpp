#pragma once

// Custom
#include <FrenchieShader.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        template<typename IShaderProgram>
        class ShaderProgram
        {
            public:

            ShaderProgram(
                unsigned int _ID, 
                const std::vector<std::shared_ptr<Shader<IShaderProgram::template shader_type>>>& _Shaders) : m_IShaderProgram(IShaderProgram(_ID, _Shaders)){}
            
            ~ShaderProgram(){}

            void use()
            {
                m_IShaderProgram.use();
            }
            
            void unuse()
            {
                m_IShaderProgram.unuse();
            }

            // getters
            const unsigned int& get_id() const
            {
                return m_IShaderProgram->get_id();
            }

            // setters
            template<typename T>
            void set_uniform(const std::string& _Name, const T& _Value)
            {
                m_IShaderProgram->set<T>(_Name,  _Value);
            }

            protected:
                IShaderProgram m_IShaderProgram;
        };
    }
}