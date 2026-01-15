#include <ImmediateUserInterfaceLayer2.hpp>

using namespace Frenchie::Application;

ImmedidateUserInterfaceContextLayer2::ImmedidateUserInterfaceContextLayer2(){}
ImmedidateUserInterfaceContextLayer2::~ImmedidateUserInterfaceContextLayer2(){}

bool ImmedidateUserInterfaceContextLayer2::awake()
{
    if(m_Renderer == nullptr)
        m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();

    return m_Renderer != nullptr;
}

void ImmedidateUserInterfaceContextLayer2::frame_start()
{
}

void ImmedidateUserInterfaceContextLayer2::frame_update()
{
    if(begin_node("Root", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally))
    {
        if(begin_node("Root/Child-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically))
        {
            if(begin_node("Root/Child-1/Child-1-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically)) end_node();
            if(begin_node("Root/Child-1/Child-1-2", UINodeSettings_::UINodeSettings_LayoutChildrenVertically)) end_node();
            if(begin_node("Root/Child-1/Child-1-3", UINodeSettings_::UINodeSettings_LayoutChildrenVertically)) end_node();

            end_node();
        }

        if(begin_node("Root/Child-2", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally))
        {
            end_node();
        }

        if(begin_node("Root/Child-3", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally))
        {
            end_node();
        }

        end_node();
    }

    if(begin_node("Root-1", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally))
    {
        m_NodesRenderingStack[m_NodesRenderingStack.size()-1]->State.WindowBox =
            gs_2dboxf(gs_vec2f(255.f, 255.f), gs_vec2f(512.f, 512.f));

        if(begin_node("Root-1/Child-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically))
        {
            if(begin_node("Root-1/Child-1/Child-1-1", UINodeSettings_::UINodeSettings_LayoutChildrenVertically)) end_node();
            if(begin_node("Root-1/Child-1/Child-1-2", UINodeSettings_::UINodeSettings_LayoutChildrenVertically)) end_node();
            if(begin_node("Root-1/Child-1/Child-1-3", UINodeSettings_::UINodeSettings_LayoutChildrenVertically)) end_node();

            end_node();
        }

        if(begin_node("Root-1/Child-2", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally))
        {
            end_node();
        }

        if(begin_node("Root-1/Child-3", UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally))
        {
            end_node();
        }

        end_node();
    }
}

void ImmedidateUserInterfaceContextLayer2::compute_geometry(UINode* _Node)
{
    gs_vec2f position   = _Node->State.WindowBox.Min;
    float    childCount = m_Hierarchy.end(_Node) - m_Hierarchy.begin(_Node);

    for(auto it = m_Hierarchy.begin(_Node); it != m_Hierarchy.end(_Node); ++it)
    {
        // compute self
        auto window = *it;
        auto parent = _Node;

        gs_vec2f size = parent->State.WindowBox.size() / (float)childCount;

        if((parent->State.Settings & UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally))
            size = gs_vec2f(size.x, parent->State.WindowBox.height());
        else if((parent->State.Settings & UINodeSettings_::UINodeSettings_LayoutChildrenVertically))
            size = gs_vec2f(parent->State.WindowBox.width(), size.y);

        size = gs_clamp(size, gs_vec2f(4.f), gs_vec2f((float)INT_MAX));

        window->State.WindowBox = gs_2dboxf(position, position + size);

        // compute child
        compute_geometry(window);

        // next
        if((parent->State.Settings & UINodeSettings_::UINodeSettings_LayoutChildrenHorizontally))
            position += gs_vec2f(size.x, 0.f);
        else if((parent->State.Settings & UINodeSettings_::UINodeSettings_LayoutChildrenVertically))
            position += gs_vec2f(0.f, size.y);
    }
}

void ImmedidateUserInterfaceContextLayer2::render(UINode* _Node)
{
    for(auto it = m_Hierarchy.begin(_Node); it != m_Hierarchy.end(_Node); ++it)
    {
        auto window = *it;

        m_Renderer->push_clip_box(window->State.WindowBox);
        window->render(m_Renderer.get());
        render(window);

        m_Renderer->pop_clip_box();
    }
}

void ImmedidateUserInterfaceContextLayer2::frame_debug()
{
    // compute geometry
    m_Hierarchy.build(m_NodesRenderingList);

    for (auto& singleton : m_Hierarchy.Singletons)
        compute_geometry(singleton);

    // catch event
    UIEvent event;
    event.CursorPosition  = Frenchie::Application::application()->get_window_cursor_position();
    event.CursorDragDelta = Frenchie::Application::application()->get_window_cursor_dragdelta();

    for (int button = ApplicationMouseButton::ApplicationMouseButton_Begin;
             button < ApplicationMouseButton::ApplicationMouseButton_End;
             button++)
    {
        if(Frenchie::Application::application()->is_mouse_button_down((ApplicationMouseButton::Button)button))
            event.MouseDown = (ApplicationMouseButton::Button)button;

        if(Frenchie::Application::application()->is_mouse_button_hold((ApplicationMouseButton::Button)button))
            event.MouseHold = (ApplicationMouseButton::Button)button;

        if(Frenchie::Application::application()->is_mouse_button_pressed((ApplicationMouseButton::Button)button))
            event.MousePressed = (ApplicationMouseButton::Button)button;

        if(Frenchie::Application::application()->is_mouse_button_clicked((ApplicationMouseButton::Button)button))
            event.MouseClicked = (ApplicationMouseButton::Button)button;

        if(Frenchie::Application::application()->is_mouse_button_double_clicked((ApplicationMouseButton::Button)button))
            event.MouseDoubleClicked = (ApplicationMouseButton::Button)button;
    }

    for (auto& singleton : m_Hierarchy.Singletons)
    {
        if(singleton->event(event))
            break;
    }
}

void ImmedidateUserInterfaceContextLayer2::frame_render()
{
    // render
    for (auto& singleton : m_Hierarchy.Singletons)
        render(singleton);
}

void ImmedidateUserInterfaceContextLayer2::frame_finish()
{
    // save state
    for (auto& node : m_NodesRenderingList)
    {
        // save cache
        node->Cache = node->State;

        // clean-up
        node->State.Depth       = 0;
        node->State.Thickness   = 1;
        node->State.DrawIndex   = 0;
        node->State.Parent      = nullptr;
        node->State.Settings    = 0;
        node->State.WindowBox = {gs_2dboxf(gs_vec2f(0.f, 0.0), gs_vec2f(255.f, 255.f))};
    }

    // clean-up
    m_NodesRenderingCache = m_NodesRenderingList;
    m_NodesRenderingList.clear();
    m_NodesRenderingCache.clear();
}