#include <FrenchieImmediateUserInterfaceTestLayer.hpp>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    Frenchie::Application::App::push_layer<Frenchie::Application::FrenchieImmediateUserInterfaceTestLayer>();
    return Frenchie::Application::App::execute();
}