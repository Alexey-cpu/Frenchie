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
                bool draw(const std::string& _Name, const std::string& _Input, ImGuiInputTextFlags _Flags = 0);

            protected:

                char* m_Buffer     = nullptr;
                int   m_BufferSize = 128;

                // service methods
                static int InputTextResizeCallback(ImGuiInputTextCallbackData*);
            };
        }
    }
}