#include <FrenchieApplication.hpp>
#include <FrenchieCoreLogger.hpp>

// ImGUI
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <imgui.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace Frenchie::Application::OpenGL;

// callbacks
void OnWindowResize(GLFWwindow* _Window, int _Width, int _Height)
{
    (void)_Window;
    glViewport(0, 0, _Width, _Height);
}

void OnWindowMaximizedCallback(GLFWwindow* _Window, int _Maximized)
{
    int width  = 0;
    int height = 0;
    glfwGetWindowSize(_Window, &width, &height);
    glViewport(0, 0, width, height);
}

// Application
Application::Application()
{
    // initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // setup Window hints
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    if((m_Window = glfwCreateWindow(800, 600, m_Name.c_str(), nullptr, nullptr)) == nullptr)
    {
        glfwTerminate();
        Frenchie::Core::Logger::instance()->error(fmt::format(
            "FRENCHIE::OPENGL::APPLICATION::COULD_NOT_CREATE_WINDOW\n"));
        return;
    }

    glfwMakeContextCurrent(m_Window);

    // setup callbacks
    glfwSetWindowSizeCallback(m_Window, &OnWindowResize);
    glfwSetFramebufferSizeCallback(m_Window, &OnWindowResize);
    glfwSetWindowMaximizeCallback(m_Window, OnWindowMaximizedCallback);

    // custom callbackcs
    glfwSetCursorPosCallback(get_window(), Application::mouse_callback);

    // load OpenGL interface using GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        m_Window = nullptr;
        Frenchie::Core::Logger::instance()->error(fmt::format(
            "FRENCHIE::OPENGL::APPLICATION::COULD_NOT_LOAD_GLAD\n"));
    }

    //---------------------------------------------------------------------------------------------------
    // ImGui::awake
    //---------------------------------------------------------------------------------------------------
    #ifdef __APPLE__
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
    #else
        const char* glsl_version = "#version 130";
    #endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_NavEnableKeyboard |
            ImGuiConfigFlags_::ImGuiConfigFlags_NavEnableGamepad  |
            ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable     |
            ImGuiConfigFlags_::ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    io.IniFilename = nullptr; // disable automatic .ini file save

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    //---------------------------------------------------------------------------------------------------
}

Application::~Application()
{
    //---------------------------------------------------------------------------------------------------
    // ImGui::finish
    //---------------------------------------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    //---------------------------------------------------------------------------------------------------

    glfwDestroyWindow(m_Window);
    glfwTerminate();
    m_Layers.clear();
}

glm::u32vec2 Application::get_window_size() const
{
    int width  = 0;
    int height = 0;
    glfwGetWindowSize(m_Window, &width, &height);
    return glm::u32vec2(width, height);
}

GLFWwindow* Application::get_window() const
{
    return m_Window;
}

void Application::set_window_size(const glm::u32vec2& _Value)
{
    glfwSetWindowSize(m_Window, _Value.x, _Value.y);
}

void Application::set_maximized(const bool& _Value)
{
    glfwMaximizeWindow(m_Window);
}

bool Application::awake()
{
    if(m_Window == nullptr) 
        return false;

    // call window callbacks
    OnWindowMaximizedCallback(m_Window, glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED));

    return true;
}

void Application::Application::frame_start()
{
    // OpenGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();

    //---------------------------------------------------------------------------------------------------
    // ImGui::frame_start
    //---------------------------------------------------------------------------------------------------
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    //---------------------------------------------------------------------------------------------------

    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden()) 
            layer->frame_start();
    }
}

void Application::Application::frame_update()
{
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden())
            layer->frame_update();
    }
}

void Application::Application::frame_finish()
{
    //---------------------------------------------------------------------------------------------------
    // ImGui::frame_finish
    //---------------------------------------------------------------------------------------------------
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_Window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
        gladLoadGL();
    }
    //---------------------------------------------------------------------------------------------------

    for(auto layer : m_Layers)
    {
        if(!layer->is_hidden())
            layer->frame_finish();
    }

    glfwSwapBuffers(m_Window);
}

void Application::Application::finish()
{
    for(auto layer : m_Layers) 
        layer->finish();
    glfwTerminate();
}

std::string Application::get_name() const
{
    return m_Name;
}

void Application::set_name(const std::string& _Name)
{
    m_Name = _Name;
}

bool Application::is_closed()
{
    return !m_Opened || glfwWindowShouldClose(m_Window);
}

void Application::close()
{
    m_Opened = false;
}

int Application::execute()
{
    if(!awake()) 
        return -1;

    while (!is_closed())
    {
        // remove layers that are closed
        for(auto it = m_Layers.begin(); it != m_Layers.end(); it++)
        {
            if((*it)->is_closed())
            {
                (*it)->finish();
                auto rm = it;
                it++;
                m_Layers.erase(rm);

                if(it == m_Layers.end())
                    break;
            }
        }

        frame_start();
        frame_update();
        frame_finish();
    }

    finish();

    return 1;
}

void Application::load_state(std::filesystem::path _Path)
{
    m_State.read<Frenchie::Core::Serialization::XMLReader>(_Path);

    // load .ini file
    ImGui::LoadIniSettingsFromMemory(m_State.find_node(STRINGIFY(Application)).get_value());
}

void Application::save_state(std::filesystem::path _Path)
{
    // save state of only opened nodes
    m_State.reset();

    // append self
    auto application = m_State.append_node(STRINGIFY(Application), ImGui::SaveIniSettingsToMemory());

    for(auto it = begin(); it != end(); ++it)
    {
        auto serializer = 
            std::dynamic_pointer_cast<Frenchie::Core::Serialization::ISerializer>(*it);

        if(serializer != nullptr) 
            serializer->serialize(application);
    }

    m_State.write<Frenchie::Core::Serialization::XMLBeautifulWriter>(_Path);
}

Frenchie::Core::Serialization::Document& Application::get_state() const
{
    return m_State;
}

Application::const_iterator Application::begin() const
{
    return m_Layers.begin();
}

Application::const_iterator Application::end() const
{
    return m_Layers.end();
}

size_t Application::size() const
{
    return m_Layers.size();
}

void Application::mouse_callback(GLFWwindow* _Window, double _X, double _Y)
{
    for(auto&& layer : Frenchie::Application::Application::instance()->m_Layers)
    {
        auto mouseCallbackHandler = 
            dynamic_cast<IMouseCallbackHandler*>(layer.get());

        if(mouseCallbackHandler != nullptr) 
            mouseCallbackHandler->mouse_callback(_Window, _X, _Y);
    }
}