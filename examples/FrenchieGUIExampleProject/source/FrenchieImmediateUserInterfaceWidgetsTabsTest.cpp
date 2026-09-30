#include <FrenchieImmediateUserInterfaceWidgetsTabsTest.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceWidgetsTabsTest::FrenchieImmediateUserInterfaceWidgetsTabsTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceWidgetsTabsTest)){}
FrenchieImmediateUserInterfaceWidgetsTabsTest::~FrenchieImmediateUserInterfaceWidgetsTabsTest(){}

bool FrenchieImmediateUserInterfaceWidgetsTabsTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceWidgetsTabsTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Tabs test", "TabsTest"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->begin_tabs(m_UI->next_id("Tabs")))
        {
            if(m_UI->begin_tab(m_UI->next_id("TextualTab", "TextualTab"),ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults, &m_SomeTextOpened))
            {
                if(m_UI->begin_scrollarea(m_UI->next_id("ScrollArea")))
                {
                    m_UI->input_string_multiline(m_UI->next_id("Text"), m_SomeText);
                    m_UI->end_scrollarea();
                }

                m_UI->end_tab();
            }

            if(m_UI->begin_tab(m_UI->next_id("ButtonsTab", "ButtonsTab")))
            {
                m_UI->push_button(m_UI->next_id("Button-1", "Button-1"));
                m_UI->push_button(m_UI->next_id("Button-2", "Button-2"));
                m_UI->push_button(m_UI->next_id("Button-2", "Button-3"));

                m_UI->end_tab();
            }

            if(m_UI->begin_tab(m_UI->next_id("LabelsTab", "LabelsTab")))
            {
                m_UI->label(m_UI->next_id("Label"), "This is a very usefull label");
                m_UI->end_tab();
            }

            m_UI->end_tabs();
        }

        m_UI->end_window();
    }
}

bool FrenchieImmediateUserInterfaceWidgetsTabsTest::allows_multiple_instances() const
{
    return false;
}