#include <FrenchieApplicationEditorLoaderLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// LoaderView
AsyncLoaderView::AsyncLoaderView(
    const std::function<void(AsyncLoaderView*)>& _LoadFunction, 
    const std::function<void()>&            _OnFinished,
    const std::string&                      _Name) : 
    Dialog(_Name),
    m_LoadFunction(_LoadFunction), 
    m_OnFinished(_OnFinished){}

AsyncLoaderView::~AsyncLoaderView(){}

void AsyncLoaderView::set_progress(const float& _Value)
{
    m_Progress = _Value;
}

bool AsyncLoaderView::awake()
{
    m_Task = std::make_shared<Frenchie::Core::Task>(
        [this](Frenchie::Core::Task* _Task)
        {
            if(m_LoadFunction != nullptr)
                m_LoadFunction(this);

            _Task->finish();
        }
    );

    return m_Task != nullptr;
}

void AsyncLoaderView::frame_update()
{
    if(m_Task == nullptr || m_Task->finished() || m_Task->canceled())
    {
        close();
        return;
    }
    
    Dialog::frame_update();
}

void AsyncLoaderView::draw_content()
{
    // calculate progress percantage
    int percantage = (int)(m_Progress * 100.f);

    // show progress
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("100%").x);
    ImGui::ProgressBar(m_Progress, ImVec2(0.0f, 0.0f), "");
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::TextUnformatted(fmt::format("{} %", percantage).c_str());
}

void AsyncLoaderView::draw_buttons()
{
    if(ImGui::Button("Cancel")) 
        close();
}

void AsyncLoaderView::finish()
{
    if(m_OnFinished != nullptr) 
        m_OnFinished();
}