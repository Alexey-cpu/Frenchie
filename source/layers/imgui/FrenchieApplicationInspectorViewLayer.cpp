#include <FrenchieApplicationInspectorViewLayer.hpp>

#include <imgui.h>

using namespace Frenchie::Application;
using namespace Frenchie::Renderer;

InspectorView::InspectorView(const std::string& _Name, std::shared_ptr<Scene3D> _Scene3D) : 
    Layer(_Name), 
    m_Scene(_Scene3D){}

InspectorView::~InspectorView(){}

bool InspectorView::awake()
{
    return Layer::awake();
}

void InspectorView::frame_start()
{
    return Layer::frame_start();
}

void InspectorView::frame_update()
{
    if(m_Scene == nullptr) 
        return;

    ImGui::Begin(get_name().c_str());

    m_Scene->apply_to_children_recursive(
        [](Object* _Object)
        {
            if(_Object->check_flag(Object::Focused)) 
                _Object->draw();
        }
    );

    ImGui::End();

    Layer::frame_update();
}

void InspectorView::frame_finish()
{
    Layer::frame_finish();
}

void InspectorView::finish()
{
    Layer::finish();
}

void InspectorView::close()
{
    Layer::close();
}

bool InspectorView::is_closed()
{
    return Layer::is_closed();
}