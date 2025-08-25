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

                InputText(const std::string& _Name) : m_Name(_Name)
                {
                    m_Buffer = (char*)std::malloc(sizeof(char) * m_BufferSize);
                    for (int i = 0; i < m_BufferSize; i++) m_Buffer[i] = '\0';
                    m_Buffer[m_BufferSize-1] = '\0';
                }

                ~InputText()
                {
                    if(m_Buffer != nullptr)
                        free(m_Buffer);
                }

                std::string get_buffer() const
                {
                    return std::string(m_Buffer);
                }

                bool draw(const std::string& _Input, ImGuiInputTextFlags _Flags = 0)
                {
                    // reallocate buffer to hold an input
                    if(_Input.size() >= m_BufferSize)
                    {
                        m_BufferSize = 2 * (int)_Input.size();
                        m_Buffer = (char*)std::realloc(m_Buffer, m_BufferSize);
                        m_Buffer[m_BufferSize-1] = '\0';
                    }

                    // copy input string a buffer
                    std::strcpy(m_Buffer, _Input.c_str());

                    return ImGui::InputText(
                        m_Name.c_str(), 
                        m_Buffer, 
                        m_BufferSize, 
                        _Flags | ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackResize,
                        InputText::InputTextResizeCallback, 
                        this
                    );
                }

            protected:

                std::string m_Name       = "InputText";
                char*       m_Buffer     = nullptr;
                int         m_BufferSize = 128;

                // service methods
                static int InputTextResizeCallback(ImGuiInputTextCallbackData* data)
                {
                    if(data->EventFlag != ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackResize) 
                        return 0;

                    InputText* explorer = 
                        reinterpret_cast<InputText*>(data->UserData);

                    if(data->BufTextLen < explorer->m_BufferSize - 1) 
                        return 0;

                    explorer->m_BufferSize = 2 * data->BufSize;

                    explorer->m_Buffer = 
                        (char*)std::realloc(explorer->m_Buffer, explorer->m_BufferSize);
                    
                    explorer->m_Buffer[explorer->m_BufferSize-1] = '\0';

                    return 0; // Return 0 to indicate no error
                }
            };
        }
    }
}