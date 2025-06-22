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

                ShaderProgram(unsigned int _ID, const std::vector<std::shared_ptr<Shader>>& _Shaders);
                virtual ~ShaderProgram();

                void use();
                void unuse();

                const unsigned int& get_id() const;
                template<typename T> void set_uniform(const std::string& _Name, const T& _Value);

            protected:
                unsigned int m_ID;
            };
        }
    }
}