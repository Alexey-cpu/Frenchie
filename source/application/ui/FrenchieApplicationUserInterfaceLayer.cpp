#include <FrenchieApplicationUserInterfaceLayer.hpp>

// Application
#include <FrenchieApplication.hpp>

// IMGUI
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

using namespace Frenchie::Application;

Interface::Interface(){}
Interface::~Interface(){}

bool Interface::awake()
{
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

    ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(Frenchie::Application::application_platform_backend_renderer()->get_context()), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void Interface::frame_start()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
}

void Interface::frame_update()
{
}

void Interface::frame_render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
        gladLoadGL();
    }
}

void Interface::frame_finish()
{
}

void Interface::finish()
{
}

void Interface::quit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool Interface::allows_multiple_instances() const
{
    return false;
}

void Interface::reload()
{
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_CreateDeviceObjects();
}