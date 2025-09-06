#include <FrenchieApplicationEditorLoaderLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// LoaderModel
LoaderModel::LoaderModel(){}
LoaderModel::~LoaderModel(){}

// LoaderView
LoaderView::LoaderView(std::shared_ptr<LoaderModel> _Model, const std::string& _Name) : 
    Layer(_Name), 
    m_Model(_Model){}

LoaderView::~LoaderView(){}

bool LoaderView::awake()
{
    return m_Model != nullptr && m_Model->awake();
}

void LoaderView::frame_update()
{
    if(m_Model == nullptr) 
    {
        close();
        return;
    }

    // calculate progress percantage
    float progress   = m_Model->execute();
    int   percantage = (int)(progress * 100.f);

    // show progress
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("100%").x);
    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "");
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::TextUnformatted(fmt::format("{} %", percantage).c_str());

    // check if we are done
    if(percantage >= 100.f)
    {
        close(); // close self when finished
        return;
    }
}

void LoaderView::finish()
{
    if(m_Model != nullptr) 
        m_Model->finish();
}