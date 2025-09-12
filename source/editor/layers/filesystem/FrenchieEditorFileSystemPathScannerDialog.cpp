#include <FrenchieEditorFileSystemPathScannerDialog.hpp>

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            class ScanPaths : public Dialog
            {
            public:
                ScanPaths(
                    const std::filesystem::path&                                       _Path,
                    const std::function<bool(const std::filesystem::path&)>&           _Predicate,
                    const std::function<void(std::map<std::filesystem::path, bool>&)>& _OnFinished     = nullptr,
                    const std::function<void(std::map<std::filesystem::path, bool>&)>& _OnCanceled     = nullptr,
                    const std::function<void(std::map<std::filesystem::path, bool>&)>& _OnFailed       = nullptr,
                    const std::string&                                                 _Name           = STRINGIFY(Frenchie::Application::Editor::ScannerView),
                    size_t                                                             _MaxSearchDepth = 100) :
                    Dialog(_Name, 512.f, 128.f),
                    m_Launcher([ // copy everything inside a process caller...
                        this,
                        _Path, 
                        _Predicate, 
                        _OnFinished, 
                        _OnCanceled, 
                        _OnFailed,
                        _Name, 
                        _MaxSearchDepth]()
                    {
                        m_Process = Frenchie::Application::ProcessQueue::instance()->push<FilesystemPathsSearchProcess>(
                            _Path,
                            _Predicate,
                            _Name,
                            _MaxSearchDepth);

                        m_Process->on_finished(
                            [this, _OnFinished]()
                            {
                                if(_OnFinished)
                                    _OnFinished(m_Process->m_Paths);
                            }
                        );

                        m_Process->on_canceled(
                            [this, _OnCanceled]()
                            {
                                if(_OnCanceled)
                                    _OnCanceled(m_Process->m_Paths);
                            }
                        );

                        m_Process->on_failed(
                            [this, _OnFailed]()
                            {
                                if(_OnFailed)
                                    _OnFailed(m_Process->m_Paths);
                            }
                        );
                    }
                ){}

                virtual ~ScanPaths()
                {
                }
                
                bool awake()
                {
                    // start process
                    if(m_Launcher != nullptr) 
                        m_Launcher();
                    m_Launcher = nullptr;

                    return true;
                }

                void frame_update()
                {
                    if(m_Process == nullptr   || 
                        m_Process->canceled() || 
                        m_Process->failed()) 
                    {
                        close();
                        return;
                    }

                    Dialog::frame_update();
                }

                void draw_content()
                {
                    std::lock_guard<std::mutex> lock(m_Mutex);

                    // show result
                    ImGui::BeginChild("Entries");
                    {
                        int id = 0;

                        for(auto&& entry : m_Process->m_Paths)
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

                        ImGui::EndChild();
                    }
                }

                void draw_buttons()
                {
                    std::lock_guard<std::mutex> lock(m_Mutex);

                    if(ImGui::Button(Translator::translate("Pause").c_str())) 
                        m_Process->pause();

                    ImGui::SameLine();

                    if(ImGui::Button(Translator::translate("Resume").c_str())) 
                        m_Process->resume();

                    ImGui::SameLine();

                    if(ImGui::Button(Translator::translate("Cancel").c_str())) 
                        m_Process->cancel();

                    ImGui::SameLine();

                    if(m_Process->finished() || m_Process->paused())
                    {
                        ImGui::SameLine();

                        if(ImGui::Button(Translator::translate("Ok").c_str())) 
                            close();
                    }

                    ImGui::SameLine();

                    // show currently processing path
                    if(m_Process->finished())
                    {
                        ImGui::TextWrapped(Translator::translate("Finished...").c_str());
                    }
                    else 
                    {    
                    ImGui::Text(
                        fmt::format("{} %s", Translator::translate("Scaning ")).c_str(), 
                        Frenchie::Core::String::as_utf8(m_Process->m_CurrentPath.wstring()).c_str());
                    }
                }

            protected:

                std::mutex                                    m_Mutex;
                std::function<void()>                         m_Launcher = nullptr;
                std::shared_ptr<FilesystemPathsSearchProcess> m_Process  = nullptr;
            };
        }
    }
}

// PathScannerDialogAsyncProcess
FilesystemPathsSearchProcess::FilesystemPathsSearchProcess(
    const std::filesystem::path&                             _Path,
    const std::function<bool(const std::filesystem::path&)>& _Predicate,
    const std::string&                                       _Name,
    size_t                                                   _MaxSearchDepth) :
    Process(_Name),
    m_Path(_Path),
    m_Predicate(_Predicate),
    m_MaxSearchDepth(_MaxSearchDepth){}

FilesystemPathsSearchProcess::~FilesystemPathsSearchProcess(){}

bool FilesystemPathsSearchProcess::awake()
{
    Frenchie::Core::ThreadPool::instance()->enqueue(
        [this]()
        {
            try
            {
                for(auto it = std::filesystem::recursive_directory_iterator(m_Path, std::filesystem::directory_options::skip_permission_denied); 
                    it != std::filesystem::recursive_directory_iterator(); it++)
                {
                    // cancel task
                    if(m_Canceled) 
                        return;

                    // pause task
                    while(m_Paused)
                    {
                        // cancel task during pause state
                        if(m_Canceled) 
                            return;
                    }

                    // update current path
                    m_CurrentPath = it->path();

                    if (it.depth() > m_MaxSearchDepth)
                    {
                        const_cast<std::filesystem::recursive_directory_iterator&>(it)
                            .disable_recursion_pending();
                        continue;
                    }

                    if(m_Predicate(m_CurrentPath))
                        m_Paths.insert({m_CurrentPath, true});
                }

                // finish task
                m_Finished = true;
            }
            catch(const std::exception& e)
            {
                Frenchie::Core::Logger::instance()->critical(e.what());
                m_Failed = true;
            }
        }
    );

    return true;
}

PathScannerDialog::PathScannerDialog(
    const std::function<bool(const std::filesystem::path&)>& _Predicate,
    const std::string&                                       _Name) : 
    Explorer(_Name), m_Predicate(_Predicate){}

PathScannerDialog::~PathScannerDialog(){}

// Frenchie::Application::Layer
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
        ImGui::BeginChild(
            "Content",
            ImVec2(
                ImGui::GetContentRegionAvail().x, 
                ImGui::GetContentRegionAvail().y - (ImGui::CalcTextSize("Button").y + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("BUTTON").y)),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);
        {
            draw_contents();
            ImGui::EndChild();
        }

        ImGui::BeginChild(
            "Buttons",
            ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
            ImGuiChildFlags_::ImGuiChildFlags_Borders,
            wiondowFlags);

        {
            if(ImGui::Button(Translator::translate("Ok").c_str()))
            {
                auto dialog = 
                    Frenchie::Application::application()->push_layer<FileSystem::ScanPaths>(
                        get_current_path(), 
                        m_Predicate, 
                        m_OnFinished, 
                        m_OnCanceled, 
                        m_OnFailed
                    );

                close();
            }

             ImGui::SameLine();

            if(ImGui::Button(Translator::translate("Cancel").c_str()))
                close();

            ImGui::EndChild();
        }

        ImGui::EndPopup();
    }
}

bool PathScannerDialog::allows_multiple_instances() const
{
    return false;
}

void PathScannerDialog::on_finished(const std::function<void(std::map<std::filesystem::path, bool>&)>& _Callback)
{
    m_OnFinished = _Callback;
}

void PathScannerDialog::on_canceled(const std::function<void(std::map<std::filesystem::path, bool>&)>& _Callback)
{
    m_OnCanceled = _Callback;
}

void PathScannerDialog::on_failed(const std::function<void(std::map<std::filesystem::path, bool>&)>& _Callback)
{
    m_OnFailed = _Callback;
}