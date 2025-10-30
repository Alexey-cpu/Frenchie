#include <FrenchieEditorToolsFontPacker.hpp>

// Core
#include <FrenchieCoreStringUnicode.hpp>

// Application
#include <FrenchieApplicationCommandsLayer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

using namespace Frenchie::Application;
using namespace Frenchie::Editor;

// add sandbox elements into main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenFontPackerTool : 
                public Frenchie::Application::Command::Registry<OpenFontPackerTool, const std::shared_ptr<CommandPayload>&>
            {
            public:

                OpenFontPackerTool(const std::shared_ptr<CommandPayload>& _Payload = nullptr) : 
                    Frenchie::Application::Command::Registry<OpenFontPackerTool, const std::shared_ptr<CommandPayload>&>(_Payload){}
                virtual ~OpenFontPackerTool(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<FontPackerTool>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Tools::Font packer");
                }
            };

            const bool openIPCActionRegistry = OpenFontPackerTool::registerFactory();
        }
    }
}

// FontPackerTool
FontPackerTool::FontPackerTool() :
    Frenchie::Application::Layer("Font packer tool"){}

FontPackerTool::~FontPackerTool(){}

void DebugNodeFontGlyph(ImFont* font, const ImFontGlyph* glyph)
{
    ImGui::Text("Codepoint: U+%04X", glyph->Codepoint);
    ImGui::Separator();
    ImGui::Text("Visible: %d", glyph->Visible);
    ImGui::Text("AdvanceX: %.1f", glyph->AdvanceX);
    ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
    ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
    if (glyph->PackId >= 0)
    {
        ImTextureRect* r = ImFontAtlasPackGetRect(font->ContainerAtlas, glyph->PackId);
        ImGui::Text("PackId: %d (%dx%d rect at %d,%d)", glyph->PackId, r->w, r->h, r->x, r->y);
    }
    ImGui::Text("SourceIdx: %d", glyph->SourceIdx);
}

void FontPackerTool::frame_update()
{
    // retrieve all imgui atlas info
    ImFontBaked* baked = ImGui::GetFontBaked();
    ImFont*      font  = ImGui::GetFont();

    ImGui::Begin(fmt::format("{}###{}", "type here a font name...", get_name()).c_str(), &m_Opened);
    {
        ImGui::BeginChild("Font", ImGui::GetContentRegionAvail());
        {
            ImGui::TextUnformatted(font->GetDebugName());

            // load all glyphs from font
            for (unsigned int base = 0; base <= IM_UNICODE_CODEPOINT_MAX; base++)
            {
                auto glyph = baked->FindGlyph((ImWchar)base);

                if(!baked->IsGlyphLoaded((ImWchar)base))
                    continue;

                ImGui::Text("Codepoint: U+%04X", glyph->Codepoint);
            }

            ImGui::EndChild();
        }
    }

    ImGui::End();
}