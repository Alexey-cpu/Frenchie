#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class FrenchieImmediateUserInterfaceWidgetsTest : public Layer
        {
        public:
            FrenchieImmediateUserInterfaceWidgetsTest();
            virtual ~FrenchieImmediateUserInterfaceWidgetsTest();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const override;

        private:
            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_UI {nullptr};

            // buttons
            bool           m_CheckButtonState           {false};

            // text
            std::string    m_MultilineString            {std::string()};
            std::string    m_MultilineBuffer            {std::string()};
            std::string    m_SinglelineString           {std::string()};
            bool           m_RenderSingleLineAsPassword {false};

            // scalar input
            float          m_FloatScalar                {0.f};
            double         m_DoubleScalar               {0.f};
            long double    m_LongDoubleScalar           {0.f};
            int            m_IntScalar                  {0  };
            short          m_ShortScalar                {0  };
            unsigned int   m_UnsignedIntScalar          {0  };
            unsigned short m_UnsignedShortScalar        {0  };

            // progress bars
            float          m_ProgressValue              {0.f};

            // color pickers
            gs_color       m_ColorPickerColor           {gs_color_rgba(255, 0, 0, 255)};
            bool           m_EnableRGB                  {true};
            bool           m_EnableHSV                  {true};
            bool           m_EnableHSL                  {true};
            bool           m_EnableAlpha                {true};
            bool           m_PreviewColor               {true};
            bool           m_RGBA                       {true};

            // tables
            int            m_ColumnsCount               {1000};
            int            m_RowsCount                  {500 };
            bool           m_RenderColumnHeaders        {false};
            bool           m_RenderRowHeaders           {false};
            bool           m_RenderCornerHeader         {false};
            std::string    m_TableDataCellString        {std::string()};
        };
    }
}