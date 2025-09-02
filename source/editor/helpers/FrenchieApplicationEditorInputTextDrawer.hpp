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

                // getters
                std::string get_buffer() const;
                
                // setters
                void set_buffer(const std::string&);

                // predicates
                bool empty() const;

                // API
                void clear();

                bool draw(const std::string&, ImGuiInputTextFlags = 0);

            protected:

                char* m_Buffer     = nullptr;
                int   m_BufferSize = 128;

                // service methods
                static int InputTextResizeCallback(ImGuiInputTextCallbackData*);
            };
        }
    }
}