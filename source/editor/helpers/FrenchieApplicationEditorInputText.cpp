#include <FrenchieApplicationEditorInputText.hpp>

#include <FrenchieCoreLogger.hpp>

// using namespace Frenchie::Application;
// using namespace Frenchie::Application::Editor;

// InputText::InputText(){}

// InputText::~InputText()
// {
//     clear();
// }

// std::string InputText::get_buffer() const
// {
//     if(empty()) 
//         set_buffer("");

//     return std::string(m_Buffer);
// }

// void InputText::set_buffer(const std::string& _Input) const
// {
//     // allocate buffer if it's nullptr
//     if(m_Buffer == nullptr)
//     {
//         m_Buffer = (char*)std::malloc(std::max<size_t>(m_BufferSize, _Input.size()) * sizeof(char) * 2);
//         for (int i = 0; i < m_BufferSize; i++) 
//             m_Buffer[i] = '\0';
//     }
//     else if(_Input.size() >= m_BufferSize)
//     {
//         m_BufferSize = 2 * (int)_Input.size();
//         m_Buffer = (char*)std::realloc(m_Buffer, sizeof(char) * m_BufferSize);
//         for (int i = 0; i < m_BufferSize; i++) 
//             m_Buffer[i] = '\0';
//     }

//     std::strcpy(m_Buffer, _Input.c_str());
// }

// void InputText::clear()
// {
//     if(m_Buffer != nullptr)
//         free(m_Buffer);
//     m_Buffer = nullptr;
// }

// bool InputText::empty() const
// {
//     return m_Buffer == nullptr || m_Buffer[0] == '\0';
// }

// bool InputText::draw(const std::string& _Name, ImGuiInputTextFlags _Flags)
// {
//     if(empty()) 
//         set_buffer("");

//     return ImGui::InputText(
//         _Name.c_str(), 
//         m_Buffer, 
//         m_BufferSize, 
//         _Flags | ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackResize,
//         InputText::InputTextResizeCallback, 
//         this
//     );
// }

// bool InputText::draw_multiline(const std::string& _Name, ImGuiInputTextFlags _Flags, float _Width, float _Height)
// {
//     if(empty()) 
//         set_buffer("");

//     return ImGui::InputTextMultiline(
//         _Name.c_str(), 
//         m_Buffer, 
//         m_BufferSize,
//         ImVec2(_Width, _Height),
//         _Flags | ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackResize,
//         InputText::InputTextResizeCallback, 
//         this
//     );
// }

// int InputText::InputTextResizeCallback(ImGuiInputTextCallbackData* _Data)
// {
//     if(_Data->EventFlag != ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackResize) 
//         return 0;

//     InputText* explorer = 
//         reinterpret_cast<InputText*>(_Data->UserData);

//     if(_Data->BufTextLen < explorer->m_BufferSize - 1) 
//         return 0;

//     explorer->m_BufferSize = 2 * _Data->BufSize;

//     explorer->m_Buffer = 
//         (char*)std::realloc(explorer->m_Buffer, explorer->m_BufferSize);

//     return 0; // Return 0 to indicate no error
// }