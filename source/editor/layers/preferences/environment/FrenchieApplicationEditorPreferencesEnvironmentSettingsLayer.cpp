#include <FrenchieApplicationEditorPreferencesEnvironmentSettingsLayer.hpp>

// Frenchie::Application::Editor
#include <FrenchieApplicationEditorLauncher.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

EnvironmentSettings::EnvironmentSettings() : 
    Frenchie::Application::Layer::Registry<EnvironmentSettings>(STRINGIFY(EnvironmentSettings)){}

EnvironmentSettings::~EnvironmentSettings(){}

std::string EnvironmentSettings::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::Preferences), STRINGIFY(EnvironmentSettings));
}

void EnvironmentSettings::frame_update() 
{
    ImGui::BeginChild(get_name().c_str());
    {
        // draw application folders
        if(ImGui::TreeNode("Applciation folders"))
        {
            draw_editor_folders();
            ImGui::TreePop();
        }

        if(ImGui::TreeNode("System PATH variable contents"))
        {
            draw_system_path_variable();
            ImGui::TreePop();
        }
    }

    ImGui::EndChild();
}

bool EnvironmentSettings::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    return true;
}

bool EnvironmentSettings::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    return true;
}

void EnvironmentSettings::draw_editor_folders()
{
    bool selected = false;

    ImGui::Selectable(
        Frenchie::Core::Helpers::String::as_utf8(Launcher::get_app_exe_directory().wstring()).c_str(), &selected);
    ImGui::SetItemTooltip("Directory where app .exe file resides.");

    ImGui::Selectable(
        Frenchie::Core::Helpers::String::as_utf8(Launcher::get_app_log_directory().wstring()).c_str(), &selected);
    ImGui::SetItemTooltip("Directory where app stores it's log files.");

    ImGui::Selectable(
        Frenchie::Core::Helpers::String::as_utf8(Launcher::get_app_state_directory().wstring()).c_str(), &selected);
    ImGui::SetItemTooltip("Directory where app stores it's state serialization file.");

    ImGui::Selectable(
        Frenchie::Core::Helpers::String::as_utf8(Launcher::get_app_console_directory().wstring()).c_str(), &selected);
    ImGui::SetItemTooltip("Directory where app stores logs written by third party tools (compilers, build systems e.t.c).");
}

void EnvironmentSettings::draw_system_path_variable()
{
    if(ImGui::Button("Reload") || m_PATH.empty())
        m_PATH = Launcher::get_system_path_variable("PATH");

    ImGui::SameLine();
    
    if(ImGui::Button("Edit"))
    {
        // TODO: add logic here
    }

#ifdef _WIN32
    auto paths = Frenchie::Core::Helpers::String::split(m_PATH, ";");
#else
    auto paths = Frenchie::Core::Helpers::String::split(m_PATH, ":");
#endif

    bool selected = false;
    for(auto&& path : paths)
        ImGui::Selectable(path.c_str(), &selected);
}

void EnvironmentSettings::draw_system_lib_variable()
{
#ifdef _WIN32
    if(ImGui::Button("Reload") || m_LIB.empty())
        m_LIB = Launcher::get_system_path_variable("LIB");
#else
    if(ImGui::Button("Reload") || m_LIB.empty())
        m_LIB = Launcher::get_system_path_variable("LD_LIBRARY_PATH");
#endif

    ImGui::SameLine();
    
    if(ImGui::Button("Edit"))
    {
        // TODO: add logic here
    }

#ifdef _WIN32
    auto paths = Frenchie::Core::Helpers::String::split(m_LIB, ";");
#else
    auto paths = Frenchie::Core::Helpers::String::split(m_LIB, ":");
#endif

    bool selected = false;
    for(auto&& path : paths)
        ImGui::Selectable(path.c_str(), &selected);
}

void EnvironmentSettings::draw_system_include_variable()
{
    if(ImGui::Button("Reload") || m_PATH.empty())
        m_PATH = Launcher::get_system_path_variable("PATH");

    ImGui::SameLine();
    
    if(ImGui::Button("Edit"))
    {
        // TODO: add logic here
    }

#ifdef _WIN32
    auto paths = Frenchie::Core::Helpers::String::split(m_PATH, ";");
#else
    auto paths = Frenchie::Core::Helpers::String::split(m_PATH, ":");
#endif

    bool selected = false;
    for(auto&& path : paths)
        ImGui::Selectable(path.c_str(), &selected);
}