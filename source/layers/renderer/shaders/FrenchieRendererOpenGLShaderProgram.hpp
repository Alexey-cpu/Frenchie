#pragma once

// Custom
#include <FrenchieRendererOpenGLShader.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class ShaderProgram
            {
            public:

                ShaderProgram(const std::vector<std::shared_ptr<Shader>>& _Shaders);
                virtual ~ShaderProgram();

                void begin();
                void end();

                const unsigned int& get_id() const;
                template<typename T> void set_uniform(const std::string& _Name, const T& _Value);

            protected:
                unsigned int m_ID;
            };
        }
    }
}