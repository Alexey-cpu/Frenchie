#pragma once

#include <FrenchieImmediateUserInterfaceModelViewControllerLayer.hpp>

class FrenchieImmediateUserInterfaceWidgetsController : public Frenchie::Application::ImmediateUserInterfaceViewController
{
public:
    virtual ~FrenchieImmediateUserInterfaceWidgetsController(){}

    virtual bool setup(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
        return true;
    }

    virtual void update(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
    }

    virtual void destroy(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
    }
};