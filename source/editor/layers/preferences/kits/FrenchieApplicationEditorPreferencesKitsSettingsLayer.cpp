#include <FrenchieApplicationEditorPreferencesKitsSettingsLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

KitsSettings::KitsSettings() : 
    Frenchie::Application::Layer::Registry<KitsSettings>(STRINGIFY(KitsSettings)){}

KitsSettings::~KitsSettings(){}

std::string KitsSettings::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::Preferences), STRINGIFY(KitsSettings));
}

void KitsSettings::frame_update() 
{
    ImGui::BeginChild(get_name().c_str());
    {
        ImGui::TextUnformatted("This is kits settings");
    }

    ImGui::EndChild();
}

bool KitsSettings::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    return true;
}

bool KitsSettings::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    return true;
}