#include <FrenchieEditorFileSystemRenamePathsCommand.hpp>

// Core
#include <FrenchieCoreFileSystem.hpp>
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreStringUtilities.hpp>

// Application
#include <FrenchieApplicationConfigurationLoaderTranslatorLayer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

// STL
#include <filesystem>

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
            class RenamePathsDialog : public Frenchie::Application::Layer
            {
            public:

                RenamePathsDialog(const std::set<std::filesystem::path>& _Paths) : 
                    Frenchie::Application::Layer(Translator::translate("Rename files"))
                {
                    for(auto&& path : _Paths)
                    {
                        m_Paths.insert(
                            {
                                path, 
                                {
                                    Frenchie::Core::String::convert_utf32_to_utf8(path.filename().u32string()), 
                                    true
                                }
                            }
                        );
                    }
                }

                virtual ~RenamePathsDialog(){}

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
                            int checkboxID = 0;

                            for(auto&& path : m_Paths)
                            {
                                ImGui::PushID(checkboxID++);
                                ImGui::Checkbox("##", &path.second.second);
                                ImGui::PopID();
                                ImGui::SameLine();
                                ImGui::PushID(checkboxID++);
                                ImGui::InputText("###", &path.second.first);

                                ImGui::PopID();
                            }
                        }
                        ImGui::EndChild();

                        ImGui::BeginChild(
                            "Buttons",
                            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
                            ImGuiChildFlags_::ImGuiChildFlags_Borders,
                            wiondowFlags);
                        {
                            if(ImGui::Button(Translator::translate("Ok").c_str()))
                            {
                                // rename files
                                for(auto& item : m_Paths)
                                {
                                    auto source   = item.first;
                                    auto& text    = item.second.first;
                                    auto selected = item.second.second;

                                    if(!std::filesystem::exists(source) || !selected) 
                                        continue;

                                    auto target = std::filesystem::path(
                                        source.parent_path().u32string()
                                        .append(U"/")
                                        .append(Frenchie::Core::String::convert_utf8_to_utf32(text))
                                    );

                                    if(source == target) 
                                        continue;

                                    while (std::filesystem::exists(target))
                                    {
                                        auto extention = Frenchie::Core::String::convert_utf8_to_utf32(
                                            Frenchie::Core::FileSystem::get_file_extention(target));

                                        target = source.parent_path().u32string()
                                            .append(U"/")
                                            .append(target.filename().stem().u32string())
                                            .append(U"_Copy")
                                            .append(extention);
                                    }
                                    
                                    try
                                    {
                                        std::filesystem::rename(source, target);
                                    }
                                    catch(const std::exception& e)
                                    {
                                        Frenchie::Core::Logger::instance()->critical(e.what());
                                    }
                                    
                                }

                                // close dialog
                                close();
                            }
                            
                            ImGui::SameLine();
                            
                            if(ImGui::Button(Translator::translate("Cancel").c_str()))
                                close();
                        }
                        ImGui::EndChild();

                        ImGui::EndPopup();
                    }
                }

            protected:
                std::map<
                    std::filesystem::path, 
                    std::pair<std::string, bool>> m_Paths;
            };
        }
    }
}

// RenameFolderCommand
RenameFolderCommand::RenameFolderCommand(const CommandPayloads& _Sender) : 
    Frenchie::Application::Command::Registry<RenameFolderCommand, const CommandPayloads&>(_Sender){}

RenameFolderCommand::~RenameFolderCommand(){}

void RenameFolderCommand::execute()
{
    std::set<std::filesystem::path> renamed;

    auto pathPayload    = get_payload<Frenchie::Application::CommandDataPayload<std::filesystem::path>>();
    auto pathSetPayload = get_payload<Frenchie::Application::CommandDataPayload<std::set<std::filesystem::path>>>();

    if(pathPayload != nullptr)
        renamed.insert(pathPayload->get());

    if(pathSetPayload != nullptr)
    {
        auto paths = pathSetPayload->get();

        for(auto&& path : paths)
            renamed.insert(path);
    }

    if(!renamed.empty())
        Frenchie::Application::application()->push_layer<RenamePathsDialog>(renamed);
}

std::string RenameFolderCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::Folder), "Rename");
}

// RenameFileCommand
RenameFileCommand::RenameFileCommand(const CommandPayloads& _Sender) : 
    Frenchie::Application::Command::Registry<RenameFileCommand, const CommandPayloads&>(_Sender){}

RenameFileCommand::~RenameFileCommand(){}

void RenameFileCommand::execute()
{
    std::set<std::filesystem::path> renamed;

    auto pathPayload    = get_payload<Frenchie::Application::CommandDataPayload<std::filesystem::path>>();
    auto pathSetPayload = get_payload<Frenchie::Application::CommandDataPayload<std::set<std::filesystem::path>>>();

    if(pathPayload != nullptr)
        renamed.insert(pathPayload->get());

    if(pathSetPayload != nullptr)
    {
        auto paths = pathSetPayload->get();

        for(auto&& path : paths)
            renamed.insert(path);
    }

    if(!renamed.empty())
        Frenchie::Application::application()->push_layer<RenamePathsDialog>(renamed);
}

std::string RenameFileCommand::factory_id()
{
    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::FileSystem::Menu::File), "Rename");
}

const bool FrenchieEditorFileSystemRenameFolderCommand = RenameFolderCommand::registerFactory();
const bool FrenchieEditorFileSystemRenameFileCommand   = RenameFileCommand::registerFactory();