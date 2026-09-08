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

    virtual bool setup(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("OpenWidgetsViewAction") =
            [](Frenchie::Application::ImmediateUserInterfaceContextLayer*)
        {
            Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
                "assets/views/FrenchieImmediateUserInterfaceWidgetsView.json",
                std::make_shared<FrenchieImmediateUserInterfaceWidgetsController>());
        };
        
        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("OpenPlots2DViewAction") =
            [](Frenchie::Application::ImmediateUserInterfaceContextLayer*)
        {
            Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
                "assets/views/FrenchieImmediateUserInterfacePlotLine2DView.json",
                std::make_shared<FrenchieImmediateUserInterfacePlotLine2DController>());
        };

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("OpenPieChartsViewAction") =
            [](Frenchie::Application::ImmediateUserInterfaceContextLayer*)
        {
            Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
                "assets/views/FrenchieImmediateUserInterfacePlotPieView.json",
                std::make_shared<FrenchieImmediateUserInterfacePlotPieController>());
        };

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("OpenVerctorDiagramsViewAction") =
            [](Frenchie::Application::ImmediateUserInterfaceContextLayer*)
        {
            Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
                "assets/views/FrenchieImmediateUserInterfacePlotVectorsView.json",
                std::make_shared<FrenchieImmediateUserInterfacePlotVectorsController>());
        };

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("OpenCustomWidgetAction") =
            [](Frenchie::Application::ImmediateUserInterfaceContextLayer*)
        {
            Frenchie::Application::App::push_layer<Frenchie::Application::ImmediateUserInterfaceModelViewControllerLayer>(
                "assets/views/FrenchieImmediateUserInterfaceCustomWidgetView.json",
                std::make_shared<FrenchieImmediateUserInterfaceCustomWidgetController>());
        };

        // FrenchieImmediateUserInterfaceCustomWidgetController.hpp

        return true;
    }

    virtual void update(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
    }

    virtual void destroy(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
    }
};