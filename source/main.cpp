// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplication.hpp>

#include <ImmedidateUserInterfaceLayer.hpp>

int main(int argc, char *argv[])
{
    Frenchie::Application::application()->push_layer<Frenchie::Application::ImmedidateUserInterfaceContextLayer>();
    return Frenchie::Application::application()->execute();
}