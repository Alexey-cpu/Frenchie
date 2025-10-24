#pragma once

#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrameCounter : public Layer
        {
        public:
            
            FrameCounter(
                const int&         _Threshold, 
                const std::string& _Name = STRINGIFY(FrameCounter)) :
                Layer(_Name),
                m_Threshold(_Threshold){}
            
            virtual ~FrameCounter(){}

            // getters
            int get_frames_count() const
            {
                return m_FramesCount;
            }

            virtual bool awake() override
            {
                m_FramesCount = 0;
                return true;
            }

            virtual void frame_update() override
            {
                m_FramesCount = ++m_FramesCount % m_Threshold;
            }

        protected:
            int  m_Threshold  {0};
            int  m_FramesCount{0};
        };
    }
}