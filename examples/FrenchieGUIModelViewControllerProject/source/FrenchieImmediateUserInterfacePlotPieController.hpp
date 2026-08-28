#pragma once

#include <FrenchieImmediateUserInterfaceModelViewControllerLayer.hpp>

class FrenchieImmediateUserInterfacePlotPieController : public Frenchie::Application::ImmediateUserInterfaceViewController
{
public:

    virtual ~FrenchieImmediateUserInterfacePlotPieController(){}

    virtual bool setup(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
        for (int i = 0; i < 5; i++)
        {
            m_PieNames.push_back(Frenchie::Core::String::format("Sector-%d", i));
            m_PieValues.push_back(5.f * (i + 1));
            m_PieColors.push_back(gs_color_rgba(gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), 255));
        }

        _Model->request<std::string*>("PieNames") = m_PieNames.data();
        _Model->request<float*>("PieValues")      = m_PieValues.data();
        _Model->request<gs_color*>("PieColors")   = m_PieColors.data();
        _Model->request<int>("PieCount")          = m_PieNames.size();
        _Model->request<bool>("ShowLegend")       = true;

        return true;
    }

    virtual void update(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
    }

    virtual void destroy(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
    }

    std::vector<std::string> m_PieNames  {std::vector<std::string>()};
    std::vector<float>       m_PieValues {std::vector<float>()      };
    std::vector<gs_color>    m_PieColors {std::vector<gs_color>()   };
};