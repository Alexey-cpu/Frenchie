// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplication.hpp>

#include <ImmedidateUserInterfaceLayer.hpp>
#include <ImmediateUserInterfaceLayer2.hpp>

int main(int argc, char *argv[])
{
    //Frenchie::Application::application()->push_layer<Frenchie::Application::ImmedidateUserInterfaceContextLayer>();
    Frenchie::Application::application()->push_layer<Frenchie::Application::ImmedidateUserInterfaceContextLayer2>();
    return Frenchie::Application::application()->execute();
}