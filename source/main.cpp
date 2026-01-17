// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <ImmediateUserInterfaceTestLayer.hpp>

int main(int argc, char *argv[])
{
    Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceTestLayer>();
    return Frenchie::Application::application()->execute();
}