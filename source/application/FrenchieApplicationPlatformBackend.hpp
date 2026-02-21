#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreClock.hpp>

// STL
#include <string>

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationPlatformBackendMouseButton
        {
            enum Button : int
            {
                ApplicationPlatformBackendMouseButtonBegin,
                ApplicationPlatformBackendMouseButtonLeft = ApplicationPlatformBackendMouseButtonBegin,
                ApplicationPlatformBackendMouseButtonRight,
                ApplicationPlatformBackendMouseButtonMiddle,
                ApplicationPlatformBackendMouseButtonEnd
            };

            int                                            Clicks       {0    };
            bool                                           Down         {false};
            bool                                           Hold         {false};
            bool                                           Pressed      {false};
            bool                                           Released     {false};
            bool                                           Clicked      {false};
            bool                                           DoubleClicked{false};
            std::chrono::high_resolution_clock::time_point PressTime    {std::chrono::high_resolution_clock::time_point()};
            std::chrono::high_resolution_clock::time_point ReleaseTime  {std::chrono::high_resolution_clock::time_point()};

            static std::string to_string(const ApplicationPlatformBackendMouseButton::Button& _Button)
            {
                switch (_Button)
                {
                case ApplicationPlatformBackendMouseButtonLeft: return GS_STRINGIFY(ApplicationPlatformBackendMouseButtonLeft);
                case ApplicationPlatformBackendMouseButtonRight: return GS_STRINGIFY(ApplicationPlatformBackendMouseButtonRight);
                case ApplicationPlatformBackendMouseButtonMiddle: return GS_STRINGIFY(ApplicationPlatformBackendMouseButtonMiddle);
                }
                return GS_STRINGIFY(ApplicationPlatformBackendMouseButtonEnd);
            }
        };

        struct ApplicationPlatformBackendMouseCursor
        {
            bool      Entered           {false};
            gs_vec2f  Position          {gs_vec2f(0.f)};
            gs_vec2f  MousePressPosition{gs_vec2f(0.f)};
            gs_vec2f  DragDelta         {gs_vec2f(0.f)};
        };

        struct ApplicationPlatformBackendWindow
        {
            bool Focused {false};
        };

        struct ApplicationPlatformBackendKey
        {
            enum Key : int
            {
                // Keyboard
                ApplicationPlatformBackendKey_None = 0,
                ApplicationPlatformBackendKey_NamedKey_BEGIN = 0,  // First valid key value (other than 0)

                ApplicationPlatformBackendKey_Tab = ApplicationPlatformBackendKey_NamedKey_BEGIN,             // == ApplicationPlatformBackendKey_NamedKey_BEGIN
                ApplicationPlatformBackendKey_LeftArrow,
                ApplicationPlatformBackendKey_RightArrow,
                ApplicationPlatformBackendKey_UpArrow,
                ApplicationPlatformBackendKey_DownArrow,
                ApplicationPlatformBackendKey_PageUp,
                ApplicationPlatformBackendKey_PageDown,
                ApplicationPlatformBackendKey_Home,
                ApplicationPlatformBackendKey_End,
                ApplicationPlatformBackendKey_Insert,
                ApplicationPlatformBackendKey_Delete,
                ApplicationPlatformBackendKey_Backspace,
                ApplicationPlatformBackendKey_Space,
                ApplicationPlatformBackendKey_Enter,
                ApplicationPlatformBackendKey_Escape,
                ApplicationPlatformBackendKey_LeftCtrl, ApplicationPlatformBackendKey_LeftShift, ApplicationPlatformBackendKey_LeftAlt, ApplicationPlatformBackendKey_LeftSuper,     // Also see ImGuiMod_Ctrl, ImGuiMod_Shift, ImGuiMod_Alt, ImGuiMod_Super below!
                ApplicationPlatformBackendKey_RightCtrl, ApplicationPlatformBackendKey_RightShift, ApplicationPlatformBackendKey_RightAlt, ApplicationPlatformBackendKey_RightSuper,
                ApplicationPlatformBackendKey_Menu,
                ApplicationPlatformBackendKey_0, ApplicationPlatformBackendKey_1, ApplicationPlatformBackendKey_2, ApplicationPlatformBackendKey_3, ApplicationPlatformBackendKey_4, ApplicationPlatformBackendKey_5, ApplicationPlatformBackendKey_6, ApplicationPlatformBackendKey_7, ApplicationPlatformBackendKey_8, ApplicationPlatformBackendKey_9,
                ApplicationPlatformBackendKey_A, ApplicationPlatformBackendKey_B, ApplicationPlatformBackendKey_C, ApplicationPlatformBackendKey_D, ApplicationPlatformBackendKey_E, ApplicationPlatformBackendKey_F, ApplicationPlatformBackendKey_G, ApplicationPlatformBackendKey_H, ApplicationPlatformBackendKey_I, ApplicationPlatformBackendKey_J,
                ApplicationPlatformBackendKey_K, ApplicationPlatformBackendKey_L, ApplicationPlatformBackendKey_M, ApplicationPlatformBackendKey_N, ApplicationPlatformBackendKey_O, ApplicationPlatformBackendKey_P, ApplicationPlatformBackendKey_Q, ApplicationPlatformBackendKey_R, ApplicationPlatformBackendKey_S, ApplicationPlatformBackendKey_T,
                ApplicationPlatformBackendKey_U, ApplicationPlatformBackendKey_V, ApplicationPlatformBackendKey_W, ApplicationPlatformBackendKey_X, ApplicationPlatformBackendKey_Y, ApplicationPlatformBackendKey_Z,
                ApplicationPlatformBackendKey_F1, ApplicationPlatformBackendKey_F2, ApplicationPlatformBackendKey_F3, ApplicationPlatformBackendKey_F4, ApplicationPlatformBackendKey_F5, ApplicationPlatformBackendKey_F6,
                ApplicationPlatformBackendKey_F7, ApplicationPlatformBackendKey_F8, ApplicationPlatformBackendKey_F9, ApplicationPlatformBackendKey_F10, ApplicationPlatformBackendKey_F11, ApplicationPlatformBackendKey_F12,
                ApplicationPlatformBackendKey_F13, ApplicationPlatformBackendKey_F14, ApplicationPlatformBackendKey_F15, ApplicationPlatformBackendKey_F16, ApplicationPlatformBackendKey_F17, ApplicationPlatformBackendKey_F18,
                ApplicationPlatformBackendKey_F19, ApplicationPlatformBackendKey_F20, ApplicationPlatformBackendKey_F21, ApplicationPlatformBackendKey_F22, ApplicationPlatformBackendKey_F23, ApplicationPlatformBackendKey_F24,
                ApplicationPlatformBackendKey_Apostrophe,        // '
                ApplicationPlatformBackendKey_Comma,             // ,
                ApplicationPlatformBackendKey_Minus,             // -
                ApplicationPlatformBackendKey_Period,            // .
                ApplicationPlatformBackendKey_Slash,             // /
                ApplicationPlatformBackendKey_Semicolon,         // ;
                ApplicationPlatformBackendKey_Equal,             // =
                ApplicationPlatformBackendKey_LeftBracket,       // [
                ApplicationPlatformBackendKey_Backslash,         // \ (this text inhibit multiline comment caused by backslash)
                ApplicationPlatformBackendKey_RightBracket,      // ]
                ApplicationPlatformBackendKey_GraveAccent,       // `
                ApplicationPlatformBackendKey_CapsLock,
                ApplicationPlatformBackendKey_ScrollLock,
                ApplicationPlatformBackendKey_NumLock,
                ApplicationPlatformBackendKey_PrintScreen,
                ApplicationPlatformBackendKey_Pause,
                ApplicationPlatformBackendKey_Keypad0, ApplicationPlatformBackendKey_Keypad1, ApplicationPlatformBackendKey_Keypad2, ApplicationPlatformBackendKey_Keypad3, ApplicationPlatformBackendKey_Keypad4,
                ApplicationPlatformBackendKey_Keypad5, ApplicationPlatformBackendKey_Keypad6, ApplicationPlatformBackendKey_Keypad7, ApplicationPlatformBackendKey_Keypad8, ApplicationPlatformBackendKey_Keypad9,
                ApplicationPlatformBackendKey_KeypadDecimal,
                ApplicationPlatformBackendKey_KeypadDivide,
                ApplicationPlatformBackendKey_KeypadMultiply,
                ApplicationPlatformBackendKey_KeypadSubtract,
                ApplicationPlatformBackendKey_KeypadAdd,
                ApplicationPlatformBackendKey_KeypadEnter,
                ApplicationPlatformBackendKey_KeypadEqual,
                ApplicationPlatformBackendKey_AppBack,               // Available on some keyboard/mouses. Often referred as "Browser Back"
                ApplicationPlatformBackendKey_AppForward,
                ApplicationPlatformBackendKey_Oem102,                // Non-US backslash.

                // Gamepad
                // (analog values are 0.0f to 1.0f)
                // (download controller mapping PNG/PSD at http://dearimgui.com/controls_sheets)
                //                              // XBOX        | SWITCH  | PLAYSTA. | -> ACTION
                ApplicationPlatformBackendKey_GamepadStart,          // Menu        | +       | Options  |
                ApplicationPlatformBackendKey_GamepadBack,           // View        | -       | Share    |
                ApplicationPlatformBackendKey_GamepadFaceLeft,       // X           | Y       | Square   | Tap: Toggle Menu. Hold: Windowing mode (Focus/Move/Resize windows)
                ApplicationPlatformBackendKey_GamepadFaceRight,      // B           | A       | Circle   | Cancel / Close / Exit
                ApplicationPlatformBackendKey_GamepadFaceUp,         // Y           | X       | Triangle | Text Input / On-screen Keyboard
                ApplicationPlatformBackendKey_GamepadFaceDown,       // A           | B       | Cross    | Activate / Open / Toggle / Tweak
                ApplicationPlatformBackendKey_GamepadDpadLeft,       // D-pad Left  | "       | "        | Move / Tweak / Resize Window (in Windowing mode)
                ApplicationPlatformBackendKey_GamepadDpadRight,      // D-pad Right | "       | "        | Move / Tweak / Resize Window (in Windowing mode)
                ApplicationPlatformBackendKey_GamepadDpadUp,         // D-pad Up    | "       | "        | Move / Tweak / Resize Window (in Windowing mode)
                ApplicationPlatformBackendKey_GamepadDpadDown,       // D-pad Down  | "       | "        | Move / Tweak / Resize Window (in Windowing mode)
                ApplicationPlatformBackendKey_GamepadL1,             // L Bumper    | L       | L1       | Tweak Slower / Focus Previous (in Windowing mode)
                ApplicationPlatformBackendKey_GamepadR1,             // R Bumper    | R       | R1       | Tweak Faster / Focus Next (in Windowing mode)
                ApplicationPlatformBackendKey_GamepadL2,             // L Trigger   | ZL      | L2       | [Analog]
                ApplicationPlatformBackendKey_GamepadR2,             // R Trigger   | ZR      | R2       | [Analog]
                ApplicationPlatformBackendKey_GamepadL3,             // L Stick     | L3      | L3       |
                ApplicationPlatformBackendKey_GamepadR3,             // R Stick     | R3      | R3       |
                ApplicationPlatformBackendKey_GamepadLStickLeft,     //             |         |          | [Analog] Move Window (in Windowing mode)
                ApplicationPlatformBackendKey_GamepadLStickRight,    //             |         |          | [Analog] Move Window (in Windowing mode)
                ApplicationPlatformBackendKey_GamepadLStickUp,       //             |         |          | [Analog] Move Window (in Windowing mode)
                ApplicationPlatformBackendKey_GamepadLStickDown,     //             |         |          | [Analog] Move Window (in Windowing mode)
                ApplicationPlatformBackendKey_GamepadRStickLeft,     //             |         |          | [Analog]
                ApplicationPlatformBackendKey_GamepadRStickRight,    //             |         |          | [Analog]
                ApplicationPlatformBackendKey_GamepadRStickUp,       //             |         |          | [Analog]
                ApplicationPlatformBackendKey_GamepadRStickDown,     //             |         |          | [Analog]

                // Aliases: Mouse Buttons (auto-submitted from AddMouseButtonEvent() calls)
                // - This is mirroring the data also written to io.MouseDown[], io.MouseWheel, in a format allowing them to be accessed via standard key API.
                ApplicationPlatformBackendKey_MouseLeft, ApplicationPlatformBackendKey_MouseRight, ApplicationPlatformBackendKey_MouseMiddle, ApplicationPlatformBackendKey_MouseX1, ApplicationPlatformBackendKey_MouseX2, ApplicationPlatformBackendKey_MouseWheelX, ApplicationPlatformBackendKey_MouseWheelY,

                // [Internal] Reserved for mod storage
                ApplicationPlatformBackendKey_ReservedForModCtrl, ApplicationPlatformBackendKey_ReservedForModShift, ApplicationPlatformBackendKey_ReservedForModAlt, ApplicationPlatformBackendKey_ReservedForModSuper,

                // [Internal] If you need to iterate all keys (for e.g. an input mapper) you may use ApplicationPlatformBackendKey_NamedKey_BEGIN..ApplicationPlatformBackendKey_NamedKey_END.
                ApplicationPlatformBackendKey_NamedKey_END,
                ApplicationPlatformBackendKey_NamedKey_COUNT = ApplicationPlatformBackendKey_NamedKey_END - ApplicationPlatformBackendKey_NamedKey_BEGIN,

                // Keyboard Modifiers (explicitly submitted by backend via AddKeyEvent() calls)
                // - Any functions taking a ApplicationPlatformBackendKeyChord parameter can binary-or those with regular keys, e.g. Shortcut(ImGuiMod_Ctrl | ApplicationPlatformBackendKey_S).
                // - Those are written back into io.KeyCtrl, io.KeyShift, io.KeyAlt, io.KeySuper for convenience,
                //   but may be accessed via standard key API such as IsKeyPressed(), IsKeyReleased(), querying duration etc.
                // - Code polling every key (e.g. an interface to detect a key press for input mapping) might want to ignore those
                //   and prefer using the real keys (e.g. ApplicationPlatformBackendKey_LeftCtrl, ApplicationPlatformBackendKey_RightCtrl instead of ImGuiMod_Ctrl).
                // - In theory the value of keyboard modifiers should be roughly equivalent to a logical or of the equivalent left/right keys.
                //   In practice: it's complicated; mods are often provided from different sources. Keyboard layout, IME, sticky keys and
                //   backends tend to interfere and break that equivalence. The safer decision is to relay that ambiguity down to the end-user...
                // - On macOS, we swap Cmd(Super) and Ctrl keys at the time of the io.AddKeyEvent() call.
                ImGuiMod_None                   = 0,
                ImGuiMod_Ctrl                   = 1 << 12, // Ctrl (non-macOS), Cmd (macOS)
                ImGuiMod_Shift                  = 1 << 13, // Shift
                ImGuiMod_Alt                    = 1 << 14, // Option/Menu
                ImGuiMod_Super                  = 1 << 15, // Windows/Super (non-macOS), Ctrl (macOS)
                ImGuiMod_Mask_                  = 0xF000,  // 4-bits
            };

            int                                            Clicks       {0    };
            bool                                           Down         {false};
            bool                                           Hold         {false};
            bool                                           Pressed      {false};
            bool                                           Released     {false};
            bool                                           Clicked      {false};
            std::chrono::high_resolution_clock::time_point PressTime    {std::chrono::high_resolution_clock::time_point()};
            std::chrono::high_resolution_clock::time_point ReleaseTime  {std::chrono::high_resolution_clock::time_point()};

            static std::string to_string(const ApplicationPlatformBackendKey::Key& _Key)
            {
                switch (_Key)
                {
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Tab: return GS_STRINGIFY(ApplicationPlatformBackendKey_Tab);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow: return GS_STRINGIFY(ApplicationPlatformBackendKey_LeftArrow);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow: return GS_STRINGIFY(ApplicationPlatformBackendKey_RightArrow);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow: return GS_STRINGIFY(ApplicationPlatformBackendKey_UpArrow);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow: return GS_STRINGIFY(ApplicationPlatformBackendKey_DownArrow);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PageUp: return GS_STRINGIFY(ApplicationPlatformBackendKey_PageUp);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PageDown: return GS_STRINGIFY(ApplicationPlatformBackendKey_PageDown);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Home: return GS_STRINGIFY(ApplicationPlatformBackendKey_Home);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_End: return GS_STRINGIFY(ApplicationPlatformBackendKey_End);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Insert: return GS_STRINGIFY(ApplicationPlatformBackendKey_Insert);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Delete: return GS_STRINGIFY(ApplicationPlatformBackendKey_Delete);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backspace: return GS_STRINGIFY(ApplicationPlatformBackendKey_Backspace);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Space: return GS_STRINGIFY(ApplicationPlatformBackendKey_Space);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Enter: return GS_STRINGIFY(ApplicationPlatformBackendKey_Enter);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Escape: return GS_STRINGIFY(ApplicationPlatformBackendKey_Escape);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Apostrophe: return GS_STRINGIFY(ApplicationPlatformBackendKey_Apostrophe);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Comma: return GS_STRINGIFY(ApplicationPlatformBackendKey_Comma);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Minus: return GS_STRINGIFY(ApplicationPlatformBackendKey_Minus);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Period: return GS_STRINGIFY(ApplicationPlatformBackendKey_Period);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Slash: return GS_STRINGIFY(ApplicationPlatformBackendKey_Slash);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Semicolon: return GS_STRINGIFY(ApplicationPlatformBackendKey_Semicolon);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Equal: return GS_STRINGIFY(ApplicationPlatformBackendKey_Equal);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftBracket: return GS_STRINGIFY(ApplicationPlatformBackendKey_LeftBracket);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backslash: return GS_STRINGIFY(ApplicationPlatformBackendKey_Backslash);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Oem102: return GS_STRINGIFY(ApplicationPlatformBackendKey_Oem102);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightBracket: return GS_STRINGIFY(ApplicationPlatformBackendKey_RightBracket);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_GraveAccent: return GS_STRINGIFY(ApplicationPlatformBackendKey_GraveAccent);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_CapsLock: return GS_STRINGIFY(ApplicationPlatformBackendKey_CapsLock);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_ScrollLock: return GS_STRINGIFY(ApplicationPlatformBackendKey_ScrollLock);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NumLock: return GS_STRINGIFY(ApplicationPlatformBackendKey_NumLock);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PrintScreen: return GS_STRINGIFY(ApplicationPlatformBackendKey_PrintScreen);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Pause: return GS_STRINGIFY(ApplicationPlatformBackendKey_Pause);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad0: return GS_STRINGIFY(ApplicationPlatformBackendKey_Keypad0);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad1: return GS_STRINGIFY(ApplicationPlatformBackendKey_Keypad1);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad2: return GS_STRINGIFY(ApplicationPlatformBackendKey_Keypad2);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad3: return GS_STRINGIFY(ApplicationPlatformBackendKey_Keypad3);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad4: return GS_STRINGIFY(ApplicationPlatformBackendKey_Keypad4);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad5: return GS_STRINGIFY(ApplicationPlatformBackendKey_Keypad5);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad6: return GS_STRINGIFY(ApplicationPlatformBackendKey_Keypad6);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad7: return GS_STRINGIFY(ApplicationPlatformBackendKey_Keypad7);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad8: return GS_STRINGIFY(ApplicationPlatformBackendKey_Keypad8);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad9: return GS_STRINGIFY(ApplicationPlatformBackendKey_Keypad9);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadDecimal: return GS_STRINGIFY(ApplicationPlatformBackendKey_KeypadDecimal);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadDivide: return GS_STRINGIFY(ApplicationPlatformBackendKey_KeypadDivide);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadMultiply: return GS_STRINGIFY(ApplicationPlatformBackendKey_KeypadMultiply);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadSubtract: return GS_STRINGIFY(ApplicationPlatformBackendKey_KeypadSubtract);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadAdd: return GS_STRINGIFY(ApplicationPlatformBackendKey_KeypadAdd);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadEnter: return GS_STRINGIFY(ApplicationPlatformBackendKey_KeypadEnter);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadEqual: return GS_STRINGIFY(ApplicationPlatformBackendKey_KeypadEqual);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftShift: return GS_STRINGIFY(ApplicationPlatformBackendKey_LeftShift);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftCtrl: return GS_STRINGIFY(ApplicationPlatformBackendKey_LeftCtrl);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftAlt: return GS_STRINGIFY(ApplicationPlatformBackendKey_LeftAlt);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftSuper: return GS_STRINGIFY(ApplicationPlatformBackendKey_LeftSuper);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightShift: return GS_STRINGIFY(ApplicationPlatformBackendKey_RightShift);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightCtrl: return GS_STRINGIFY(ApplicationPlatformBackendKey_RightCtrl);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightAlt: return GS_STRINGIFY(ApplicationPlatformBackendKey_RightAlt);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightSuper: return GS_STRINGIFY(ApplicationPlatformBackendKey_RightSuper);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Menu: return GS_STRINGIFY(ApplicationPlatformBackendKey_Menu);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_0: return GS_STRINGIFY(ApplicationPlatformBackendKey_0);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_1: return GS_STRINGIFY(ApplicationPlatformBackendKey_1);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_2: return GS_STRINGIFY(ApplicationPlatformBackendKey_2);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_3: return GS_STRINGIFY(ApplicationPlatformBackendKey_3);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_4: return GS_STRINGIFY(ApplicationPlatformBackendKey_4);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_5: return GS_STRINGIFY(ApplicationPlatformBackendKey_5);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_6: return GS_STRINGIFY(ApplicationPlatformBackendKey_6);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_7: return GS_STRINGIFY(ApplicationPlatformBackendKey_7);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_8: return GS_STRINGIFY(ApplicationPlatformBackendKey_8);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_9: return GS_STRINGIFY(ApplicationPlatformBackendKey_9);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_A: return GS_STRINGIFY(ApplicationPlatformBackendKey_A);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_B: return GS_STRINGIFY(ApplicationPlatformBackendKey_B);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_C: return GS_STRINGIFY(ApplicationPlatformBackendKey_C);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_D: return GS_STRINGIFY(ApplicationPlatformBackendKey_D);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_E: return GS_STRINGIFY(ApplicationPlatformBackendKey_E);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F: return GS_STRINGIFY(ApplicationPlatformBackendKey_F);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_G: return GS_STRINGIFY(ApplicationPlatformBackendKey_G);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_H: return GS_STRINGIFY(ApplicationPlatformBackendKey_H);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_I: return GS_STRINGIFY(ApplicationPlatformBackendKey_I);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_J: return GS_STRINGIFY(ApplicationPlatformBackendKey_J);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_K: return GS_STRINGIFY(ApplicationPlatformBackendKey_K);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_L: return GS_STRINGIFY(ApplicationPlatformBackendKey_L);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_M: return GS_STRINGIFY(ApplicationPlatformBackendKey_M);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_N: return GS_STRINGIFY(ApplicationPlatformBackendKey_N);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_O: return GS_STRINGIFY(ApplicationPlatformBackendKey_O);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_P: return GS_STRINGIFY(ApplicationPlatformBackendKey_P);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Q: return GS_STRINGIFY(ApplicationPlatformBackendKey_Q);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_R: return GS_STRINGIFY(ApplicationPlatformBackendKey_R);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_S: return GS_STRINGIFY(ApplicationPlatformBackendKey_S);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_T: return GS_STRINGIFY(ApplicationPlatformBackendKey_T);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_U: return GS_STRINGIFY(ApplicationPlatformBackendKey_U);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_V: return GS_STRINGIFY(ApplicationPlatformBackendKey_V);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_W: return GS_STRINGIFY(ApplicationPlatformBackendKey_W);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_X: return GS_STRINGIFY(ApplicationPlatformBackendKey_X);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Y: return GS_STRINGIFY(ApplicationPlatformBackendKey_Y);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Z: return GS_STRINGIFY(ApplicationPlatformBackendKey_Z);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F1: return GS_STRINGIFY(ApplicationPlatformBackendKey_F1);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F2: return GS_STRINGIFY(ApplicationPlatformBackendKey_F2);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F3: return GS_STRINGIFY(ApplicationPlatformBackendKey_F3);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F4: return GS_STRINGIFY(ApplicationPlatformBackendKey_F4);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F5: return GS_STRINGIFY(ApplicationPlatformBackendKey_F5);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F6: return GS_STRINGIFY(ApplicationPlatformBackendKey_F6);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F7: return GS_STRINGIFY(ApplicationPlatformBackendKey_F7);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F8: return GS_STRINGIFY(ApplicationPlatformBackendKey_F8);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F9: return GS_STRINGIFY(ApplicationPlatformBackendKey_F9);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F10: return GS_STRINGIFY(ApplicationPlatformBackendKey_F10);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F11: return GS_STRINGIFY(ApplicationPlatformBackendKey_F11);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F12: return GS_STRINGIFY(ApplicationPlatformBackendKey_F12);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F13: return GS_STRINGIFY(ApplicationPlatformBackendKey_F13);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F14: return GS_STRINGIFY(ApplicationPlatformBackendKey_F14);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F15: return GS_STRINGIFY(ApplicationPlatformBackendKey_F15);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F16: return GS_STRINGIFY(ApplicationPlatformBackendKey_F16);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F17: return GS_STRINGIFY(ApplicationPlatformBackendKey_F17);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F18: return GS_STRINGIFY(ApplicationPlatformBackendKey_F18);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F19: return GS_STRINGIFY(ApplicationPlatformBackendKey_F19);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F20: return GS_STRINGIFY(ApplicationPlatformBackendKey_F20);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F21: return GS_STRINGIFY(ApplicationPlatformBackendKey_F21);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F22: return GS_STRINGIFY(ApplicationPlatformBackendKey_F22);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F23: return GS_STRINGIFY(ApplicationPlatformBackendKey_F23);
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F24: return GS_STRINGIFY(ApplicationPlatformBackendKey_F24);
                    default: return GS_STRINGIFY(ApplicationPlatformBackendKey_None);
                }
            }
        };

        struct ApplicationPlatformBackendInput
        {            
            ApplicationPlatformBackendMouseButton MouseButtons[ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd]{};
            ApplicationPlatformBackendMouseCursor MouseCursor {ApplicationPlatformBackendMouseCursor()};
            ApplicationPlatformBackendWindow      Window      {ApplicationPlatformBackendWindow()};
            ApplicationPlatformBackendKey         Keys        [ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_COUNT]{};
        };

        class ApplicationPlatformBackend final // TODO: may be implement strategy pattern here ????
        {
        public:
            // API
            static bool awake();
            static void frame_start();
            static void frame_update();
            static void frame_finish();

            static void quit();
            static bool is_closed();
            static void close();

            // getters
            static std::string get_window_name();
            static gs_vec2f    get_window_size();
            static gs_vec2f    get_window_position();
            static gs_vec2f    get_window_cursor_position();
            static gs_vec2f    get_window_cursor_dragdelta();
            static gs_vec2f    get_window_framebuffer_size();

            static bool is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button&);
            static bool is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button&);
            static bool is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button&);
            static bool is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button&);
            static bool is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button&);
            static bool is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button&);

            static bool is_key_down(const ApplicationPlatformBackendKey::Key&);
            static bool is_key_hold(const ApplicationPlatformBackendKey::Key&);
            static bool is_key_pressed(const ApplicationPlatformBackendKey::Key&);
            static bool is_key_released(const ApplicationPlatformBackendKey::Key&);
            static bool is_key_clicked(const ApplicationPlatformBackendKey::Key&);

            // setters
            static void set_window_name(const std::string& _Name);

            // info
            static ApplicationPlatformBackendInput m_Input;

        private:

            // context
            static void* m_Context;
        };
    }
}