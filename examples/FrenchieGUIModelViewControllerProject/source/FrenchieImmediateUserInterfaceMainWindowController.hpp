#pragma once

#include <FrenchieImmediateUserInterfaceModelViewControllerLayer.hpp>

#include <FrenchieImmediateUserInterfaceWidgetsController.hpp>
#include <FrenchieImmediateUserInterfacePlotVectorsController.hpp>
#include <FrenchieImmediateUserInterfacePlotPieController.hpp>
#include <FrenchieImmediateUserInterfacePlotLine2DController.hpp>
#include <FrenchieImmediateUserInterfaceCustomWidgetController.hpp>

class FrenchieImmediateUserInterfaceMainWindowController : public Frenchie::Application::ImmediateUserInterfaceViewController
{
public:

    virtual ~FrenchieImmediateUserInterfaceMainWindowController(){}

    virtual bool setup(Frenchie::Application::ImmediateUserInterfaceViewModel* _Model) override
    {
        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterface*)>>("OpenWidgetsViewAction") =
            [](Frenchie::Application::ImmediateUserInterface*)
        {
            Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
                "assets/views/FrenchieImmediateUserInterfaceWidgetsView.json",
                std::make_shared<FrenchieImmediateUserInterfaceWidgetsController>());
        };
        
        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterface*)>>("OpenPlots2DViewAction") =
            [](Frenchie::Application::ImmediateUserInterface*)
        {
            Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
                "assets/views/FrenchieImmediateUserInterfacePlotLine2DView.json",
                std::make_shared<FrenchieImmediateUserInterfacePlotLine2DController>());
        };

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterface*)>>("OpenPieChartsViewAction") =
            [](Frenchie::Application::ImmediateUserInterface*)
        {
            Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
                "assets/views/FrenchieImmediateUserInterfacePlotPieView.json",
                std::make_shared<FrenchieImmediateUserInterfacePlotPieController>());
        };

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterface*)>>("OpenVerctorDiagramsViewAction") =
            [](Frenchie::Application::ImmediateUserInterface*)
        {
            Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
                "assets/views/FrenchieImmediateUserInterfacePlotVectorsView.json",
                std::make_shared<FrenchieImmediateUserInterfacePlotVectorsController>());
        };

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterface*)>>("OpenCustomWidgetAction") =
            [](Frenchie::Application::ImmediateUserInterface*)
        {
            Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
                "assets/views/FrenchieImmediateUserInterfaceCustomWidgetView.json",
                std::make_shared<FrenchieImmediateUserInterfaceCustomWidgetController>());
        };

        // FrenchieImmediateUserInterfaceCustomWidgetController.hpp

        return true;
    }

    virtual void update(Frenchie::Application::ImmediateUserInterfaceViewModel* _Model) override
    {
    }

    virtual void destroy(Frenchie::Application::ImmediateUserInterfaceViewModel* _Model) override
    {
    }
};