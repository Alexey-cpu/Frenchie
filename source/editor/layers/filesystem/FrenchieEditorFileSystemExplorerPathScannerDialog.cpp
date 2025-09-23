#include <FrenchieEditorFileSystemExplorerPathScannerDialog.hpp>

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

PathScannerDialog::PathScannerDialog(
    const std::function<bool(const std::filesystem::path&)>& _Predicate,
    const std::string&                                       _Name) : 
    Explorer(_Name), 
    m_Predicate(_Predicate){}

PathScannerDialog::~PathScannerDialog(){}

// Frenchie::Application::Layer
bool PathScannerDialog::awake()
{
    if(!Explorer::awake()) 
        return false;

    m_ThreadQueue = Frenchie::Application::application()->push_layer<Frenchie::Application::ThreadQueue>();

    return true;
}

void PathScannerDialog::frame_update()
{
    ImGui::OpenPopup(m_Name.c_str());

    ImGuiStyle& style = ImGui::GetStyle();

    // press button
    auto wiondowFlags =
        ImGuiWindowFlags_::ImGuiWindowFlags_None        | 
        ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::SetNextWindowSizeConstraints(ImVec2(512, 512), ImVec2(FLT_MAX, FLT_MAX));

    if(ImGui::BeginPopupModal(m_Name.c_str(), &m_Opened))
    {
        // draw content
        ImGui::BeginChild(
            "Content",
            ImVec2(
                ImGui::GetContentRegionAvail().x, 
                ImGui::GetContentRegionAvail().y - (ImGui::CalcTextSize("Button").y + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("BUTTON").y)),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);
        {
            if(m_Thread == nullptr)
            {
                draw_contents();
            }
            else
            {
                std::lock_guard<std::mutex> lock(m_Mutex);

                int id = 0;

                for(auto&& entry : m_Paths)
                {
                    ImGui::PushID(id++);
                    ImGui::Checkbox("##", &entry.second);
                    ImGui::PopID();

                    ImGui::SameLine();

                    ImGui::PushID(id++);
                    ImGui::Selectable(
                        Frenchie::Core::String::as_utf8(entry.first.wstring()).c_str(), 
                        &entry.second,
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns    | 
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowOverlap      | 
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_NoAutoClosePopups |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick);
                    ImGui::PopID();
                }
            }

            ImGui::EndChild();
        }

        // draw buttons
        ImGui::BeginChild(
            "Buttons",
            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);

        {
            if(m_Thread == nullptr)
            {
                // launch thread here
                if(ImGui::Button(Translator::translate("Ok").c_str()))
                {
                    m_Thread = 
                        m_ThreadQueue->push(
                            [this](const Frenchie::Application::Thread* _Thread)
                            {
                                for(auto it = std::filesystem::recursive_directory_iterator(get_current_path(), std::filesystem::directory_options::skip_permission_denied); 
                                    it != std::filesystem::recursive_directory_iterator(); it++)
                                {
                                    if(_Thread->requested_stop()) 
                                        return;

                                    if (it.depth() > m_MaximumSearchDepth)
                                    {
                                        const_cast<std::filesystem::recursive_directory_iterator&>(it)
                                            .disable_recursion_pending();
                                        continue;
                                    }

                                    m_CurrentlyProcessedPath = it->path();

                                    if(m_Predicate(it->path()))
                                        m_Paths.insert({it->path(), true});
                                }

                                // wait here
                                _Thread->pause();
                                _Thread->requested_stop();
                            }, 
                            "Scanning paths..."
                        );
                }

                ImGui::SameLine();

                if(ImGui::Button(Translator::translate("Cancel").c_str()))
                    close();
            }
            else
            {
                if(ImGui::Button(Translator::translate("Pause").c_str())) 
                    m_Thread->pause();

                ImGui::SameLine();

                if(ImGui::Button(Translator::translate("Resume").c_str())) 
                    m_Thread->resume();

                ImGui::SameLine();

                if(ImGui::Button(Translator::translate("Cancel").c_str())) 
                    m_Thread->stop();

                ImGui::SameLine();

                if(m_Thread->finished() || m_Thread->paused())
                {
                    ImGui::SameLine();

                    if(ImGui::Button(Translator::translate("Ok").c_str())) 
                    {
                        // resume thread
                        close();
                    }
                }

                ImGui::SameLine();

                // show currently processing path
                if(m_Thread->finished())
                {
                    ImGui::TextUnformatted("Thread awaits...");
                }
                else 
                {
                    std::lock_guard<std::mutex> lock(m_Mutex);
                    ImGui::TextWrapped(m_CurrentlyProcessedPath.string().c_str());
                }
            }

            ImGui::EndChild();
        }

        ImGui::EndPopup();
    }
}

void PathScannerDialog::finish()
{
    // close local thread queue
    if(m_ThreadQueue != nullptr) 
        m_ThreadQueue->close();

    // execute callback
    if(m_OnAccepted != nullptr) 
        m_OnAccepted(m_Paths);
}

bool PathScannerDialog::allows_multiple_instances() const
{
    return false;
}

void PathScannerDialog::on_accepted(const std::function<void(const std::map<std::filesystem::path, bool>&)>& _Callback)
{
    m_OnAccepted = _Callback;
}