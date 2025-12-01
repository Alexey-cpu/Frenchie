// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplication.hpp>

#include <FrenchieApplicationSandBoxImmediate2DRendererTestLayer.hpp>

int main(int argc, char *argv[])
{
    Frenchie::Application::application()->push_layer<Frenchie::Application::Immedidate2DRendererTestLayer>();
    return Frenchie::Application::application()->execute();
}