#include <FrenchieApplicationPlatformLayer.hpp>

// Core
#include <FrenchieCoreFileSystem.hpp>
#include <FrenchieCoreMemoryCache.hpp>
#include <FrenchieCoreStringUnicode.hpp>

// Application
#include <FrenchieApplication.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// STB
#include <stb_image.h>
#include <stb_image_write.h>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Memory;
using namespace Frenchie::Core::String;
using namespace Frenchie::Core::FileSystem;

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        const std::set<std::string> RENDERER_SUPPORTED_IMAGE_FORMATS =
            std::set<std::string>({".png", ".jpg", ".tga", ".bmp"});
    }
}

Platform::Platform() :
    m_Backend(std::make_unique<PlatformBackendInstance>()){}

Platform::~Platform(){}

std::unique_ptr<PlatformBackendInstance>& Platform::get_backend() const
{
    return m_Backend;
}

bool Platform::awake()
{
    return m_Backend->awake(
        m_Name.c_str(),
        nullptr,
        PlatformBackendContextWindowHints_::PlatformBackendContextWindowHints_Visible   |
        PlatformBackendContextWindowHints_::PlatformBackendContextWindowHints_Decorated |
        PlatformBackendContextWindowHints_::PlatformBackendContextWindowHints_Resizable |
        PlatformBackendContextWindowHints_::PlatformBackendContextWindowHints_Focused
);;
}

void Platform::frame_start()
{
    m_Backend->frame_start(
        PlatformBackendRendererHints_::PlatformBackendRendererHints_ClearColorBuffer   |
        PlatformBackendRendererHints_::PlatformBackendRendererHints_ClearDepthBuffer   |
        PlatformBackendRendererHints_::PlatformBackendRendererHints_ClearStencilBuffer |
        PlatformBackendRendererHints_::PlatformBackendRendererHints_PollEvents);
}

void Platform::frame_update()
{
    m_Backend->frame_update();

    if(m_Backend->is_closed())
        Frenchie::Application::application()->close();
}

void Platform::frame_render()
{
    m_Backend->frame_render();
}

void Platform::frame_finish()
{
    m_Backend->frame_finish();
}

void Platform::finish()
{
    m_Backend->finish();
}

void Platform::quit()
{
    m_Backend->quit();
}

bool Platform::allows_multiple_instances() const
{
    return false;
}