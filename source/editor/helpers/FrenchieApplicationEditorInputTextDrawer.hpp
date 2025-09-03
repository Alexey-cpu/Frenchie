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
                void set_buffer(const std::string&);
                bool empty() const;
                void clear();
                bool draw(const std::string&, ImGuiInputTextFlags = 0);

            protected:
                char* m_Buffer     = nullptr;
                int   m_BufferSize = 128;

            private:
                static int InputTextResizeCallback(ImGuiInputTextCallbackData*);
            };
        }
    }
}