#pragma once

#include <FrenchieImmediateUserInterfaceModelViewControllerLayer.hpp>

class FrenchieImmediateUserInterfaceWidgetsController : public Frenchie::Application::ImmediateUserInterfaceViewController
{
public:
    virtual ~FrenchieImmediateUserInterfaceWidgetsController(){}

    virtual bool setup(Frenchie::Application::ImmediateUserInterfaceViewModel* _Model) override
    {
        return true;
    }

    virtual void update(Frenchie::Application::ImmediateUserInterfaceViewModel* _Model) override
    {
    }

    virtual void destroy(Frenchie::Application::ImmediateUserInterfaceViewModel* _Model) override
    {
    }
};