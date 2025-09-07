#pragma once

// IMGUI
#include <imgui.h>

// STL
#include <string>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class InputText
            {
            public:

                InputText();
                ~InputText();

                std::string get_buffer() const;
                void set_buffer(const std::string&) const;
                bool empty() const;
                void clear();
                bool draw(const std::string&, ImGuiInputTextFlags = 0);
                bool draw_multiline(const std::string&, ImGuiInputTextFlags = 0, float _Width = 0.f, float _Height = 0.f);

            protected:
                mutable char* m_Buffer     = nullptr;
                mutable int   m_BufferSize = 128;

            private:
                static int InputTextResizeCallback(ImGuiInputTextCallbackData*);
            };
        }
    }
}