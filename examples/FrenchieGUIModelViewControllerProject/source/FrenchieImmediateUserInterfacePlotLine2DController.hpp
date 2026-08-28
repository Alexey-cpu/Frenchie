#pragma once

#include <FrenchieImmediateUserInterfaceModelViewControllerLayer.hpp>

class FrenchieImmediateUserInterfacePlotLine2DController : public Frenchie::Application::ImmediateUserInterfaceViewController
{
public:

    virtual ~FrenchieImmediateUserInterfacePlotLine2DController(){}

    virtual bool setup(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
        // generate data
        float fn = 50.f;
        float Ns = 80.f;
        float fs = fn * Ns;
        float Ts = 1.f / fs;
        float T  = 20.f / 1000.f;
        int   nn = 0;

        float A = gs_pseudo_random<float>(0, 100);
        float min = gs_huge<float>();
        float max = gs_tiny<float>();

        for (int n = 0; n <= (int)(T * fs) * 1; n++)
        {
            if(nn >= gs_pseudo_random<float>(0, 4))
            {
                nn = 0;
                continue;
            }

            m_YValues.push_back(A * sin(PI2 * fn * Ts * (float)n + gs_to_radians(30.f)));
            m_XValues.push_back(n);

            max = gs_max(m_YValues[m_YValues.size() - 1], max);
            min = gs_min(m_YValues[m_YValues.size() - 1], min);

            nn++;
        }

        _Model->request<float>("XAxisMin")       = 0.f;
        _Model->request<float>("XAxisMax")       = (float)m_XValues.size();
        _Model->request<float>("YAxisMin")       = min;
        _Model->request<float>("YAxisMax")       = max;
        _Model->request<float*>("XYPlotXValues") = m_XValues.data();
        _Model->request<float*>("XYPlotYValues") = m_YValues.data();
        _Model->request<int>("XYPlotCount")      = std::max<int>(m_XValues.size(), m_YValues.size());
        _Model->request<gs_color>("LineColor")   = gs_color_rgb(255, 0, 0);
        _Model->request<std::string>("PlotType") = "stems";

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("SetPlotTypeStems") =
            [&_Model](Frenchie::Application::ImmediateUserInterfaceContextLayer*)
            {
                _Model->request<std::string>("PlotType") = "stems";
            };

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("SetPlotTypeAreas") =
            [&_Model](Frenchie::Application::ImmediateUserInterfaceContextLayer*)
            {
                _Model->request<std::string>("PlotType") = "areas";
            };

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("SetPlotTypeLines") =
            [&_Model](Frenchie::Application::ImmediateUserInterfaceContextLayer*)
            {
                _Model->request<std::string>("PlotType") = "lines";
            };

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("SetPlotTypePoints") =
            [&_Model](Frenchie::Application::ImmediateUserInterfaceContextLayer*)
            {
                _Model->request<std::string>("PlotType") = "points";
            };

        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("SetPlotTypeRectangles") =
            [&_Model](Frenchie::Application::ImmediateUserInterfaceContextLayer*)
            {
                _Model->request<std::string>("PlotType") = "rectangles";
            };

        return true;
    }

    virtual void update(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
    }

    virtual void destroy(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
    }

    std::vector<float> m_XValues{std::vector<float>()};
    std::vector<float> m_YValues{std::vector<float>()};
};