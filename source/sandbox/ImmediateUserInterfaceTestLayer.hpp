#pragma once

#include <FrenchieImmediateUserInterfaceLayer.hpp>

#include <filesystem>

namespace Frenchie
{
    namespace Application
    {
        // This is the test layer state
        struct ImmediateUserInterfaceTestLayerState
        {
            // context settings
            bool ShowContextSettings            = false;
            bool ShowContextOverlay             = true;
            bool ContextWindowsDockingEnabled   = true;
            bool ContextWorkspaceDockingEnabled = true;
            bool ContextHighlightHoveredNodes   = true;

            // style settings
            bool ShowStyleSettings             = false;
            bool showColorPciker               = false;

            // layout configurator
            float       TopMargin                         = 0.f;
            float       LeftMargin                        = 0.f;
            float       RightMargin                       = 0.f;
            float       BottomMargin                      = 0.f;
            float       TopPadding                        = 0.f;
            float       LeftPadding                       = 0.f;
            float       RightPadding                      = 0.f;
            float       BottomPadding                     = 0.f;
            int         HorizontalAlignmentSettings       = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter;
            int         VerticalAlignmentSettings         = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_None | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter;
            std::string HorizontalAligmentComboboxPreview = "None";
            std::string VerticalAligmentComboboxPreview   = "None";

            // scalar input
            float          FloatScalar         = 0.f;
            double         DoubleScalar        = 0.0;
            long double    LongDoubleScalar    = 0.0;
            int            IntScalar           = 0;
            short          ShortScalar         = 0;
            unsigned int   UnsignedIntScalar   = 0;
            unsigned short UnsignedShortScalar = 0;

            // color pickers
            gs_color ColorPickerColor = gs_color_rgba(255, 0, 0, 255);
            bool     EnableRGB        = true;
            bool     EnableHSV        = true;
            bool     EnableHSL        = true;
            bool     EnableAlpha      = true;
            bool     PreviewColor     = true;
            bool     RGBA             = true;

            // textual input
            std::string MultilineString;
            std::string MultilineBuffer;
            std::string SinglelineString;
            bool        RenderSingleLineAsPassword;
        };

        // This is the test layer that demonstaits main features Frenchie immediate mode GUI library
        class ImmediateUserInterfaceTestLayer : public Layer
        {
        public:
            ImmediateUserInterfaceTestLayer();
            virtual ~ImmediateUserInterfaceTestLayer();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual void finish() override;

            void widgets_test();
            void develop_test();

        protected:

            ImmediateUserInterfaceTestLayerState State;

            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_UI{nullptr};
        };
    }
}