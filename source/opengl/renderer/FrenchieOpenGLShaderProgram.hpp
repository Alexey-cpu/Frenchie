#pragma once

// Custom
#include <FrenchieOpenGLShader.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class ShaderProgram
            {
            public:

                typedef std::vector<std::shared_ptr<Shader>> shaders;

                ShaderProgram(unsigned int _ID, const shaders& _Shaders);
                virtual ~ShaderProgram();

                void use();
                void unuse();

                // getters
                const unsigned int& get_id() const;

                // setters
                template<typename T> void set_uniform(const std::string& _Name, const T& _Value);

            protected:
                unsigned int m_ID;
            };
        }
    }
}