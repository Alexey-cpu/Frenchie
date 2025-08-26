#include <FrenchieApplicationEditorInputTextDrawer.hpp>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

InputText::InputText()
{
    m_Buffer = (char*)std::malloc(sizeof(char) * m_BufferSize);
    for (int i = 0; i < m_BufferSize; i++) 
        m_Buffer[i] = '\0';
}

InputText::~InputText()
{
    if(m_Buffer != nullptr)
        free(m_Buffer);
}

std::string InputText::get_buffer() const
{
    return std::string(m_Buffer);
}

bool InputText::draw(const std::string& _Name, const std::string& _Input, ImGuiInputTextFlags _Flags)
{
    // reallocate buffer to hold an input
    if(_Input.size() >= m_BufferSize)
    {
        m_BufferSize = 2 * (int)_Input.size();
        m_Buffer = (char*)std::realloc(m_Buffer, m_BufferSize);
        for (int i = 0; i < m_BufferSize; i++) 
            m_Buffer[i] = '\0';
    }

    // copy input string an empty buffer
    if(m_Buffer[0] == '\0')
        std::strcpy(m_Buffer, _Input.c_str());

    return ImGui::InputText(
        _Name.c_str(), 
        m_Buffer, 
        m_BufferSize, 
        _Flags | ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackResize,
        InputText::InputTextResizeCallback, 
        this
    );
}

int InputText::InputTextResizeCallback(ImGuiInputTextCallbackData* _Data)
{
    if(_Data->EventFlag != ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackResize) 
        return 0;

    InputText* explorer = 
        reinterpret_cast<InputText*>(_Data->UserData);

    if(_Data->BufTextLen < explorer->m_BufferSize - 1) 
        return 0;

    explorer->m_BufferSize = 2 * _Data->BufSize;

    explorer->m_Buffer = 
        (char*)std::realloc(explorer->m_Buffer, explorer->m_BufferSize);
    
    explorer->m_Buffer[explorer->m_BufferSize-1] = '\0';

    return 0; // Return 0 to indicate no error
}