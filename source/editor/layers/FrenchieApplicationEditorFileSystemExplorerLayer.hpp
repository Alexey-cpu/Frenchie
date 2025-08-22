#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

// Core
#include <FrenchieCoreISerializer.hpp>

// IMGUI
#include <imgui.h>

// STL
#include <map>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class FlileSystemExplorerHelpers
            {
            public:
                FlileSystemExplorerHelpers(const std::string& _Name) : m_Name(_Name)
                {
                    m_CurrentDirectoryBuffer = (char*)std::malloc(sizeof(char) * m_CurrentDirectoryBufferSize);
                    for (int i = 0; i < m_CurrentDirectoryBufferSize; i++) m_CurrentDirectoryBuffer[i] = '\0';
                    m_CurrentDirectoryBuffer[m_CurrentDirectoryBufferSize-1] = '\0';
                }

                ~FlileSystemExplorerHelpers()
                {
                    if(m_CurrentDirectoryBuffer != nullptr)
                        free(m_CurrentDirectoryBuffer);
                }

                bool draw(const std::string& _Input, ImGuiInputTextFlags _Flags = 0)
                {
                    // reallocate buffer to hold an input
                    if(_Input.size() >= m_CurrentDirectoryBufferSize)
                    {
                        m_CurrentDirectoryBufferSize = 2 * (int)_Input.size();
                        m_CurrentDirectoryBuffer = (char*)std::realloc(m_CurrentDirectoryBuffer, m_CurrentDirectoryBufferSize);
                        m_CurrentDirectoryBuffer[m_CurrentDirectoryBufferSize-1] = '\0';
                    }

                    // copy input string a buffer
                    std::strcpy(m_CurrentDirectoryBuffer, _Input.c_str());

                    return ImGui::InputText(
                        "CurrentDirectory", 
                        m_CurrentDirectoryBuffer, 
                        m_CurrentDirectoryBufferSize, 
                        _Flags | ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackResize,
                        FlileSystemExplorerHelpers::InputTextResizeCallback, 
                        this
                    );
                }

                static int InputTextResizeCallback(ImGuiInputTextCallbackData* data)
                {
                    if(data->EventFlag != ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackResize) 
                        return 0;

                    FlileSystemExplorerHelpers* explorer = 
                        reinterpret_cast<FlileSystemExplorerHelpers*>(data->UserData);

                    if(data->BufTextLen < explorer->m_CurrentDirectoryBufferSize - 1) 
                        return 0;

                    explorer->m_CurrentDirectoryBufferSize = 2 * data->BufSize;

                    explorer->m_CurrentDirectoryBuffer = 
                        (char*)std::realloc(explorer->m_CurrentDirectoryBuffer, explorer->m_CurrentDirectoryBufferSize);
                    
                    explorer->m_CurrentDirectoryBuffer[explorer->m_CurrentDirectoryBufferSize-1] = '\0';

                    return 0; // Return 0 to indicate no error
                }

                std::string get_buffer() const
                {
                    return std::string(m_CurrentDirectoryBuffer);
                }

            protected:

                std::string m_Name = "InputText";
                char*       m_CurrentDirectoryBuffer     = nullptr;
                int         m_CurrentDirectoryBufferSize = 128;
            };

            class FlileSystemExplorer : public Layer
            {
            public:
                FlileSystemExplorer();
                virtual ~FlileSystemExplorer();

                // Layer
                virtual void frame_update() override;

            protected:

                // info
                std::map<std::string, bool> m_ForamtFilter = 
                    std::map<std::string, bool>();

                FlileSystemExplorerHelpers m_CurrentDirectory = 
                    FlileSystemExplorerHelpers("CurrentDirectory");
            };
        }
    }
}