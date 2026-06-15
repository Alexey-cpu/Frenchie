#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreClock.hpp>
#include <FrenchieCoreOptional.hpp>

// STL
#include <string>
#include <vector>
#include <memory>

#if defined(unix) || defined(__unix) || defined(__unix__)
#define FRENCHIE_APPLICATION_PLATFORM_IS_LINUX
#endif

#if defined(__APPLE__) || defined(__MACH__)
#define FRENCHIE_APPLICATION_PLATFORM_IS_MACOS
#endif

#if defined(_WIN32) || defined(_WIN64)
#define FRENCHIE_APPLICATION_PLATFORM_IS_WINDOWS
#endif

#if defined(FRENCHIE_APPLICATION_PLATFORM_IS_LINUX) || defined(FRENCHIE_APPLICATION_PLATFORM_IS_MACOS)
#define FRENCHIE_APPLICATION_PLATFORM_IS_UNIX
#endif

/*! \defgroup <Application> (Application)
*  @brief The module contains application launching instance.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Application
    {
        /*! \defgroup <ApplicationPlatformBackend> (Application platform backend)
        *  @ingroup Application
        *  @brief The module contains utility classes that wrap application platform backend state.
        *  @{
        */

        /**
         * @brief This struct encapulates platform backend keyboard key state.
         * @struct ApplicationPlatformBackendKey
         */
        struct ApplicationPlatformBackendKey
        {
            /**
             * @brief This enum encodes platform backend keyboard key code.
             * @enum Key
             */
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

                // [Internal] If you need to iterate all keys (for e.g. an input mapper) you may use ApplicationPlatformBackendKey_NamedKey_BEGIN..ApplicationPlatformBackendKey_NamedKey_END.
                ApplicationPlatformBackendKey_NamedKey_END
            };

            int                              Clicks       {0    };                                                 ///< clicks count
            bool                             Down         {false};                                                 ///< is true when key is down
            bool                             Hold         {false};                                                 ///< is true when key is hold
            bool                             Pressed      {false};                                                 ///< is true when key is pressed
            bool                             Released     {false};                                                 ///< is true when key is released
            bool                             Clicked      {false};                                                 ///< is true when key is clicked
            Frenchie::Core::Clock::TimePoint PressTime    {Frenchie::Core::Clock::TimePoint()}; ///< time point when key is pressed
            Frenchie::Core::Clock::TimePoint ReleaseTime  {Frenchie::Core::Clock::TimePoint()}; ///< time point when key is released

            /**
             * @brief Converts key code to a string.
             * @param _Key keyboard key code.
             * @return returns keyboard key name.
             */
            static std::string to_string(const ApplicationPlatformBackendKey::Key& _Key)
            {
                switch (_Key)
                {
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Tab:            return "Key_Tab";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftArrow:      return "Key_LeftArrow";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightArrow:     return "Key_RightArrow";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_UpArrow:        return "Key_UpArrow";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_DownArrow:      return "Key_DownArrow";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PageUp:         return "Key_PageUp";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PageDown:       return "Key_PageDown";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Home:           return "Key_Home";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_End:            return "Key_End";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Insert:         return "Key_Insert";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Delete:         return "Key_Delete";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backspace:      return "Key_Backspace";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Space:          return "Key_Space";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Enter:          return "Key_Enter";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Escape:         return "Key_Escape";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Apostrophe:     return "Key_Apostrophe";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Comma:          return "Key_Comma";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Minus:          return "Key_Minus";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Period:         return "Key_Period";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Slash:          return "Key_Slash";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Semicolon:      return "Key_Semicolon";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Equal:          return "Key_Equal";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftBracket:    return "Key_LeftBracket";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Backslash:      return "Key_Backslash";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Oem102:         return "Key_Oem102";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightBracket:   return "Key_RightBracket";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_GraveAccent:    return "Key_GraveAccent";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_CapsLock:       return "Key_CapsLock";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_ScrollLock:     return "Key_ScrollLock";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_NumLock:        return "Key_NumLock";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_PrintScreen:    return "Key_PrintScreen";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Pause:          return "Key_Pause";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad0:        return "Key_Keypad0";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad1:        return "Key_Keypad1";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad2:        return "Key_Keypad2";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad3:        return "Key_Keypad3";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad4:        return "Key_Keypad4";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad5:        return "Key_Keypad5";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad6:        return "Key_Keypad6";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad7:        return "Key_Keypad7";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad8:        return "Key_Keypad8";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Keypad9:        return "Key_Keypad9";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadDecimal:  return "Key_KeypadDecimal";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadDivide:   return "Key_KeypadDivide";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadMultiply: return "Key_KeypadMultiply";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadSubtract: return "Key_KeypadSubtract";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadAdd:      return "Key_KeypadAdd";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadEnter:    return "Key_KeypadEnter";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_KeypadEqual:    return "Key_KeypadEqual";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftShift:      return "Key_LeftShift";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftCtrl:       return "Key_LeftCtrl";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftAlt:        return "Key_LeftAlt";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_LeftSuper:      return "Key_LeftSuper";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightShift:     return "Key_RightShift";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightCtrl:      return "Key_RightCtrl";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightAlt:       return "Key_RightAlt";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_RightSuper:     return "Key_RightSuper";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Menu:           return "Key_Menu";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_0:              return "Key_0";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_1:              return "Key_1";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_2:              return "Key_2";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_3:              return "Key_3";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_4:              return "Key_4";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_5:              return "Key_5";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_6:              return "Key_6";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_7:              return "Key_7";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_8:              return "Key_8";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_9:              return "Key_9";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_A:              return "Key_A";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_B:              return "Key_B";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_C:              return "Key_C";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_D:              return "Key_D";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_E:              return "Key_E";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F:              return "Key_F";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_G:              return "Key_G";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_H:              return "Key_H";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_I:              return "Key_I";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_J:              return "Key_J";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_K:              return "Key_K";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_L:              return "Key_L";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_M:              return "Key_M";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_N:              return "Key_N";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_O:              return "Key_O";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_P:              return "Key_P";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Q:              return "Key_Q";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_R:              return "Key_R";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_S:              return "Key_S";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_T:              return "Key_T";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_U:              return "Key_U";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_V:              return "Key_V";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_W:              return "Key_W";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_X:              return "Key_X";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Y:              return "Key_Y";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_Z:              return "Key_Z";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F1:             return "Key_F1";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F2:             return "Key_F2";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F3:             return "Key_F3";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F4:             return "Key_F4";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F5:             return "Key_F5";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F6:             return "Key_F6";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F7:             return "Key_F7";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F8:             return "Key_F8";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F9:             return "Key_F9";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F10:            return "Key_F10";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F11:            return "Key_F11";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F12:            return "Key_F12";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F13:            return "Key_F13";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F14:            return "Key_F14";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F15:            return "Key_F15";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F16:            return "Key_F16";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F17:            return "Key_F17";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F18:            return "Key_F18";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F19:            return "Key_F19";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F20:            return "Key_F20";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F21:            return "Key_F21";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F22:            return "Key_F22";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F23:            return "Key_F23";
                    case ApplicationPlatformBackendKey::ApplicationPlatformBackendKey_F24:            return "Key_F24";
                    default: return "Key_None";
                }

                return "Key_None";
            }
        };

        /**
         * @brief This struct encapulates platform backend keyboard key modifier state.
         * @struct ApplicationPlatformBackendKeyModifier
         */
        struct ApplicationPlatformBackendKeyModifier
        {
            /**
             * @brief This enum encodes platform backend keyboard key modifier code.
             * @enum Modifier
             */
            enum Modifier : int
            {
                ApplicationPlatformBackendKeyModifier_Begin,
                ApplicationPlatformBackendKeyModifier_Alt = ApplicationPlatformBackendKeyModifier_Begin,
                ApplicationPlatformBackendKeyModifier_Ctrl,
                ApplicationPlatformBackendKeyModifier_Shift,
                ApplicationPlatformBackendKeyModifier_End,
            };

            bool Active = false; ///< true if keyboard modifier is active (applied)

            /**
             * @brief Converts keyboard key modifier to a string.
             * @param _Modifier keyboard key modifier code.
             * @return returns keyboard key modifier name.
             */
            static std::string to_string(const ApplicationPlatformBackendKeyModifier::Modifier& _Modifier)
            {
                switch (_Modifier)
                {
                    case ApplicationPlatformBackendKeyModifier_Alt:   return "KeyModifier_Alt";
                    case ApplicationPlatformBackendKeyModifier_Ctrl:  return "KeyModifier_Ctrl";
                    case ApplicationPlatformBackendKeyModifier_Shift: return "KeyModifier_Shift";
                    default: return "KeyModifier_End";
                }

                return "KeyModifier_End";
            }
        };

        /**
         * @brief This struct encapulates platform backend mouse button state
         * @struct ApplicationPlatformBackendMouseButton
         */
        struct ApplicationPlatformBackendMouseButton
        {
            /**
             * @brief This enum encodes platform backend mouse button code.
             * @enum Button
             */
            enum Button : int
            {
                ApplicationPlatformBackendMouseButtonBegin,
                ApplicationPlatformBackendMouseButtonLeft = ApplicationPlatformBackendMouseButtonBegin,
                ApplicationPlatformBackendMouseButtonRight,
                ApplicationPlatformBackendMouseButtonMiddle,
                ApplicationPlatformBackendMouseButtonEnd
            };

            int                              Clicks       {0    };                                                 ///< mouse button clicks count
            bool                             Down         {false};                                                 ///< true if mouse button is down
            bool                             Hold         {false};                                                 ///< true if mouse button is hold
            bool                             Pressed      {false};                                                 ///< true if mouse button is pressed
            bool                             Released     {false};                                                 ///< true if mouse button is released
            bool                             Clicked      {false};                                                 ///< true if mouse button is clicked
            bool                             DoubleClicked{false};                                                 ///< true if mouse button is double clicked
            Frenchie::Core::Clock::TimePoint PressTime    {Frenchie::Core::Clock::TimePoint()}; ///< time point when mosue button is pressed
            Frenchie::Core::Clock::TimePoint ReleaseTime  {Frenchie::Core::Clock::TimePoint()}; ///< time point when mosue button is released

            /**
             * @brief Converts mouse button code to a string.
             * @param _Button mouse button code 
             * @return returns mouse button name.
             */
            static std::string to_string(const ApplicationPlatformBackendMouseButton::Button& _Button)
            {
                switch (_Button)
                {
                    case ApplicationPlatformBackendMouseButtonLeft:   return "MouseButtonLeft";
                    case ApplicationPlatformBackendMouseButtonRight:  return "MouseButtonRight";
                    case ApplicationPlatformBackendMouseButtonMiddle: return "MouseButtonMiddle";
                    default: return "MouseButtonNone";
                }

                return "MouseButtonNone";
            }
        };

        /**
         * @brief This struct encapsulates platform backend mouse cursor state.
         * @struct ApplicationPlatformBackendMouseCursor
         */
        struct ApplicationPlatformBackendMouseCursor
        {
            bool      Entered           {false};         ///< true when cursor enters context window
            gs_vec2f  Position          {gs_vec2f(0.f)}; ///< cursor position
            gs_vec2f  MousePressPosition{gs_vec2f(0.f)}; ///< cursor position at the moment when mouse button pressed
            gs_vec2f  DragDelta         {gs_vec2f(0.f)}; ///< cursor drag delta
        };

        /**
         * @brief This struct encapsulates platform backend context window state.
         * @struct ApplicationPlatformBackendWindow
         */
        struct ApplicationPlatformBackendWindow
        {
            bool Focused {false}; ///< true when window is focused
        };

        /**
         * @brief This struct encapsulates platform backend input state.
         * @struct ApplicationPlatformBackendInput
         */
        struct ApplicationPlatformBackendInput
        {            
            ApplicationPlatformBackendMouseButton  MouseButtons     [ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonEnd]{}; ///< mouse buttons state
            ApplicationPlatformBackendMouseCursor  MouseCursor      {ApplicationPlatformBackendMouseCursor()};                                                   ///< mouse cursor state
            ApplicationPlatformBackendWindow       Window           {ApplicationPlatformBackendWindow()};                                                        ///< context window state
            ApplicationPlatformBackendKeyModifier  Modifiers        [ApplicationPlatformBackendKeyModifier::ApplicationPlatformBackendKeyModifier_End]{};        ///< keyboard key modifiers state
            ApplicationPlatformBackendKey          Keys             [ApplicationPlatformBackendKey::Key::ApplicationPlatformBackendKey_NamedKey_END]{};          ///< keyboard keys state
            Frenchie::Core::Optional<unsigned int> Character;                                                                                                    ///< currently input character
            gs_vec2f                               MouseScrollOffset{gs_vec2f(0.f, 0.f)};                                                                        ///< current normalized mouse wheel scroll offset
            gs_vec2f                               WindowSize;                                                                                                   ///< context window current size
            gs_vec2f                               WindowPosition;                                                                                               ///< context window current position
            gs_vec2f                               FrameBufferSize;                                                                                              ///< context window current framebuffer size
        };

        /**
         * @brief This class wraps system specific backend state.
         * @class FrenchieApplicationPlatformApi
         * @details This class wraps system specific backend state. By default only system input and context window pointer are stored.
         * If yout want to implement your own platform bakcend and this backend has state you need to inherit from this class.
         * See FrenchieApplicationPlatformBackendGLFWOpenGL.cpp and FrenchieApplicationPlatformBackendSDL3OpenGL.cpp.
         */
        struct FrenchieApplicationPlatformApi
        {
            FrenchieApplicationPlatformApi(){}
            virtual ~FrenchieApplicationPlatformApi(){}

            void*                           Window = nullptr; ///< context window pointer
            ApplicationPlatformBackendInput Input;            ///< backend input (mouse, keyboard events e.t.c)
        };

        /**
         * @brief Class that wraps platform backend functionality.
         * @class ApplicationPlatformBackend
         * @details Class that wraps platform backend functionality. As the platform is the only one then this class is static.
         */
        class ApplicationPlatformBackend final
        {
        public:

            ApplicationPlatformBackend() = delete;
            ApplicationPlatformBackend(const ApplicationPlatformBackend&) = delete;
            ApplicationPlatformBackend& operator=(const ApplicationPlatformBackend&) = delete;

            // API

            /**
             * @brief In this function we create context window and load rendering backend graphics API.
             * @return returns true if context window and graphics API load is successfull.
             */
            static bool awake();

            /**
             * @brief In this function we usually poll context window events.
             */
            static void frame_start();

            /**
             * @brief In this function we usually adjust window viewport, size and posistion.
             */
            static void frame_update();

            /**
             * @brief In this function we usually swap context window backbuffers.
             */
            static void frame_finish();

            /**
             * @brief In this function we safelly dispose context window and rendering backend resources.
             */
            static void quit();

            /**
             * @brief This function checks if context window is closed.
             * @returns returns true if context window is closed. Used within application layered loop.
             */
            static bool is_closed();

            /**
             * @brief forces application context window to close.
             */
            static void close();

            // getters

            /**
             * @brief returns context window name.
             * @return returns context window name.
             */
            static std::string get_window_name();

            /**
             * @brief returns context window clipboard text.
             * @return returns context window clipboard text.
             */
            static std::string get_clipboard_text();

            /**
             * @brief returns context window size.
             * @return returns context window size.
             */
            static gs_vec2f    get_window_size();

            /**
             * @brief returns context window position.
             * @return returns context window position.
             */
            static gs_vec2f    get_window_position();

            /**
             * @brief returns context window cursor position.
             * @return returns context window cursor position.
             */
            static gs_vec2f    get_window_cursor_position();

            /**
             * @brief returns context window cursor dragdelta.
             * @return returns context window cursor dragdelta.
             */
            static gs_vec2f    get_window_cursor_dragdelta();

            /**
             * @brief returns context window framebuffer size.
             * @return returns context window framebuffer size.
             */
            static gs_vec2f    get_window_framebuffer_size();

            /**
             * @brief returns context window normalized mouse wheel scroll offset [-1; +1].
             * @return returns context window normalized mouse wheel scroll offset [-1; +1].
             */
            static gs_vec2f    get_mouse_wheel_scroll_offset();

            /**
             * @brief returns context window key clicks count.
             * @param _Key input keyboard key
             * @return returns context window key clicks count.
             */
            static int  key_clicks_count(const ApplicationPlatformBackendKey::Key& _Key);

            /**
             * @brief returns context window input text in UTF-8.
             * @return returns context window input text in UTF-8.
             */
            static std::string input_text();

            /**
             * @brief returns platform API state holder.
             * @return returns platform API state holder.
             */
            template <typename T = FrenchieApplicationPlatformApi>
            static std::shared_ptr<T> platform_api()
            {
                return std::dynamic_pointer_cast<T>(m_Api);
            }

            // predicates

            /**
             * @brief Checks if there is an input text.
             * @return returns true if there is an input text. 
             */
            static bool has_input_text();
            
            /**
             * @brief Checks if there is text within context window clipboard.
             * @return returns true if there is text within context window clipboard.
             */
            static bool has_clipboard_text();

            /**
             * @brief Checks if mouse button is down.
             * @param _Button mouse button
             * @return returns true  if mouse button is down.
             */
            static bool is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button& _Button);

            /**
             * @brief Checks if mouse button is hold.
             * @param _Button mouse button.
             * @return returns true if mouse button is hold.
             */
            static bool is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button& _Button);

            /**
             * @brief Checks if mouse button has been pressed.
             * @param _Button mouse button.
             * @return returns true if mouse button has been pressed.
             */
            static bool is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button& _Button);

            /**
             * @brief Checks if mouse button has been released.
             * @param _Button mouse button.
             * @return returns true if mouse button has been pressed.
             */
            static bool is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button& _Button);

            /**
             * @brief Checks if mouse button has been clicked.
             * @param _Button mouse button.
             * @return returns true if mouse button has been clicked.
             */
            static bool is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button);

            /**
             * @brief Checks if mouse button has been double clicked.
             * @param _Button mouse button.
             * @return returns true if mouse button has been double clicked.
             */
            static bool is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button);

            /**
             * @brief Returns mouse button clicks count.
             * @param _Button mouse button.
             * @return returns mouse button clicks count.
             */
            static int  mouse_button_clicks_count(const ApplicationPlatformBackendMouseButton::Button& _Button);

            /**
             * @brief Checks if keyboard key is down.
             * @param _Key keyboard key
             * @return returns true if keyboard key is down.
             */
            static bool is_key_down(const ApplicationPlatformBackendKey::Key& _Key);

            /**
             * @brief Checks if keyboard key is hold.
             * @param _Key keyboard key
             * @return returns true if keyboard key is hold.
             */
            static bool is_key_hold(const ApplicationPlatformBackendKey::Key& _Key);

            /**
             * @brief Checks if keyboard key has been pressed.
             * @param _Key keyboard key
             * @return returns true if keyboard key has been pressed.
             */
            static bool is_key_pressed(const ApplicationPlatformBackendKey::Key& _Key);

            /**
             * @brief Checks if keyboard key has been released.
             * @param _Key keyboard key
             * @return returns true if keyboard key has been released.
             */
            static bool is_key_released(const ApplicationPlatformBackendKey::Key& _Key);

            /**
             * @brief Checks if keyboard key has been clicked.
             * @param _Key keyboard key
             * @return returns true if keyboard key has been clicked.
             */
            static bool is_key_clicked(const ApplicationPlatformBackendKey::Key& _Key);

            /**
             * @brief Checks if keyboard key modifier (CRTL, SHIFT e.t.c) has been applied.
             * @param _Modifier keyboard key modifier
             * @return returns true if keyboard key modifier (CRTL, SHIFT e.t.c) has been applied.
             */
            static bool has_modifier(const ApplicationPlatformBackendKeyModifier::Modifier& _Modifier);

            /**
             * @brief Sets context window name
             * @param _Name context window name
             */
            static void set_window_name(const std::string& _Name);

            /**
             * @brief Sets context window clipboard text
             * @param _Text context window clipboard text
             */
            static void set_clipboard_text(const std::string& _Text);

        private:

            // context
            static std::shared_ptr<FrenchieApplicationPlatformApi> m_Api;

            // service methods
            static void collect_input();
            static void restore_input();
        };

        /*! @} */
    }
}