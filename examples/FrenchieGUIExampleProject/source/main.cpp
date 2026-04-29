#include <FrenchieImmediateUserInterfaceTestLayer.hpp>
#include <iostream>

class ApplicationCloseLayer : public Frenchie::Application::Layer
{
public:
    ApplicationCloseLayer() : Frenchie::Application::Layer(STRINGIFY(ApplicationCloseLayer)){}
    virtual ~ApplicationCloseLayer(){}

    virtual void frame_finish()
    {
        Frenchie::Application::application()->close();
    }
};

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if(std::string(argv[i]) == "test")
            Frenchie::Application::application()->push_layer<ApplicationCloseLayer>();
    }

    Frenchie::Application::application()->push_layer<Frenchie::Application::FrenchieImmediateUserInterfaceTestLayer>();

    return Frenchie::Application::application()->execute();
}