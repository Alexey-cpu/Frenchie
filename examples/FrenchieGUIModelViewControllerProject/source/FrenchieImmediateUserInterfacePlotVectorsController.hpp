#pragma once

#include <FrenchieImmediateUserInterfaceModelViewControllerLayer.hpp>

class FrenchieImmediateUserInterfacePlotVectorsController : public Frenchie::Application::ImmediateUserInterfaceViewController
{
public:

    virtual ~FrenchieImmediateUserInterfacePlotVectorsController(){}

    virtual bool setup(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
        for (int i = 0; i < 5; i++)
        {
            float    angle  = gs_to_radians(30.f * i);
            gs_vec2f vector = gs_vec2f(cos(angle), sin(angle)) * gs_pseudo_random<float>(30.f, 200.f);

            m_Names.push_back(Frenchie::Core::String::format("Sector-%d", i));
            m_Values.push_back(gs_vec4f(0.f, 0.f, vector.x, vector.y));
            m_Colors.push_back(gs_color_rgba(gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), gs_pseudo_random<int>(0, 255), 255));
        }

        _Model->request<std::string*>("DiagramNames") = m_Names.data();
        _Model->request<gs_vec4f*>("DiagramValues")   = m_Values.data();
        _Model->request<gs_color*>("DiagramColors")   = m_Colors.data();
        _Model->request<int>("DiagramCount")          = m_Names.size();
        _Model->request<bool>("ShowLegend")           = true;

        return true;
    }

    virtual void update(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
    }

    virtual void destroy(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
    }

    std::vector<std::string> m_Names  {std::vector<std::string>()};
    std::vector<gs_vec4f>    m_Values {std::vector<gs_vec4f>()   };
    std::vector<gs_color>    m_Colors {std::vector<gs_color>()   };
};