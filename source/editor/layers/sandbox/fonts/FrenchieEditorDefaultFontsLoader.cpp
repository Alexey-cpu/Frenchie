#include <FrenchieEditorDefaultFontsLoader.hpp>

// FONTS
#include <NewHardCodedFont.hpp>

// IMGUI
#include <imgui.h>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationConfigurationLoader.hpp>

#include <FrenchieEditorHelpers.hpp>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Configuration;
using namespace Frenchie::Editor;

// add to config loader
namespace Frenchie
{
    namespace Application
    {
        namespace Configuration
        {
            class PushDefaultFontsIntoAppCommandQueue : 
                public Frenchie::Application::Command::Registry<PushDefaultFontsIntoAppCommandQueue, const CommandPayloads&>
            {
            public:

                PushDefaultFontsIntoAppCommandQueue(const CommandPayloads& _Sender) : 
                    Frenchie::Application::Command::Registry<PushDefaultFontsIntoAppCommandQueue, const CommandPayloads&>(_Sender){}
                virtual ~PushDefaultFontsIntoAppCommandQueue(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<DefaultFonts>();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Configuration), STRINGIFY(DefaultFonts));
                }
            };

            const bool pushDefaultFontsIntoAppCommandQueue =
                PushDefaultFontsIntoAppCommandQueue::registerFactory();
        }
    }
}

DefaultFonts::DefaultFonts() :
    Frenchie::Application::Layer(STRINGIFY(DefaultFonts)){}

DefaultFonts::~DefaultFonts(){}

bool DefaultFonts::awake()
{
    ImFontConfig font_cfg = ImFontConfig();
    std::strcpy(font_cfg.Name, "HardCodedFont");

    ImFont* font =
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
            HardCodedFont::BUFFER,
            HardCodedFont::COMPRESSED_SIZE,
            font_cfg.SizePixels,
            &font_cfg,
            ImGui::GetIO().Fonts->GetGlyphRangesCyrillic()
        );

    return true;
}

void DefaultFonts::frame_update(){}