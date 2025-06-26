#pragma once

#include <FrenchieApplicationLayer.hpp>

#include <FrenchieRendererScene3D.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace Frenchie
{
    namespace Application
    {
        using namespace Frenchie::Renderer;

        class SceneSreenshooter
        {
        public:
            SceneSreenshooter(){}
            ~SceneSreenshooter()
            {
                glDeleteBuffers(1, &framebuffer);
                glDeleteBuffers(1, &rbo);
                glDeleteTextures(1, &textureColorbuffer);
            }

            bool awake()
            {
                int SCR_WIDTH = 2048;
                int SCR_HEIGHT = 1024;

                // create frame buffer
                framebuffer;
                glGenFramebuffers(1, &framebuffer);
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
                
                // create a color attachment texture
                textureColorbuffer;
                glGenTextures(1, &textureColorbuffer);
                glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
                
                // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
                rbo;
                glGenRenderbuffers(1, &rbo);
                glBindRenderbuffer(GL_RENDERBUFFER, rbo);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
                
                // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
                    return false;
                }
                
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                return true;
            }

            unsigned int framebuffer;
            unsigned int textureColorbuffer;
            unsigned int rbo;
        };

        class Scene : public Layer
        {
        public:
            Scene(const std::string& _Name);
            virtual ~Scene();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void close() override;
            virtual bool is_closed() override;

        protected:

            std::shared_ptr<Scene3D> m_Viewport = nullptr;

            SceneSreenshooter m_SceneScreenshooter;
        };
    }
}