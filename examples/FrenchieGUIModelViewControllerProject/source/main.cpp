#include <FrenchieApplication.hpp>
#include <FrenchieImmediateUserInterfaceLayer.hpp>
#include <FrenchieApplicationLayerRenderingQueue2D.hpp>
#include <FrenchieImmediateUserInterfaceModelViewControllerLayer.hpp>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // FrenchieImmediateUserInterfacePlotLine2DView.json
    // FrenchieImmediateUserInterfacePlotPieView.json

    // Frenchie::Application::Application::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
    //     "C:/SDK/Qt_Projects/OpenGL/examples/FrenchieGUIModelViewControllerProject/FrenchieImmediateUserInterfacePlotLine2DView.json",
    //     std::make_shared<LinePlot2DViewController>());

    // Frenchie::Application::Application::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
    //     "C:/SDK/Qt_Projects/OpenGL/examples/FrenchieGUIModelViewControllerProject/FrenchieImmediateUserInterfaceWidgetsView.json",
    //     std::make_shared<WidgetsViewController>());

    return Frenchie::Application::Application::execute();
}