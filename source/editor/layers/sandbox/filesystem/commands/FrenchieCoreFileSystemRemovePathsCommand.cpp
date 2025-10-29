#include <FrenchieCoreFileSystemRemovePathsCommand.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationConfigurationLoaderLanguage.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Core;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Configuration;
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            class RemovePathsDialog : public Frenchie::Application::Layer
            {
            public:
                RemovePathsDialog(const std::set<std::filesystem::path>& _Paths) : 
                    Frenchie::Application::Layer(Translator::translate("Are you sure you want to remove these files/folders ?")), 
                    m_Paths(_Paths){}
                virtual ~RemovePathsDialog(){}

                virtual void frame_update() override
                {
                    ImGui::OpenPopup(m_Name.c_str());

                    ImGuiStyle& style = ImGui::GetStyle();

                    ImGui::SetNextWindowSizeConstraints(ImVec2(512.f, 512.f), ImVec2(FLT_MAX, FLT_MAX));

                    // press button
                    auto wiondowFlags =
                        ImGuiWindowFlags_::ImGuiWindowFlags_None        | 
                        ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | 
                        ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse;

                    if(ImGui::BeginPopupModal(m_Name.c_str(), &m_Opened))
                    {
                        ImGui::BeginChild(
                            "Content",
                            ImVec2(
                                ImGui::GetContentRegionAvail().x, 
                                ImGui::GetContentRegionAvail().y - (ImGui::CalcTextSize("Button").y + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("BUTTON").y)),
                            ImGuiChildFlags_::ImGuiChildFlags_Borders,
                            wiondowFlags);
                        {
                            for(auto&& path : m_Paths)
                                ImGui::TextUnformatted(path.string().c_str());
                        }
                        ImGui::EndChild();

                        ImGui::BeginChild(
                            "Buttons",
                            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
                            ImGuiChildFlags_::ImGuiChildFlags_Borders,
                            wiondowFlags);
                        {
                            if(ImGui::Button(Translator::translate("Yes").c_str()))
                            {
                                if(m_Paths.empty())
                                    return;

                                for(auto& path : m_Paths)
                                {
                                    try
                                    {
                                        std::filesystem::remove_all(path);
                                    }
                                    catch(...)
                                    {
                                        Frenchie::Core::Logger::instance()->critical(fmt::format("Could not remove {}", path.string()));
                                    }
                                }

                                close();
                            }
                            
                            ImGui::SameLine();
                            
                            if(ImGui::Button(Translator::translate("No").c_str()))
                                close();
                        }
                        ImGui::EndChild();

                        ImGui::EndPopup();
                    }
                }

                virtual bool allows_multiple_instances() const override
                {
                    return false;
                }

            protected:
                std::set<std::filesystem::path> m_Paths;
            };
        }
    }
}

// RemoveFolderCommand
RemoveFolderCommand::RemoveFolderCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Sender) : 
    Frenchie::Application::Command::Registry<RemoveFolderCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Sender){}

RemoveFolderCommand::~RemoveFolderCommand(){}

void RemoveFolderCommand::execute()
{
    auto payload = 
        get_payload<Frenchie::Application::CommandDataPayload<std::filesystem::path>>();

    if(payload != nullptr)
    {
        Frenchie::Application::application()->push_layer<RemovePathsDialog>(
            std::set<std::filesystem::path>({payload->get()})
        );
    }
}

std::string RemoveFolderCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::Folder), "Remove");
}

const bool removeFolderCommand = RemoveFolderCommand::registerFactory();

// RemoveFileCommand
RemoveFileCommand::RemoveFileCommand(const std::shared_ptr<Frenchie::Application::CommandPayload>& _Sender) : 
    Frenchie::Application::Command::Registry<RemoveFileCommand, const std::shared_ptr<Frenchie::Application::CommandPayload>&>(_Sender){}

RemoveFileCommand::~RemoveFileCommand(){}

void RemoveFileCommand::execute()
{
    // remove single path
    auto pathPayload = get_payload<Frenchie::Application::CommandDataPayload<std::filesystem::path>>();

    if(pathPayload != nullptr)
    {
        Frenchie::Application::application()->push_layer<RemovePathsDialog>(
            std::set<std::filesystem::path>({pathPayload->get()})
        );

        return;
    }

    // remove a set of paths
    auto pathSetPayload = get_payload<Frenchie::Application::CommandDataPayload<std::set<std::filesystem::path>>>();

    if(pathSetPayload != nullptr)
        Frenchie::Application::application()->push_layer<RemovePathsDialog>(pathSetPayload->get());
}

std::string RemoveFileCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::File), "Remove");
}

const bool removeFileCommand = RemoveFileCommand::registerFactory();