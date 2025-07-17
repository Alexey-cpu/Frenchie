#include <FrenchieRendererScreenShoterComponent.hpp>
#include <FrenchieRendererSizeComponent.hpp>

#include <FrenchieCoreLogger.hpp>

using namespace Frenchie::Renderer;
using namespace Frenchie::Core;

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// IMGUI
#include <imgui.h>
#include <imgui_internal.h>

ScreenShoter::ScreenShoter() : 
    Core::Component::Registry<ScreenShoter>(STRINGIFY(ScreenShoter)){}

ScreenShoter::~ScreenShoter()
{
    if(m_Framebuffer != 0)
        glDeleteBuffers(1, &m_Framebuffer);

    if(m_TextureDepthBuffer != 0)
        glDeleteBuffers(1, &m_TextureDepthBuffer);
    
    if(m_TextureColorBuffer != 0)
        glDeleteTextures(1, &m_TextureColorBuffer);
}

unsigned int ScreenShoter::get_texture() const
{
    return m_TextureColorBuffer;
}

bool ScreenShoter::awake()
{
    int width  = 2048;
    int height = 1024;

    // create frame buffer
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    
    // create a color attachment texture
    glGenTextures(1, &m_TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);
    
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &m_TextureDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_TextureDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_TextureDepthBuffer); // now actually attach it
    
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Logger::instance()->error("FRENCHIE::APPLICATION::OBJECT_SCREEN_SHOTER::AWAKE_FAILED");
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void ScreenShoter::frame_start(){}

void ScreenShoter::frame_update(){}

void ScreenShoter::frame_finish(){}

void ScreenShoter::draw_editor()
{
    ImGui::PushID("Color picker");
    ImVec4 color(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    
    ImGui::ColorPicker3(
        "Clear color", 
        (float*)&color, 
        ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueBar  |
        ImGuiColorEditFlags_::ImGuiColorEditFlags_NoSidePreview |
        ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs      |
        ImGuiColorEditFlags_::ImGuiColorEditFlags_NoAlpha
    );
    
    ImGui::ColorEdit4(
        "RGB",
        (float*)&color, 
        ImGuiColorEditFlags_::ImGuiColorEditFlags_DisplayRGB |
        ImGuiColorEditFlags_::ImGuiColorEditFlags_InputHSV   |
        ImGuiColorEditFlags_::ImGuiColorEditFlags_Uint8);

    m_ClearColor = glm::vec4(color.x, color.y, color.z, color.w);

    ImGui::PopID();
}

void ScreenShoter::render()
{
    if(get_object() == nullptr) 
        return;

    auto size    = get_object()->get_component<Size>();
    float width  = size != nullptr ? size->get_size().x : 2048.f;
    float height = size != nullptr ? size->get_size().y : 1024.f;

    // resize frame buffer
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)width, (int)height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, m_TextureDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)width, (int)height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_TextureDepthBuffer);

    // bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    
    // depth/stencil/blending
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0, 0, (int)width, (int)height);

    if(is_enabled())
        get_object()->frame_finish();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_FRAMEBUFFER, 0);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
}