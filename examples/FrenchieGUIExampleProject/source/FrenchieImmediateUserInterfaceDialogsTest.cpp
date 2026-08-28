#include <FrenchieImmediateUserInterfaceDialogsTest.hpp>

using namespace Frenchie::Application;

FrenchieImmediateUserInterfaceDialogsTest::FrenchieImmediateUserInterfaceDialogsTest() : Layer(STRINGIFY(FrenchieImmediateUserInterfaceDialogsTest)){}
FrenchieImmediateUserInterfaceDialogsTest::~FrenchieImmediateUserInterfaceDialogsTest(){}

bool FrenchieImmediateUserInterfaceDialogsTest::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    return m_UI != nullptr;
}

void FrenchieImmediateUserInterfaceDialogsTest::frame_update()
{
    if(m_UI->begin_window(
        m_UI->next_id("Dialogs test window"),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_UI->push_button(m_UI->next_id("Open dialog", "OpenDialog")))
            m_ShowParentDialog = true;

        if(m_UI->begin_dialog(
            m_UI->next_id("Dialog", "ParentDialog"),
              ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults
            | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ShowDialogBlur,
            &m_ShowParentDialog))
        {
            if(m_UI->push_button(m_UI->next_id("Open another dialog", "OpenAnotherDialog")))
                m_ShowChildDialog = true;

            if(m_UI->begin_dialog(
                m_UI->next_id("Another dialog", "AnotherDialog"),
                  ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults
                | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ShowDialogBlur,
                &m_ShowChildDialog))
            {
                m_UI->end_dialog();
            }

            m_UI->end_dialog();
        }

        m_UI->end_window();
    }
}