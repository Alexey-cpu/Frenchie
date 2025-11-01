#include <FrenchieEditorToolsFontHardCoder.hpp>

// Core
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreStringUtilities.hpp>

// Application
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationConfigurationLoaderLanguage.hpp>

// Editor
#include <FrenchieEditorFileSystemExplorerDialog.hpp>
#include <FrenchieEditorHelpers.hpp>

// STB
#include "stb_truetype.h"

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Configuration;

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::Tools;

namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenFontPackerTool : 
                public Frenchie::Application::Command::Registry<OpenFontPackerTool, const CommandPayloads&>
            {
            public:

                OpenFontPackerTool(const CommandPayloads& _Payload = CommandPayloads()) : 
                    Frenchie::Application::Command::Registry<OpenFontPackerTool, const CommandPayloads&>(_Payload){}
                virtual ~OpenFontPackerTool(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<FontHardCoderTool>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Tools::Font exporter");
                }
            };

            const bool openIPCActionRegistry = OpenFontPackerTool::registerFactory();
        }
    }
}

namespace Frenchie
{
    namespace Editor
    {
        namespace  Tools
        {
            // stb_compress* from stb.h - declaration
            typedef unsigned int stb_uint;
            typedef unsigned char stb_uchar;

            static stb_uint stb_adler32(stb_uint adler32, stb_uchar *buffer, stb_uint buflen)
            {
                const unsigned long ADLER_MOD = 65521;
                unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
                unsigned long blocklen, i;

                blocklen = buflen % 5552;
                while (buflen) {
                    for (i=0; i + 7 < blocklen; i += 8) {
                        s1 += buffer[0], s2 += s1;
                        s1 += buffer[1], s2 += s1;
                        s1 += buffer[2], s2 += s1;
                        s1 += buffer[3], s2 += s1;
                        s1 += buffer[4], s2 += s1;
                        s1 += buffer[5], s2 += s1;
                        s1 += buffer[6], s2 += s1;
                        s1 += buffer[7], s2 += s1;

                        buffer += 8;
                    }

                    for (; i < blocklen; ++i)
                        s1 += *buffer++, s2 += s1;

                    s1 %= ADLER_MOD, s2 %= ADLER_MOD;
                    buflen -= blocklen;
                    blocklen = 5552;
                }
                return (s2 << 16) + s1;
            }

            static unsigned int stb_matchlen(stb_uchar *m1, stb_uchar *m2, stb_uint maxlen)
            {
                stb_uint i;
                for (i=0; i < maxlen; ++i)
                    if (m1[i] != m2[i]) return i;
                return i;
            }

            // simple implementation that just takes the source data in a big block
            static stb_uchar *stb__out;
            static FILE      *stb__outfile;
            static stb_uint   stb__outbytes;

            static void stb__write(unsigned char v)
            {
                fputc(v, stb__outfile);
                ++stb__outbytes;
            }

            //#define stb_out(v)    (stb__out ? *stb__out++ = (stb_uchar) (v) : stb__write((stb_uchar) (v)))
            #define stb_out(v)    do { if (stb__out) *stb__out++ = (stb_uchar) (v); else stb__write((stb_uchar) (v)); } while (0)

            static void stb_out2(stb_uint v) { stb_out(v >> 8); stb_out(v); }
            static void stb_out3(stb_uint v) { stb_out(v >> 16); stb_out(v >> 8); stb_out(v); }
            static void stb_out4(stb_uint v) { stb_out(v >> 24); stb_out(v >> 16); stb_out(v >> 8 ); stb_out(v); }

            static void outliterals(stb_uchar *in, int numlit)
            {
                while (numlit > 65536) {
                    outliterals(in,65536);
                    in     += 65536;
                    numlit -= 65536;
                }

                if      (numlit ==     0)    ;
                else if (numlit <=    32)    stb_out (0x000020 + numlit-1);
                else if (numlit <=  2048)    stb_out2(0x000800 + numlit-1);
                else /*  numlit <= 65536) */ stb_out3(0x070000 + numlit-1);

                if (stb__out) {
                    memcpy(stb__out,in,numlit);
                    stb__out += numlit;
                } else
                    fwrite(in, 1, numlit, stb__outfile);
            }

            static int stb__window = 0x40000; // 256K

            static int stb_not_crap(int best, int dist)
            {
                return   ((best > 2  &&  dist <= 0x00100)
                    || (best > 5  &&  dist <= 0x04000)
                    || (best > 7  &&  dist <= 0x80000));
            }

            static  stb_uint stb__hashsize = 32768;

            // note that you can play with the hashing functions all you
            // want without needing to change the decompressor
            #define stb__hc(q,h,c)      (((h) << 7) + ((h) >> 25) + q[c])
            #define stb__hc2(q,h,c,d)   (((h) << 14) + ((h) >> 18) + (q[c] << 7) + q[d])
            #define stb__hc3(q,c,d,e)   ((q[c] << 14) + (q[d] << 7) + q[e])

            static unsigned int stb__running_adler;

            static int font_hardcoder_stb_compress_chunk(stb_uchar *history,
                stb_uchar *start,
                stb_uchar *end,
                int length,
                int *pending_literals,
                stb_uchar **chash,
                stb_uint mask)
            {
                (void)history;
                int window = stb__window;
                stb_uint match_max;
                stb_uchar *lit_start = start - *pending_literals;
                stb_uchar *q = start;

            #define STB__SCRAMBLE(h)   (((h) + ((h) >> 16)) & mask)

                // stop short of the end so we don't scan off the end doing
                // the hashing; this means we won't compress the last few bytes
                // unless they were part of something longer
                while (q < start+length && q+12 < end) {
                    int m;
                    stb_uint h1,h2,h3,h4, h;
                    stb_uchar *t;
                    int best = 2, dist=0;

                    if (q+65536 > end)
                        match_max = (stb_uint)(end-q);
                    else
                        match_max = 65536;

            #define stb__nc(b,d)  ((d) <= window && ((b) > 9 || stb_not_crap((int)(b),(int)(d))))

            #define STB__TRY(t,p)  /* avoid retrying a match we already tried */ \
                if (p ? dist != (int)(q-t) : 1)                             \
                if ((m = stb_matchlen(t, q, match_max)) > best)     \
                if (stb__nc(m,q-(t)))                                \
                best = m, dist = (int)(q - (t))

                    // rather than search for all matches, only try 4 candidate locations,
                    // chosen based on 4 different hash functions of different lengths.
                    // this strategy is inspired by LZO; hashing is unrolled here using the
                    // 'hc' macro
                    h = stb__hc3(q,0, 1, 2); h1 = STB__SCRAMBLE(h);
                    t = chash[h1]; if (t) STB__TRY(t,0);
                    h = stb__hc2(q,h, 3, 4); h2 = STB__SCRAMBLE(h);
                    h = stb__hc2(q,h, 5, 6);        t = chash[h2]; if (t) STB__TRY(t,1);
                    h = stb__hc2(q,h, 7, 8); h3 = STB__SCRAMBLE(h);
                    h = stb__hc2(q,h, 9,10);        t = chash[h3]; if (t) STB__TRY(t,1);
                    h = stb__hc2(q,h,11,12); h4 = STB__SCRAMBLE(h);
                    t = chash[h4]; if (t) STB__TRY(t,1);

                    // because we use a shared hash table, can only update it
                    // _after_ we've probed all of them
                    chash[h1] = chash[h2] = chash[h3] = chash[h4] = q;

                    if (best > 2)
                        assert(dist > 0);

                    // see if our best match qualifies
                    if (best < 3) { // fast path literals
                        ++q;
                    } else if (best > 2  &&  best <= 0x80    &&  dist <= 0x100) {
                        outliterals(lit_start, (int)(q-lit_start)); lit_start = (q += best);
                        stb_out(0x80 + best-1);
                        stb_out(dist-1);
                    } else if (best > 5  &&  best <= 0x100   &&  dist <= 0x4000) {
                        outliterals(lit_start, (int)(q-lit_start)); lit_start = (q += best);
                        stb_out2(0x4000 + dist-1);
                        stb_out(best-1);
                    } else if (best > 7  &&  best <= 0x100   &&  dist <= 0x80000) {
                        outliterals(lit_start, (int)(q-lit_start)); lit_start = (q += best);
                        stb_out3(0x180000 + dist-1);
                        stb_out(best-1);
                    } else if (best > 8  &&  best <= 0x10000 &&  dist <= 0x80000) {
                        outliterals(lit_start, (int)(q-lit_start)); lit_start = (q += best);
                        stb_out3(0x100000 + dist-1);
                        stb_out2(best-1);
                    } else if (best > 9                      &&  dist <= 0x1000000) {
                        if (best > 65536) best = 65536;
                        outliterals(lit_start, (int)(q-lit_start)); lit_start = (q += best);
                        if (best <= 0x100) {
                            stb_out(0x06);
                            stb_out3(dist-1);
                            stb_out(best-1);
                        } else {
                            stb_out(0x04);
                            stb_out3(dist-1);
                            stb_out2(best-1);
                        }
                    } else {  // fallback literals if no match was a balanced tradeoff
                        ++q;
                    }
                }

                // if we didn't get all the way, add the rest to literals
                if (q-start < length)
                    q = start+length;

                // the literals are everything from lit_start to q
                *pending_literals = (int)(q - lit_start);

                stb__running_adler = stb_adler32(stb__running_adler, start, (stb_uint)(q - start));
                return (int)(q - start);
            }

            static int font_hardcoder_stb_compress_inner(stb_uchar *input, stb_uint length)
            {
                int literals = 0;
                stb_uint len,i;

                stb_uchar **chash;
                chash = (stb_uchar**) malloc(stb__hashsize * sizeof(stb_uchar*));
                if (chash == nullptr) return 0; // failure
                for (i=0; i < stb__hashsize; ++i)
                    chash[i] = nullptr;

                // stream signature
                stb_out(0x57); stb_out(0xbc);
                stb_out2(0);

                stb_out4(0);       // 64-bit length requires 32-bit leading 0
                stb_out4(length);
                stb_out4(stb__window);

                stb__running_adler = 1;

                len = font_hardcoder_stb_compress_chunk(input, input, input+length, length, &literals, chash, stb__hashsize-1);
                assert(len == length);

                outliterals(input+length - literals, literals);

                free(chash);

                stb_out2(0x05fa); // end opcode

                stb_out4(stb__running_adler);

                return 1; // success
            }

            stb_uint font_hardcoder_stb_compress(stb_uchar *out, stb_uchar *input, stb_uint length)
            {
                stb__out = out;
                stb__outfile = nullptr;

                font_hardcoder_stb_compress_inner(input, length);

                return (stb_uint)(stb__out - out);
            }

            static bool font_hardcoder_binary_to_compressed_cpp(
                std::filesystem::path                       _FontFilePath,
                std::filesystem::path                       _CppFilePath,
                std::string                                 _ClassName,
                std::vector<FontHardCoderToolCodePointInfo> _CodePointsMap,
                FontHardCoderToolEncoding                   _Encoding,
                bool                                        _Compress)
            {
                // Read file
                FILE* f = fopen(Frenchie::Core::String::convert_utf32_to_utf8(_FontFilePath.u32string()).c_str(), "rb");
                if (!f) return false;
                int data_sz;
                if (fseek(f, 0, SEEK_END) || (data_sz = (int)ftell(f)) == -1 || fseek(f, 0, SEEK_SET)) { fclose(f); return false; }
                char* data = new char[data_sz + 4];
                if (fread(data, 1, data_sz, f) != (size_t)data_sz) { fclose(f); delete[] data; return false; }
                memset((void*)(((char*)data) + data_sz), 0, 4);
                fclose(f);

                // Compress
                int maxlen = data_sz + 512 + (data_sz >> 2) + sizeof(int); // total guess
                char* compressed = _Compress ? new char[maxlen] : data;
                int compressed_sz = _Compress ? font_hardcoder_stb_compress((stb_uchar*)compressed, (stb_uchar*)data, data_sz) : data_sz;
                if (_Compress)
                    memset(compressed + compressed_sz, 0, maxlen - compressed_sz);

                // Write file
                FILE* out = fopen(Frenchie::Core::String::convert_utf32_to_utf8(_CppFilePath.u32string()).c_str(), "wb");
                fprintf(out, "// File: '%s' (%d bytes)\n", Frenchie::Core::String::convert_utf32_to_utf8(_FontFilePath.u32string()).c_str(), (int)data_sz);
                fprintf(out, "class %s\n{", _ClassName.c_str());
                fprintf(out, "\npublic:\n");
                
                for (auto&& codePointMapItem : _CodePointsMap)
                {
                    auto codePoint     = codePointMapItem.Code;
                    auto codePointName = Frenchie::Core::String::utf8_to_upper(codePointMapItem.Name);

                    if(codePointName.empty())
                        continue;

                    fprintf(out, "\tstatic constexpr unsigned int %s = %#08x;\n\n", codePointName.c_str(), codePoint);
                }

                fprintf(out, "\tstatic constexpr char* NAME = \"%s\";\n\n", _ClassName.c_str());

                if (_Encoding == HardCoderToolEncoding_UTF8)
                {
                    fprintf(out, "\tstatic constexpr unsigned int  COMPRESSED_SIZE = %d;\n\n", (int)compressed_sz);
                    fprintf(out, "\tstatic constexpr unsigned char BUFFER[%d] =\n\t{", (int)compressed_sz);
                    int column = 0;
                    for (int i = 0; i < compressed_sz; i++)
                    {
                        unsigned char d = *(unsigned char*)(compressed + i);
                        if (column == 0)
                            fprintf(out, "\n\t\t");
                        column += fprintf(out, "%d,", d);
                        if (column >= 180)
                            column = 0;
                    }
                    fprintf(out, "\n\t};");
                }
                else if (_Encoding == HardCoderToolEncoding_UTF32)
                {
                    fprintf(out, "\tstatic constexpr unsigned int COMPRESSED_SIZE = %d;\n\n", (int)compressed_sz);
                    fprintf(out, "\tstatic const unsigned char BUFFER[%d] =\n\t{", (int)((compressed_sz + 3) / 4) * 4);
                    int column = 0;
                    for (int i = 0; i < compressed_sz; i += 4)
                    {
                        unsigned int d = *(unsigned int*)(compressed + i);
                        if ((column++ % 14) == 0)
                            fprintf(out, "\n\t\t0x%08x, ", d);
                        else
                            fprintf(out, "0x%08x, ", d);
                    }
                    fprintf(out, "\n\t};");
                }

                fprintf(out, "\n};");

                // Cleanup
                delete[] data;
                if (_Compress)
                    delete[] compressed;

                // close outfile
                fclose(out);

                return true;
            }
        
            static std::vector<FontHardCoderToolCodePointInfo> font_hardcoder_retrieve_codepoints(const std::filesystem::path& _FontFilePath)
            {
                FILE* file = fopen(Frenchie::Core::String::convert_utf32_to_utf8(_FontFilePath.u32string()).c_str(), "rb");

                std::vector<FontHardCoderToolCodePointInfo> codepoints;

                if (file == NULL) 
                {
                    perror("Error opening file");
                    return codepoints;
                }

                // Determine file size
                fseek(file, 0, SEEK_END);
                int file_size = ftell(file);
                fseek(file, 0, SEEK_SET);

                // Allocate memory for the buffer
                unsigned char* buffer = (unsigned char*)malloc(file_size);

                if (buffer == NULL) 
                {
                    fclose(file);
                    return codepoints;
                }

                // Read the file into the buffer
                size_t bytes_read = fread(buffer, 1, file_size, file);

                if (bytes_read != file_size) 
                {
                    free(buffer);
                    fclose(file);
                    return codepoints;
                }

                // close file
                fclose(file);

                stbtt_fontinfo font;

                stbtt_InitFont(&font, buffer, stbtt_GetFontOffsetForIndex(buffer,0));

                for (unsigned int codepoint = 0; codepoint <= IM_UNICODE_CODEPOINT_MAX; codepoint++)
                {
                    if(stbtt_FindGlyphIndex(&font, codepoint))
                        codepoints.push_back({codepoint, std::string()});
                }

                return codepoints;
            }

            static int font_hardcoder_text_edit_callback(ImGuiInputTextCallbackData* data)
            {
                if (data->EventChar == '!'  ||
                    data->EventChar == '@'  ||
                    data->EventChar == '#'  ||
                    data->EventChar == '&'  ||
                    data->EventChar == '('  ||
                    data->EventChar == ')'  ||
                    data->EventChar == '{'  ||
                    data->EventChar == '}'  ||
                    data->EventChar == '-'  ||
                    data->EventChar == '+'  ||
                    data->EventChar == '='  ||
                    data->EventChar == '^'  ||
                    data->EventChar == '%'  ||
                    data->EventChar == '"'  ||
                    data->EventChar == '\'' ||
                    data->EventChar == '<'  ||
                    data->EventChar == '>'  ||
                    data->EventChar == '\\' ||
                    data->EventChar == '/'  ||
                    data->EventChar == ':'  ||
                    data->EventChar == ':'  ||
                    data->EventChar == '$')
                {
                    return 1; // Return 1 to signal that the character should be rejected
                }
                return 0; // Return 0 to allow the character
            }

            #undef stb__hc
            #undef stb__hc2
            #undef stb__hc3
            #undef stb_out        
        }
    }
}

// FontPackerTool
FontHardCoderTool::FontHardCoderTool() :
    Frenchie::Application::Layer("Font exporter"){}

FontHardCoderTool::~FontHardCoderTool(){}

void FontHardCoderTool::frame_update()
{
    ImGui::Begin(fmt::format("{}###Font exporter", Translator::translate(get_name())).c_str(), &m_Opened);
    {
        if(ImGui::Button(
            std::filesystem::exists(m_TTF) ?
                Frenchie::Core::String::convert_utf32_to_utf8(m_TTF.u32string()).c_str() :
                Translator::translate("No .ttf file selected").c_str(),
                ImVec2(ImGui::GetContentRegionAvail().x, 0.f)))
        {
            on_ttf_file_path_search_button_pressed();
        }

        if(ImGui::Button(
            !m_CPP.empty() && std::filesystem::exists(m_CPP.parent_path()) ?
                Frenchie::Core::String::convert_utf32_to_utf8(m_CPP.u32string()).c_str() :
                Translator::translate("No .cpp file export path set").c_str(),
                ImVec2(ImGui::GetContentRegionAvail().x, 0.f)))
        {
            on_cpp_file_path_search_button_pressed();
        }

        ImGui::InputText(
            Translator::translate("Class (functions prefix) name").c_str(),
            &m_ClassName,
            ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCharFilter,
            Tools::font_hardcoder_text_edit_callback);

        if(ImGui::BeginCombo(
            Translator::translate("Encoding").c_str(),
            encoding_to_string(m_Encoding).c_str()))
        {
            if(ImGui::Button(encoding_to_string(FontHardCoderToolEncoding::HardCoderToolEncoding_UTF8).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f)))
                m_Encoding = FontHardCoderToolEncoding::HardCoderToolEncoding_UTF8;

            if(ImGui::Button( encoding_to_string(FontHardCoderToolEncoding::HardCoderToolEncoding_UTF32).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f)))
                m_Encoding = FontHardCoderToolEncoding::HardCoderToolEncoding_UTF32;

            ImGui::EndCombo();
        }

        ImGui::Checkbox(Translator::translate("Compress").c_str(), &m_Compress);

        if(m_FontInfo.Font != nullptr)
        {
            if(ImGui::Button(
                Translator::translate("Export").c_str(),
                ImVec2(ImGui::GetContentRegionAvail().x, 0.f)))
            {
                on_font_hardcode_button_pressed();
            }

            if(ImGui::BeginTable("FileSystemContentTable",
                4,
                ImGuiTableFlags_::ImGuiTableFlags_ScrollY      |
                ImGuiTableFlags_::ImGuiTableFlags_RowBg        |
                ImGuiTableFlags_::ImGuiTableFlags_BordersOuter |
                ImGuiTableFlags_::ImGuiTableFlags_BordersV     |
                ImGuiTableFlags_::ImGuiTableFlags_Resizable    |
                ImGuiTableFlags_::ImGuiTableFlags_Reorderable  |
                ImGuiTableFlags_::ImGuiTableFlags_Hideable))
            {
                ImGui::TableNextRow();

                int id  = 0;
                int idx = 0;

                // draw codepoints
                for(auto&& codePoint : m_FontInfo.Codes)
                {
                    ImGui::TableNextRow();

                    // code point
                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushID(id++);
                    ImGui::Text("%d", idx++);
                    ImGui::PopID();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushID(id++);
                    ImGui::Text("U+%04X", codePoint.Code);
                    ImGui::PopID();

                    // glyph
                    ImGui::TableSetColumnIndex(2);
                    ImGui::PushFont(m_FontInfo.Font);
                    ImGui::PushID(id++);
                    ImGui::TextUnformatted(Helpers::convert_imgui_text_char_to_utf8(codePoint.Code).c_str());
                    ImGui::PopFont();
                    ImGui::PopID();

                    // constant name
                    ImGui::TableSetColumnIndex(3);
                    ImGui::PushID(id++);
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputText("##", &codePoint.Name);
                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        }
    }

    ImGui::End();
}

void FontHardCoderTool::finish()
{
    // remove font
    if(m_FontInfo.Font == nullptr)
        return;

    ImGui::GetIO().Fonts->RemoveFont(m_FontInfo.Font);
    m_FontInfo.Font = nullptr;
    m_FontInfo.Codes.clear();
}

void FontHardCoderTool::on_ttf_file_path_search_button_pressed()
{
    auto dialog = Frenchie::Application::application()->push_layer<FileSystem::ExplorerDialog>(
        Translator::translate("Select .ttf file ...")
    );

    dialog->on_accepted(
        [this]()
        {
            Frenchie::Core::Reference<FileSystem::ExplorerDialog> dialog = 
                Frenchie::Application::application()->find_layer<FileSystem::ExplorerDialog>();

            if(dialog == nullptr) 
                return;

            m_TTF = dialog->get_current_file();

            // load font
            Frenchie::Application::CommandsQueue::instance()->push<CallbackCommand>(
                [this]()
                {
                    // remove previous font
                    if(m_FontInfo.Font != nullptr)
                    {
                        ImGui::GetIO().Fonts->RemoveFont(m_FontInfo.Font);
                        m_FontInfo.Font = nullptr;
                        m_FontInfo.Codes.clear();
                    }

                    // load font
                    m_FontInfo.Font = ImGui::GetIO().Fonts->AddFontFromFileTTF(
                        Frenchie::Core::String::convert_utf32_to_utf8(m_TTF.u32string()).c_str(),
                        ImGui::GetStyle().FontSizeBase,
                        nullptr,
                        ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

                    ImGui::GetIO().Fonts->Build();
                    Frenchie::Application::application()->reload();

                    // load font glyphs codepoints
                    m_FontInfo.Codes = Tools::font_hardcoder_retrieve_codepoints(m_TTF);
                }
            );
        }
    );
}

void FontHardCoderTool::on_cpp_file_path_search_button_pressed()
{
    auto dialog = Frenchie::Application::application()->push_layer<FileSystem::ExplorerDialog>(
        Translator::translate("Set save file name for .cpp file ...")
    );

    dialog->on_accepted(
        [this]()
        {
            Frenchie::Core::Reference<FileSystem::ExplorerDialog> dialog = 
                Frenchie::Application::application()->find_layer<FileSystem::ExplorerDialog>();

            if(dialog == nullptr) return;

            m_CPP = dialog->get_current_file();
            m_ClassName = Frenchie::Core::String::utf8_remove_symbols(
                Frenchie::Core::String::convert_utf32_to_utf8(m_CPP.u32string()),
                {'!','@','#','&','(',')','{','}','-','+','=','^','%','"','\'','<','>','\\','/',':',':','$'}
            );
        }
    );
}

void FontHardCoderTool::on_font_hardcode_button_pressed()
{
    if(!std::filesystem::exists(m_TTF))
    {
        // TODO add log here !!!
    }
    else if(!std::filesystem::exists(m_CPP.parent_path()))
    {
        // TODO: add log here !!!
    }
    else
    {
        // hardcode font
        Frenchie::Editor::Tools::font_hardcoder_binary_to_compressed_cpp(
            m_TTF,
            m_CPP,
            m_ClassName,
            m_FontInfo.Codes,
            m_Encoding,
            m_Compress
        );
    }
}

std::string FontHardCoderTool::encoding_to_string(const FontHardCoderToolEncoding& _Encoding)
{
    if(_Encoding == FontHardCoderToolEncoding::HardCoderToolEncoding_UTF8)
        return "UTF-8";

    if(_Encoding == FontHardCoderToolEncoding::HardCoderToolEncoding_UTF32)
        return "UTF-32";

    return "UNKNOWN";
}