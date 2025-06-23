// Custom
#include <FrenchieOpenGLApplication.hpp>
#include <FrenchieLogger.hpp>

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

    Frenchie::Core::Logger::instance()->info("OnWindowResize");
}

void OnWindowMaximizedCallback(GLFWwindow* _Window, int _Maximized)
{
    int width  = 0;
    int height = 0;
    glfwGetWindowSize(_Window, &width, &height);
    glViewport(0, 0, width, height);

    Frenchie::Core::Logger::instance()->info("OnWindowMaximizedCallback");
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
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";
#endif

    // setup Window hints
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    if((m_MainWindow = glfwCreateWindow(800, 600, m_Name.c_str(), nullptr, nullptr)) == nullptr)
    {
        glfwTerminate();
        Frenchie::Core::Logger::instance()->error(fmt::format(
            "FRENCHIE::OPENGL::APPLICATION::COULD_NOT_CREATE_WINDOW\n"));
        return;
    }

    glfwMakeContextCurrent(m_MainWindow);

    // setup callbacks
    glfwSetWindowSizeCallback(m_MainWindow, &OnWindowResize);
    glfwSetFramebufferSizeCallback(m_MainWindow, &OnWindowResize);
    glfwSetWindowMaximizeCallback(m_MainWindow, OnWindowMaximizedCallback);

    // load OpenGL interface using GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        m_MainWindow = nullptr;
        Frenchie::Core::Logger::instance()->error(fmt::format(
            "FRENCHIE::OPENGL::APPLICATION::COULD_NOT_LOAD_GLAD\n"));
    }

    //---------------------------------------------------------------------------------------------------
    // ImGui::awake
    //---------------------------------------------------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_NavEnableKeyboard |
            ImGuiConfigFlags_::ImGuiConfigFlags_NavEnableGamepad  |
            ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable     |
            ImGuiConfigFlags_::ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(m_MainWindow, true);
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

    glfwDestroyWindow(m_MainWindow);
    glfwTerminate();
    m_Layers.clear();
}

glm::u32vec2 Application::get_window_size() const
{
    int width  = 0;
    int height = 0;
    glfwGetWindowSize(m_MainWindow, &width, &height);
    return glm::u32vec2(width, height);
}

void Application::set_window_size(const glm::u32vec2& _Value)
{
    glfwSetWindowSize(m_MainWindow, _Value.x, _Value.y);
}

void Application::set_maximized(const bool& _Value)
{
    glfwMaximizeWindow(m_MainWindow);
}

bool Application::awake()
{
    if(m_MainWindow == nullptr) 
        return false;

    // call window callbacks
    OnWindowMaximizedCallback(m_MainWindow, glfwGetWindowAttrib(m_MainWindow, GLFW_MAXIMIZED));

    // awake layers
    for(auto it = m_Layers.begin(); it != m_Layers.end(); it++)
    {
        if(!(*it)->awake())
        {
            (*it)->finish();
            auto rm = it;
            it++;
            m_Layers.erase(rm);

            Frenchie::Core::Logger::instance()->error(fmt::format(
                "FRENCHIE::OPENGL::APPLICATION::COULD_NOT_AWAKE_LAYER {}\n", 
                (*it)->get_name()));

            if(it == m_Layers.end())
                break;
        }
    }

    return true;
}

void Application::Application::frame_start()
{
    // OpenGL
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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
        layer->frame_start();
}

void Application::Application::frame_update()
{
    for(auto layer : m_Layers) 
        layer->frame_update();
}

void Application::Application::frame_finish()
{
    //---------------------------------------------------------------------------------------------------
    // ImGui::frame_finish
    //---------------------------------------------------------------------------------------------------
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_MainWindow, &display_w, &display_h);
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
        layer->frame_finish();

    glfwSwapBuffers(m_MainWindow);
}

void Application::Application::finish()
{
    for(auto layer : m_Layers) 
        layer->finish();
    glfwTerminate();
}

bool Application::Application::is_closed()
{
    return m_Closed || glfwWindowShouldClose(m_MainWindow);
}