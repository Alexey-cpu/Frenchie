#include <FrenchieImmediateUserInterfaceTestLayer.hpp>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    Frenchie::Application::application()->push_layer<Frenchie::Application::FrenchieImmediateUserInterfaceTestLayer>();
    return Frenchie::Application::application()->execute();
}