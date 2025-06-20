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
        class AbstractShader
        {
            public:

                AbstractShader(unsigned int _ID) : m_ID(_ID){}
                virtual ~AbstractShader(){}
                virtual bool compile() = 0;

                const unsigned int& get_id() const
                {
                    return m_ID;
                }

            private:
                const unsigned int m_ID = 0;
        };
    }
}