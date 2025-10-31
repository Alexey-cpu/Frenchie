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
using namespace Frenchie::Editor;

// add to config loader
namespace Frenchie
{
    namespace Application
    {
        namespace Configuration
        {
            class PushDefaultFontsLoaderQueueIntoAppQueue : 
                public Frenchie::Application::Command::Registry<PushDefaultFontsLoaderQueueIntoAppQueue, const CommandPayloads&>
            {
            public:

                PushDefaultFontsLoaderQueueIntoAppQueue(const CommandPayloads& _Sender) : 
                    Frenchie::Application::Command::Registry<PushDefaultFontsLoaderQueueIntoAppQueue, const CommandPayloads&>(_Sender){}
                virtual ~PushDefaultFontsLoaderQueueIntoAppQueue(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<DefaultFontsLoader>();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Configuration), STRINGIFY(Fonts));
                }
            };

            const bool pushDefaultFontsLoaderQueueIntoAppQueue = PushDefaultFontsLoaderQueueIntoAppQueue::registerFactory();
        }
    }
}

DefaultFontsLoader::DefaultFontsLoader() :
    Frenchie::Application::Layer(STRINGIFY(DefaultFontsLoader)){}

DefaultFontsLoader::~DefaultFontsLoader(){}

bool DefaultFontsLoader::awake()
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

void DefaultFontsLoader::frame_update()
{
    // ImGui::Begin("DefaultFontsLoader", &m_Opened);

    // for(int i = 0; i < 10; i++)
    // {    
    //     ImGui::TextUnformatted(
    //         Helpers::convert_imgui_text_char_to_utf8(HardCodedFont::EXCLAMATION).c_str());
    // }

    // ImGui::End();
}