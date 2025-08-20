#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreHelpers.hpp>

namespace Frenchie
{
    namespace Application
    {
        class TimeProviderLayer : public Layer
        {
        public:
            TimeProviderLayer();
            virtual ~TimeProviderLayer();

            // getters
            double get_time_delta() const;
            double get_time() const;

            // Layer
            virtual void on_frame_update() override;

        protected:
            
            double m_PreviousFrame = 0.0;
            double m_CurrentFrame  = 0.0;
        };
    }
}