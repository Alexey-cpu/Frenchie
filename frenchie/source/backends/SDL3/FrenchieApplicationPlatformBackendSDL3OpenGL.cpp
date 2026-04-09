#include <FrenchieApplication.hpp>

#include <FrenchieCoreStringUtilities.hpp>

#include <FrenchieApplicationPlatformBackend.hpp>

using namespace Frenchie::Application;

#include "SDL3/SDL.h"

namespace Frenchie
{
    namespace Application
    {
        struct FrenchieApplicationPlatformSDL3 : public FrenchieApplicationPlatformApi
        {
            FrenchieApplicationPlatformSDL3(){}
            virtual ~FrenchieApplicationPlatformSDL3(){}

            SDL_Event     Event;
            SDL_GLContext Context;
        };

        class SDLApplicationInputHandler
        {
        public:

            static ApplicationPlatformBackendMouseButton::Button glfw_mouse_button_to_application_mouse_button(int _MouseButton)
            {
                switch (_MouseButton)
                {
                case SDL_BUTTON_LEFT:
                    return ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonLeft;
                case SDL_BUTTON_RIGHT:
                    return ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonRight;
                case SDL_BUTTON_MIDDLE:
                    return ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonMiddle;
                }

                return ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd;
            }

            static ApplicationPlatformBackendKey::Key glfw_key_to_application_key(SDL_Keycode keycode, SDL_Scancode scancode)
            {
                // Keypad doesn't have individual key values in SDL3
                switch (scancode)
                {
                    case SDL_SCANCODE_KP_0: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad0;
                    case SDL_SCANCODE_KP_1: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad1;
                    case SDL_SCANCODE_KP_2: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad2;
                    case SDL_SCANCODE_KP_3: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad3;
                    case SDL_SCANCODE_KP_4: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad4;
                    case SDL_SCANCODE_KP_5: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad5;
                    case SDL_SCANCODE_KP_6: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad6;
                    case SDL_SCANCODE_KP_7: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad7;
                    case SDL_SCANCODE_KP_8: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad8;
                    case SDL_SCANCODE_KP_9: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad9;
                    case SDL_SCANCODE_KP_PERIOD: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadDecimal;
                    case SDL_SCANCODE_KP_DIVIDE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadDivide;
                    case SDL_SCANCODE_KP_MULTIPLY: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadMultiply;
                    case SDL_SCANCODE_KP_MINUS: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadSubtract;
                    case SDL_SCANCODE_KP_PLUS: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadAdd;
                    case SDL_SCANCODE_KP_ENTER: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadEnter;
                    case SDL_SCANCODE_KP_EQUALS: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadEqual;
                    default: break;
                }
                switch (keycode)
                {
                    case SDLK_TAB: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Tab;
                    case SDLK_LEFT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow;
                    case SDLK_RIGHT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow;
                    case SDLK_UP: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow;
                    case SDLK_DOWN: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow;
                    case SDLK_PAGEUP: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PageUp;
                    case SDLK_PAGEDOWN: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PageDown;
                    case SDLK_HOME: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Home;
                    case SDLK_END: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_End;
                    case SDLK_INSERT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Insert;
                    case SDLK_DELETE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Delete;
                    case SDLK_BACKSPACE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backspace;
                    case SDLK_SPACE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Space;
                    case SDLK_RETURN: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Enter;
                    case SDLK_ESCAPE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Escape;
                    //case SDLK_APOSTROPHE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Apostrophe;
                    case SDLK_COMMA: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Comma;
                    //case SDLK_MINUS: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Minus;
                    case SDLK_PERIOD: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Period;
                    //case SDLK_SLASH: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Slash;
                    case SDLK_SEMICOLON: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Semicolon;
                    //case SDLK_EQUALS: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Equal;
                    //case SDLK_LEFTBRACKET: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftBracket;
                    //case SDLK_BACKSLASH: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backslash;
                    //case SDLK_RIGHTBRACKET: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightBracket;
                    //case SDLK_GRAVE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_GraveAccent;
                    case SDLK_CAPSLOCK: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_CapsLock;
                    case SDLK_SCROLLLOCK: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_ScrollLock;
                    case SDLK_NUMLOCKCLEAR: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NumLock;
                    case SDLK_PRINTSCREEN: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PrintScreen;
                    case SDLK_PAUSE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Pause;
                    case SDLK_LCTRL: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftCtrl;
                    case SDLK_LSHIFT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftShift;
                    case SDLK_LALT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftAlt;
                    case SDLK_LGUI: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftSuper;
                    case SDLK_RCTRL: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightCtrl;
                    case SDLK_RSHIFT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightShift;
                    case SDLK_RALT: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightAlt;
                    case SDLK_RGUI: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightSuper;
                    case SDLK_APPLICATION: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Menu;
                    case SDLK_0: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_0;
                    case SDLK_1: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_1;
                    case SDLK_2: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_2;
                    case SDLK_3: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_3;
                    case SDLK_4: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_4;
                    case SDLK_5: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_5;
                    case SDLK_6: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_6;
                    case SDLK_7: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_7;
                    case SDLK_8: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_8;
                    case SDLK_9: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_9;
                    case SDLK_A: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_A;
                    case SDLK_B: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_B;
                    case SDLK_C: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_C;
                    case SDLK_D: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_D;
                    case SDLK_E: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_E;
                    case SDLK_F: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F;
                    case SDLK_G: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_G;
                    case SDLK_H: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_H;
                    case SDLK_I: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_I;
                    case SDLK_J: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_J;
                    case SDLK_K: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_K;
                    case SDLK_L: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_L;
                    case SDLK_M: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_M;
                    case SDLK_N: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_N;
                    case SDLK_O: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_O;
                    case SDLK_P: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_P;
                    case SDLK_Q: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Q;
                    case SDLK_R: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_R;
                    case SDLK_S: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_S;
                    case SDLK_T: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_T;
                    case SDLK_U: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_U;
                    case SDLK_V: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_V;
                    case SDLK_W: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_W;
                    case SDLK_X: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_X;
                    case SDLK_Y: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Y;
                    case SDLK_Z: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Z;
                    case SDLK_F1: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F1;
                    case SDLK_F2: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F2;
                    case SDLK_F3: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F3;
                    case SDLK_F4: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F4;
                    case SDLK_F5: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F5;
                    case SDLK_F6: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F6;
                    case SDLK_F7: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F7;
                    case SDLK_F8: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F8;
                    case SDLK_F9: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F9;
                    case SDLK_F10: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F10;
                    case SDLK_F11: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F11;
                    case SDLK_F12: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F12;
                    case SDLK_F13: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F13;
                    case SDLK_F14: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F14;
                    case SDLK_F15: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F15;
                    case SDLK_F16: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F16;
                    case SDLK_F17: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F17;
                    case SDLK_F18: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F18;
                    case SDLK_F19: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F19;
                    case SDLK_F20: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F20;
                    case SDLK_F21: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F21;
                    case SDLK_F22: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F22;
                    case SDLK_F23: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F23;
                    case SDLK_F24: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F24;
                    case SDLK_AC_BACK: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_AppBack;
                    case SDLK_AC_FORWARD: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_AppForward;
                    default: break;
                }

                // Fallback to scancode
                switch (scancode)
                {
                case SDL_SCANCODE_GRAVE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_GraveAccent;
                case SDL_SCANCODE_MINUS: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Minus;
                case SDL_SCANCODE_EQUALS: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Equal;
                case SDL_SCANCODE_LEFTBRACKET: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftBracket;
                case SDL_SCANCODE_RIGHTBRACKET: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightBracket;
                case SDL_SCANCODE_NONUSBACKSLASH: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Oem102;
                case SDL_SCANCODE_BACKSLASH: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backslash;
                case SDL_SCANCODE_SEMICOLON: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Semicolon;
                case SDL_SCANCODE_APOSTROPHE: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Apostrophe;
                case SDL_SCANCODE_COMMA: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Comma;
                case SDL_SCANCODE_PERIOD: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Period;
                case SDL_SCANCODE_SLASH: return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Slash;
                default: break;
                }
                return ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_None;
            }
        };
    }
}

// Application
#include <FrenchieApplicationRenderingBackend.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

std::string ApplicationPlatformBackend::get_window_name()
{
    return std::string(SDL_GetWindowTitle(reinterpret_cast<SDL_Window*>(m_Api->Window)));
}

std::string ApplicationPlatformBackend::get_clipboard_text()
{
    return std::string(SDL_GetClipboardText());
}

bool ApplicationPlatformBackend::has_clipboard_text()
{
    return SDL_GetClipboardText() != nullptr;
}

void ApplicationPlatformBackend::set_window_name(const std::string& _Value)
{
    SDL_SetWindowTitle(reinterpret_cast<SDL_Window*>(m_Api->Window), _Value.c_str());
}

void ApplicationPlatformBackend::set_clipboard_text(const std::string& _Value)
{
    SDL_SetClipboardText(_Value.c_str());
}

bool ApplicationPlatformBackend::awake()
{
    if(m_Api != nullptr && m_Api->Window != nullptr)
        return true;

    // initialization
    if(!SDL_Init(SDL_INIT_VIDEO))
        return false;

    // create platform API
    m_Api = std::make_shared<FrenchieApplicationPlatformSDL3>();

    auto SDL3 = platform_api<FrenchieApplicationPlatformSDL3>();

#ifdef FRENCHIE_APPLICATION_PLATFORM_IS_MACOS
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    // create context
    SDL3->Window =
        SDL_CreateWindow("Application", 512, 256, SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

    if(m_Api->Window == nullptr)
    {
        SDL_Quit();
        return false;
    }

    SDL3->Context = SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(SDL3->Window));

    if(SDL3->Context == nullptr)
    {
        SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(SDL3->Window));
        SDL_Quit();
        return false;
    }

    // configure context
    if(!SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(SDL3->Window), SDL3->Context))
    {
        SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(SDL3->Window));
        SDL_GL_DestroyContext(SDL3->Context);
        SDL_Quit();
        return false;
    }

    // load rendering backend
    if(!ApplicationRenderingBackend::awake((ApplicationRenderingBackend::Loader)SDL_GL_GetProcAddress))
    {
        SDL_Quit();
        return false;
    }

    return true;
}

void ApplicationPlatformBackend::frame_start()
{
    auto SDL3 = platform_api<FrenchieApplicationPlatformSDL3>();

    if(SDL3 == nullptr)
        return;

    // retrieve frame buffer size
    int x = 0;
    int y = 0;

    SDL_GetWindowSizeInPixels(reinterpret_cast<SDL_Window*>(SDL3->Window), &x, &y);
    SDL3->Input.FrameBufferSize = gs_vec2f(x, y);

    // retrieve window size
    SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(SDL3->Window), &x, &y);
    SDL3->Input.WindowSize = gs_vec2f(x, y);

    // retrieve window position
    SDL_GetWindowPosition(reinterpret_cast<SDL_Window*>(SDL3->Window), &x, &y);
    SDL3->Input.WindowPosition = gs_vec2f(x, y);

    // poll events
    SDL_StartTextInput(reinterpret_cast<SDL_Window*>(SDL3->Window));
    SDL_PollEvent(&SDL3->Event);
    SDL_GL_SetSwapInterval(1);

    // handle events
    switch (SDL3->Event.type)
    {
        case SDL_EVENT_MOUSE_MOTION:
        {
            SDL3->Input.MouseCursor.Position = gs_vec2f((float)SDL3->Event.motion.x, (float)SDL3->Event.motion.y);
            return;
        }
        case SDL_EVENT_MOUSE_WHEEL:
        {
            SDL3->Input.MouseScrollOffset = gs_vec2f(SDL3->Event.wheel.x, SDL3->Event.wheel.y);
            return;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            SDL3->Input.MouseButtons[SDLApplicationInputHandler::glfw_mouse_button_to_application_mouse_button(SDL3->Event.button.button)].Pressed =
                SDL3->Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
            
            SDL3->Input.MouseButtons[SDLApplicationInputHandler::glfw_mouse_button_to_application_mouse_button(SDL3->Event.button.button)].Released =
                SDL3->Event.type == SDL_EVENT_MOUSE_BUTTON_UP;

            return;
        }
        case SDL_EVENT_TEXT_INPUT:
        {
            std::u32string utf32 = Frenchie::Core::String::convert_utf8_to_utf32(SDL3->Event.text.text);

            if(!utf32.empty())
            {
                SDL3->Input.Character = utf32[0];
                return;
            }
        }
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        {
            SDL3->Input.Keys[SDLApplicationInputHandler::glfw_key_to_application_key(SDL3->Event.key.key, SDL3->Event.key.scancode)].Pressed =
                SDL3->Event.type == SDL_EVENT_KEY_DOWN;

            SDL3->Input.Keys[SDLApplicationInputHandler::glfw_key_to_application_key(SDL3->Event.key.key, SDL3->Event.key.scancode)].Released =
                SDL3->Event.type == SDL_EVENT_KEY_UP;

            return;
        }
        case SDL_EVENT_DISPLAY_ORIENTATION:
        case SDL_EVENT_DISPLAY_ADDED:
        case SDL_EVENT_DISPLAY_REMOVED:
        case SDL_EVENT_DISPLAY_MOVED:
        case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
        {
            // TODO: add monitors update logic here
            return;
        }
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
        {
            SDL3->Input.MouseCursor.Entered = true;
            return;
        }
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        {
            SDL3->Input.MouseCursor.Entered = false;
            return;
        }
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_FOCUS_LOST:
        {
            SDL3->Input.Window.Focused = SDL3->Event.type == SDL_EVENT_WINDOW_FOCUS_GAINED;
            return;
        }
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        case SDL_EVENT_WINDOW_MOVED:
        case SDL_EVENT_WINDOW_RESIZED:
        {
            if(SDL3->Event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
            {
                // TODO: add logic here
            }
            else if(SDL3->Event.type == SDL_EVENT_WINDOW_MOVED)
            {
                // TODO: add logic here
            }
            else if(SDL3->Event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                // TODO: add logic here
            }

            return;
        }
        case SDL_EVENT_GAMEPAD_ADDED:
        case SDL_EVENT_GAMEPAD_REMOVED:
        {
            return;
        }
        default:
            break;
    }


}

void ApplicationPlatformBackend::frame_update()
{
    auto SDL3 = platform_api<FrenchieApplicationPlatformSDL3>();

    if(SDL3 == nullptr)
        return;

    if(SDL_TextInputActive(reinterpret_cast<SDL_Window*>(SDL3->Window)))
        SDL_StopTextInput(reinterpret_cast<SDL_Window*>(SDL3->Window));

    // adjust viewport
    ApplicationRenderingBackend::set_viewport(gs_vec2f(0, 0), SDL3->Input.FrameBufferSize);
}

void ApplicationPlatformBackend::frame_finish()
{
    auto SDL3 = platform_api<FrenchieApplicationPlatformSDL3>();

    if(SDL3 == nullptr)
        return;

    SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(SDL3->Window));
}

void ApplicationPlatformBackend::quit()
{
    // terminate self
    auto SDL3 = platform_api<FrenchieApplicationPlatformSDL3>();

    if(SDL3 != nullptr)
    {
        SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(SDL3->Window));
        SDL_GL_DestroyContext(SDL3->Context);
    }

    SDL_Quit();

    // terminate rendering API
    ApplicationRenderingBackend::quit();
}

bool ApplicationPlatformBackend::is_closed()
{
    auto SDL3 = platform_api<FrenchieApplicationPlatformSDL3>();

    return SDL3 == nullptr || SDL3->Event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED;
}

void ApplicationPlatformBackend::close()
{
    ApplicationPlatformBackend::quit();
}