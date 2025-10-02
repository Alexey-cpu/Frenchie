#pragma once

#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        template<typename TimeUnits = std::chrono::milliseconds>
        class SynchronousTimer : public Layer
        {
        public:
            
            SynchronousTimer(
                const double&      _Threshold, 
                const bool&        _AutoReset = true, 
                const std::string& _Name      = STRINGIFY(SynchronousTimer)) :
                Layer(_Name),
                m_ThresholdTime(_Threshold),
                m_AutoReset(_AutoReset){}
            
            virtual ~SynchronousTimer(){}

            // getters
            double get_elapsed_time() const
            {
                return Frenchie::Core::Time::elapsed<TimeUnits>(m_Then, m_Now);
            }

            virtual bool awake() override
            {
                m_Then = Frenchie::Core::Time::tic();
                return true;
            }

            virtual void frame_update() override
            {
                m_Now = Frenchie::Core::Time::tic();

                if(m_AutoReset && get_elapsed_time() > m_ThresholdTime)
                    m_Then = m_Now;
            }

        protected:
            bool                                           m_AutoReset    {true};
            double                                         m_ThresholdTime{0.0};
            std::chrono::high_resolution_clock::time_point m_Then;
            std::chrono::high_resolution_clock::time_point m_Now;
        };
    }
}