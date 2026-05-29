#pragma once

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationLayerRenderingQueue2D.hpp>

// STL
#include <type_traits>
#include <functional>
#include <stdarg.h>
#include <memory>
#include <chrono>
#include <stack>
#include <map>
#include <set>

/*! \defgroup <ApplicationLayers> (Application layers)
*  @brief The module contains main application layers.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Application
    {
        /*! \defgroup <ApplicationImmediateModeUserInterface> (Application immediate mode user interface)
        *  @ingroup ApplicationLayers
        *  @brief The module provides classes and utilities for creating UI in immediate mode.
        *  @details The module provides classes and utilities for creating UI in immediate mode.
        * Immediate mode means that all UI and it's style is recalculated every frame that is usefull
        * for data driven real time allications or game development.
        *  @{
        */

        // Events

        /**
         * @brief This enum declares basic UI node events 
         * @enum ImmediateUserInterfaceNodeEvents_
         */
        enum ImmediateUserInterfaceNodeEvents_ : int
        {
            // sentinel
            ImmediateUserInterfaceNodeEvents_None                 = 0,
            
            // move
            ImmediateUserInterfaceNodeEvents_IsMoved              = 1 << 0, ///< indicates that node is being moved
            
            // resize
            ImmediateUserInterfaceNodeEvents_IsResizedTop         = 1 << 1, ///< indicates that node is being resized by dragging top part of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedLeft        = 1 << 2, ///< indicates that node is being resized by dragging left part of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedRight       = 1 << 3, ///< indicates that node is being resized by dragging right part of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedBottom      = 1 << 4, ///< indicates that node is being resized by dragging bottom part of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedTopLeft     = 1 << 5, ///< indicates that node is being resized by dragging top left corner of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedTopRight    = 1 << 6, ///< indicates that node is being resized by dragging top right corner of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedBottomLeft  = 1 << 7, ///< indicates that node is being resized by dragging bottom left corner of it's bounding box
            ImmediateUserInterfaceNodeEvents_IsResizedBottomRight = 1 << 8, ///< indicates that node is being resized by dragging bottom right corner of it's bounding box

            // custom event
            ImmediateUserInterfaceNodeEvents_Custom               = 1 << 9, ///< is used for user defined events
        };

        /**
         * @brief This enum declares immediate user interface contextual layer settings
         * @enum ImmediateUserInterfaceNodeMouseHover_
         */
        enum ImmediateUserInterfaceNodeMouseHover_ : int
        {
            ImmediateUserInterfaceNodeMouseHover_None         = 0,
            ImmediateUserInterfaceNodeMouseHover_MouseLeft    = 1 << 0, ///< is set when mouse leaves bounding box of previously hovered node
            ImmediateUserInterfaceNodeMouseHover_MouseHovered = 1 << 1, ///< is set if mouse is within bounding box of a node
            ImmediateUserInterfaceNodeMouseHover_MouseEntered = 1 << 2, ///< is set when mouse enters bounding box of node
        };

        // Colors
        /**
         * @brief This enum declares basic UI colors
         * @enum ImmediateUserInterfaceNodeColors_
         */
        enum ImmediateUserInterfaceNodeColors_ : int
        {
            ImmediateUserInterfaceNodeColors_Begin            = 0,

            // layouts/windows e.t.c
            ImmediateUserInterfaceNodeColors_ChildBackground = ImmediateUserInterfaceNodeColors_Begin,  ///< child UI elements background color
            ImmediateUserInterfaceNodeColors_ParentBackground,                                          ///< parent UI elements background color
            ImmediateUserInterfaceNodeColors_ParentBackgroundHovered,                                   ///< hovered parent UI elements background color

            // buttons
            ImmediateUserInterfaceNodeColors_ButtonOutline,                                             ///< push button, check button, radio button, slider button, combobox, input text outline color
            ImmediateUserInterfaceNodeColors_ButtonBackground,                                          ///< push button, check button, radio button, slider button, combobox, input text background color
            ImmediateUserInterfaceNodeColors_ButtonBackgroundHovered,                                   ///< hovered push button, check button, radio button, slider button, combobox, input text background color
            ImmediateUserInterfaceNodeColors_ButtonBackgroundPressed,                                   ///< pressed push button, check button, radio button, slider button, combobox, input text background color

            // scrollbar
            ImmediateUserInterfaceNodeColors_ScrollBarSliderBackground,                                 ///< scroll bar slider background color
            ImmediateUserInterfaceNodeColors_ScrollBarSliderBackgroundHovered,                          ///< hovered scroll bar slider background color

            // menus
            ImmediateUserInterfaceNodeColors_MenuActionBackground,                                      ///< menu action background
            ImmediateUserInterfaceNodeColors_MenuActionBackgroundHovered,                               ///< hovered menu action background
            ImmediateUserInterfaceNodeColors_MenuActionBackgroundPressed,                               ///< hovered menu action background

            ImmediateUserInterfaceNodeColors_ProgressbarOutline,                                        ///< progressbar outline
            ImmediateUserInterfaceNodeColors_ProgressbarBackground,                                     ///< progressbar background

            // gizmos
            ImmediateUserInterfaceNodeColors_Gizmos,                                                    ///< gizmos background
            ImmediateUserInterfaceNodeColors_GizmosHovered,                                             ///< hovered gizmos background

            // plotting
            ImmediateUserInterfaceNodeColors_2DPlotsAxis,                                               ///< 2D plots XY axis color
            ImmediateUserInterfaceNodeColors_2DPlotsAxisTicks,                                          ///< 2D plots XY axis color
            ImmediateUserInterfaceNodeColors_2DPlotsBackground,                                         ///< 2D plots background color

            // text
            ImmediateUserInterfaceNodeColors_Text,                                                      ///< text color
            
            ImmediateUserInterfaceNodeColors_End
        };

        // Settings
        /**
         * @brief This enum declares basic UI node settings
         * @enum ImmediateUserInterfaceNodeSettings_
         */
        enum ImmediateUserInterfaceNodeSettings_ : int
        {
            // sentinel
            ImmediateUserInterfaceNodeSettings_None                                   = 0,

            // modifications
            ImmediateUserInterfaceNodeSettings_Movable                                = 1 << 0, ///< makes node movable
            ImmediateUserInterfaceNodeSettings_Resizable                              = 1 << 1, ///< makes node resizable
            ImmediateUserInterfaceNodeSettings_NullParent                             = 1 << 2, ///< makes node ignore it's parent within hierarchy

            // content alignment
            ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop            = 1 << 3, ///< content is aligned top on vertical axis
            ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter         = 1 << 4, ///< content is aligned center on vertical axis
            ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom         = 1 << 5, ///< content is aligned bottom on vertical axis
            ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft         = 1 << 6, ///< content is aligned left on horizontal axis
            ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter       = 1 << 7, ///< content is aligned center on horizontal axis
            ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight        = 1 << 8, ///< content is aligned right on horizontal axis

            // scrollbars
            ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar                 = 1 << 9,  ///< vertical scrollbar will always be disabled
            ImmediateUserInterfaceNodeSettings_AlwaysVerticalScrollBar                = 1 << 10, ///< vertical scrollbar will always be enabled
            ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar              = 1 << 11, ///< vertical scrollbar will be enabled when needed
            ImmediateUserInterfaceNodeSettings_NeverHorizontalScrollBar               = 1 << 12, ///< horizontal scrollbar will always be disabled
            ImmediateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar              = 1 << 13, ///< horizontal scrollbar will always be enabled
            ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar            = 1 << 14, ///< horizontal scrollbar will be enabled when needed
            ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically             = 1 << 15, ///< node with scrollarea will be resized to it's contents vertically
            ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally           = 1 << 16, ///< node with scrollarea will be resized to it's contents horizontally
            ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment   = 1 << 17, ///< vertical scroll bar position will be adjusted by up/down keyboard arrows
            ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment = 1 << 18, ///< horizontal scroll bar position will be adjusted by left/right keyboard arrows
            ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment  = 1 << 19, ///< vertical scroll bar position will be adjusted by a mouse wheel
            ImmediateUserInterfaceNodeSettings_InvisibleVerticalScrollBar             = 1 << 20, ///< vertical scrollbar will be invisible
            ImmediateUserInterfaceNodeSettings_InvisibleHorizontalScrollBar           = 1 << 21, ///< horizontal scrollbar will be invisible

            // blur
            ImmediateUserInterfaceNodeSettings_ShowBlur                               = 1 << 22, ///< enables blured background for dialogs

            // layout
            ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren              = 1 << 23, ///< clamps layout element size to zero when there are no children in it

            ImmediateUserInterfaceNodeSettings_AllowedModificationsDefaults           = 
                  ImmediateUserInterfaceNodeSettings_Movable
                | ImmediateUserInterfaceNodeSettings_Resizable,

            ImmediateUserInterfaceNodeSettings_ScrollAreaDefaults                     =
                  ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar
                | ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar
                | ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment
                | ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment
                | ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment,

            ImmediateUserInterfaceNodeSettings_ContentAlignmentDefaults               =
                  ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop
                | ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft,

            ImmediateUserInterfaceNodeSettings_Defaults =
                  ImmediateUserInterfaceNodeSettings_AllowedModificationsDefaults
                | ImmediateUserInterfaceNodeSettings_ScrollAreaDefaults
                | ImmediateUserInterfaceNodeSettings_ContentAlignmentDefaults,
        };

        /**
         * @brief This enum declares text label settings
         * @enum ImmediateUserInterfaceLabelSettings_
         */
        enum ImmediateUserInterfaceLabelSettings_ : int
        {
            ImmediateUserInterfaceLabelSettings_None        = 0,
            ImmediateUserInterfaceLabelSettings_AlignLeft   = 1 << 0, ///< aligns text left
            ImmediateUserInterfaceLabelSettings_AlignRight  = 1 << 1, ///< aligns text right
            ImmediateUserInterfaceLabelSettings_AlignCenter = 1 << 2, ///< aligns text center
        };

        /**
         * @brief This enum declares tree node settings
         * @enum ImmediateUserInterfaceTreeNodeSettings_
         */
        enum ImmediateUserInterfaceTreeNodeSettings_ : int
        {
            ImmediateUserInterfaceTreeNodeSettings_OpenOnClick           = 1 << 0, ///< tree node opened on mouse click
            ImmediateUserInterfaceTreeNodeSettings_OpenOnDoubleClick     = 1 << 1, ///< tree node opened on mouse double click
            ImmediateUserInterfaceTreeNodeSettings_RenderConnectionLines = 1 << 2, ///< tree node siblings connection lines are drawn

            ImmediateUserInterfaceTreeNodeSettings_Defaults              =
                  ImmediateUserInterfaceTreeNodeSettings_OpenOnDoubleClick
                | ImmediateUserInterfaceTreeNodeSettings_RenderConnectionLines
        };

        /**
         * @brief This enum declares check button settings
         * @enum ImmediateUserInterfaceCheckButtonSettings_
         */
        enum ImmediateUserInterfaceCheckButtonSettings_ : int
        {
            ImmediateUserInterfaceCheckButtonSettings_Checkbox     = 1 << 0, ///< check button is rendered as checkbox
            ImmediateUserInterfaceCheckButtonSettings_RadioButton  = 1 << 1, ///< check button is rendered as radio button
            ImmediateUserInterfaceCheckButtonSettings_SliderButton = 1 << 2, ///< check button is rendered as slider button
            ImmediateUserInterfaceCheckButtonSettings_Checkable    = 1 << 3, ///< check button changes boolean variable passed to it

            ImmediateUserInterfaceCheckButtonSettings_Defaults     =
                ImmediateUserInterfaceCheckButtonSettings_Checkbox
                | ImmediateUserInterfaceCheckButtonSettings_Checkable,
        };

        /**
         * @brief This enum declares input string settings
         * @enum ImmediateUserInterfaceInputStringSettings_
         */
        enum ImmediateUserInterfaceInputStringSettings_ : int
        {
            ImmediateUserInterfaceInputStringSettings_NoInput           = 1 << 0, ///< disables input
            ImmediateUserInterfaceInputStringSettings_Password          = 1 << 1, ///< all input symbols are changed on '*' while rendering
            ImmediateUserInterfaceInputStringSettings_NoClipboard       = 1 << 2, ///< disables copy/paste actions
            ImmediateUserInterfaceInputStringSettings_NoSelection       = 1 << 3, ///< disables selection
            ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter = 1 << 4, ///< input string function returns true when enter key is pressed
            ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEdit  = 1 << 5, ///< input string function returns true when text is being edited
            ImmediateUserInterfaceInputStringSettings_StopEditOnEscape  = 1 << 6, ///< input string function stops editing when escape key is pressed
        };

        /**
         * @brief This enum declares input scalar widget settings
         * @enum ImmediateUserInterfaceInputScalarSettings_
         */
        enum ImmediateUserInterfaceInputScalarSettings_ : int
        {
            ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEnter = 1 << 0, ///< input scalar function returns true when enter key is pressed
            ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEdit  = 1 << 1, ///< input scalar function returns true when text is being edited
            ImmediateUserInterfaceInputScalarSettings_StopEditOnEscape  = 1 << 2, ///< input scalar function stops editing when escape key is pressed

            ImmediateUserInterfaceInputScalarSettings_Defaults          =
                  ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEnter
                | ImmediateUserInterfaceInputStringSettings_StopEditOnEscape
        };

        /**
         * @brief This enum declares color picker widgets settings
         * @enum ImmediateUserInterfaceColorPickerSettings_
         */
        enum ImmediateUserInterfaceColorPickerSettings_ : int
        {
            ImmediateUserInterfaceColorPickerSettings_None         = 0,      ///< sentinel
            ImmediateUserInterfaceColorPickerSettings_EditRGB      = 1 << 0, ///< enables RGB   editor in color picker
            ImmediateUserInterfaceColorPickerSettings_EditHSV      = 1 << 1, ///< enables HSV   editor in color picker
            ImmediateUserInterfaceColorPickerSettings_EditHSL      = 1 << 2, ///< enables HSL   editor in color picker
            ImmediateUserInterfaceColorPickerSettings_EditAlpha    = 1 << 3, ///< enables alpha editor in color picker
            ImmediateUserInterfaceColorPickerSettings_PreviewColor = 1 << 4, ///< enables color preview image

            ImmediateUserInterfaceColorPickerSettings_Defaults  =
                  ImmediateUserInterfaceColorPickerSettings_EditRGB
                | ImmediateUserInterfaceColorPickerSettings_EditHSV
                | ImmediateUserInterfaceColorPickerSettings_EditHSL
                | ImmediateUserInterfaceColorPickerSettings_EditAlpha
                | ImmediateUserInterfaceColorPickerSettings_PreviewColor
        };

        /**
         * @brief This enum declares 2D line plot settings
         * @enum ImmediateUserInterfacePlotLineSettings_
         */
        enum ImmediateUserInterfacePlotLineSettings_ : int
        {
            ImmediateUserInterfacePlotLineSettings_None                 = 0,      ///< sentinel

            ImmediateUserInterfacePlotLineSettings_RenderAsLines        = 1 << 0, ///< renders 2D line plot as line pieces
            ImmediateUserInterfacePlotLineSettings_RenderAsStems        = 1 << 1, ///< renders 2D line plot as stems
            ImmediateUserInterfacePlotLineSettings_RenderAsPoints       = 1 << 2, ///< renders 2D line plot as points
            ImmediateUserInterfacePlotLineSettings_RenderAsRectangles   = 1 << 3, ///< renders 2D line plot as rectangles
            ImmediateUserInterfacePlotLineSettings_RenderAsConvexAreas  = 1 << 4, ///< renders 2D line plot as convex areas

            ImmediateUserInterfacePlotLineSettings_MarkersOpened        = 1 << 5, ///< markes are opened
            ImmediateUserInterfacePlotLineSettings_MarkersPoints        = 1 << 6, ///< renders point markers
            ImmediateUserInterfacePlotLineSettings_MarkersTriangles     = 1 << 7, ///< renders triangular markers
            ImmediateUserInterfacePlotLineSettings_MarkersRectangles    = 1 << 8, ///< renders rectangular markers

            ImmediateUserInterfacePlotLineSettings_RenderLabelsOnHover  = 1 << 9, ///< when plot point is hovered label with point values is rendered
            ImmediateUserInterfacePlotLineSettings_HighlightOnAxisHover = 1 << 10, ///< when axis is hovered the plot line attached to that axis is highlighted

            ImmediateUserInterfacePlotLineSettings_Defaults =
                  ImmediateUserInterfacePlotLineSettings_RenderAsLines
                | ImmediateUserInterfacePlotLineSettings_RenderLabelsOnHover
                | ImmediateUserInterfacePlotLineSettings_HighlightOnAxisHover
        };

        /**
         * @brief This enum declares immediate user interface contextual layer settings
         * @enum ImmediateUserInterfaceContextSettings_
         */
        enum ImmediateUserInterfaceContextSettings_ : int
        {
            // docking
            ImmediateUserInterfaceContextSettings_None                             = 1 << 0, ///< disables all docking features
            ImmediateUserInterfaceContextSettings_EnableWindowsDocking             = 1 << 1, ///< enables windows mutual docking
            ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking           = 1 << 2, ///< enables workspace dock area

            // highlighting
            ImmediateUserInterfaceContextSettings_HighlighHoveredNodes             = 1 << 3, ///< enables hovered node highligting by a semi-transparent rectangle
        
            // .ini file
            ImmediateUserInterfaceContextSettings_SaveStyleSettingsToIniFile       = 1 << 4, ///< saves style settings to .ini file
        };

        /**
         * @brief This enum declares rendering order of UI nodes
         * @enum ImmedidateUserInterfaceRenderingOrder_
         */
        enum ImmedidateUserInterfaceRenderingOrder_ : int
        {
            ImmedidateUserInterfaceRenderingOrder_Begin      = 0,
            ImmedidateUserInterfaceRenderingOrder_Background = ImmedidateUserInterfaceRenderingOrder_Begin, ///< background UI elements
            ImmedidateUserInterfaceRenderingOrder_Main,                                                     ///< default UI elements rendering oreder
            ImmedidateUserInterfaceRenderingOrder_Focus,                                                    ///< focused UI elements
            ImmedidateUserInterfaceRenderingOrder_Modal,                                                    ///< modal UI elements (dialogs)
            ImmedidateUserInterfaceRenderingOrder_Popup,                                                    ///< popup UI elements (popup menus)
            ImmedidateUserInterfaceRenderingOrder_End,
        };

        typedef int ImmediateUserInterfaceNodeEvents;
        typedef int ImmediateUserInterfaceNodeMouseHover;

        typedef int ImmediateUserInterfaceNodeSettings;
        typedef int ImmediateUserInterfaceLabelSettings;
        typedef int ImmediateUserInterfaceTreeNodeSettings;
        typedef int ImmediateUserInterfaceCheckButtonSettings;
        typedef int ImmediateUserInterfaceInputStringSettings;
        typedef int ImmediateUserInterfaceInputScalarSettings;
        typedef int ImmediateUserInterfaceColorPickerSettings;

        typedef int ImmediateUserInterfacePlotLineSettings;

        typedef int ImmediateUserInterfaceContextSettings;

        typedef int ImmedidateUserInterfaceDockingAnchor;
        typedef int ImmedidateUserInterfaceRenderingOrder;
        typedef int ImmedidateUserInterfaceRenderingLayer;

        class ImmediateUserInterfaceContextLayer;

        /**
         * @brief This class defines overall UI style. It incapsulates font, color scheme settings e.t.c
         * @class ImmedidateUserInterfaceStyle
         */
        struct ImmedidateUserInterfaceStyle final
        {
            ImmedidateUserInterfaceStyle();
            ~ImmedidateUserInterfaceStyle();

            // getters

            // frames radius

            /**
             * @brief returns minimum UI frames radius
             * @return returns minimum UI frames radius 
             */
            float get_minimum_frames_radius() const;

            /**
             * @brief returns maximum UI frames radius
             * @return returns maximum UI frames radius 
             */
            float get_maximum_frames_radius() const;

            /**
             * @brief returns UI frames radius
             * @return returns UI frames radius 
             */
            float& get_frames_radius() const;

            /**
             * @brief returns minimum UI frames width
             * @return returns minimum UI frames width 
             */
            float get_minimum_frames_width() const;

            /**
             * @brief returns maximum UI frames width
             * @return returns maximum UI frames width 
             */
            float get_maximum_frames_width() const;

            /**
             * @brief returns UI frames width
             * @return returns UI frames width 
             */
            float& get_frames_width() const;

            /**
             * @brief returns minimum UI font size
             * @return returns minimum UI font size 
             */
            float get_minimum_font_size() const;

            /**
             * @brief returns maximum UI font size
             * @return returns maximum UI font size 
             */
            float get_maximum_font_size() const;

            /**
             * @brief returns UI font size
             * @return returns UI font size 
             */
            float& get_font_size() const;

            /**
             * @brief returns minimum UI scrollbar width
             * @return returns minimum UI scrollbar width
             */
            float get_minimum_scrollbar_width() const;

            /**
             * @brief returns maximum UI scrollbar width
             * @return returns maximum UI scrollbar width
             */
            float get_maximum_scrollbar_width() const;

            /**
             * @brief returns UI scrollbar width
             * @return returns UI scrollbar width
             */
            float& get_scrollbar_width() const;

            // menu pointer size
            float& get_popup_menu_pointer_size() const;

            /**
             * @brief returns currently used font
             * @return returns currently used font
             */
            ApplicationRenderingBackendFont get_current_font() const;

            /**
             * @brief returns a given color scheme color
             * @param _Color wanted color scheme color code
             * @return returns a given color scheme color
             */
            gs_color&   get_color(const ImmediateUserInterfaceNodeColors_& _Color) const;

            /**
             * @brief converts given color scheme color to a string
             * @param _Color wanted color scheme color code
             * @param _Camel if true returns color name in camel style (usefull for serialization)
             * @return returns given color scheme color name in default or camel style
             */
            std::string style_color_to_string(const ImmediateUserInterfaceNodeColors_& _Color, bool _Camel = false) const;

        private:

            // infos
            mutable float                            FramesRadius         = 32.f;
            mutable float                            FramesWidth          = 0.f;
            mutable float                            FontSize             = 32.f;
            mutable float                            ScrollBarWidth       = 32.f;
            mutable float                            PopupMenuPointerSize = 32.f;
            mutable std::vector<gs_color>            Colors;
            mutable ApplicationRenderingBackendFont  Font;
        };

        /**
         * @brief This class plays role of input catcher for UI. 
         * It collects input, mouse cursor position, viewport size e.t.c every frame
         * @class ImmedidateUserInterfaceInput
         */
        struct ImmedidateUserInterfaceInput final
        {
            ImmedidateUserInterfaceInput(ImmediateUserInterfaceContextLayer* _Context = nullptr);

            // getters
            /**
             * @brief returns cursor position relative to a viewport.
             * @return returns cursor position relative to a viewport.
             */
            gs_vec2f    get_cusor_position() const;

            /**
             * @brief returns cursor drag delta.
             * @return returns cursor drag delta.
             */
            gs_vec2f    get_cusor_drag_delta() const;

            /**
             * @brief returns cursor scroll offset.
             * @return returns cursor drag delta.
             */
            gs_vec2f    get_cusor_scroll_offset() const;

            /**
             * @brief returns input text.
             * @return returns input text.
             */
            std::string get_input_text() const;

            /**
             * @brief returns clipboard text.
             * @return returns clipboard text.
             */
            std::string get_clipboard_text() const;
            
            // predicates

            /**
             * @brief Checks if there is a text input.
             * @return returns true if there is a text input.
             */
            bool has_input_text() const;

            /**
             * @brief Checks if there is a text in clipboard.
             * @return returns true if there is a text in clipboard.
             */
            bool has_clipboard_text() const;

            // setters

            /**
             * @brief Sets text in clipboard.
             * @param _Text text that is set to clipboard.
             */
            void set_clipboard_text(const std::string& _Text);

            /**
             * @brief Checks if any mouse button is down
             * @return returns true if any mouse button is down 
             */
            bool is_mouse_button_down() const;

            /**
             * @brief Checks if any mouse button is hold.
             * @return returns true if any mouse button is hold.
             */
            bool is_mouse_button_hold() const;

            /**
             * @brief Checks if any mouse button is pressed.
             * @return returns true if any mouse button is pressed.
             */
            bool is_mouse_button_pressed() const;

            /**
             * @brief Checks if any mouse button is released.
             * @return returns true if any mouse button is released.
             */
            bool is_mouse_button_released() const;

            /**
             * @brief Checks if any mouse button is clicked.
             * @return returns true if any mouse button is clicked.
             */
            bool is_mouse_button_clicked() const;

            /**
             * @brief Checks if any mouse button is double clicked.
             * @return returns true if any mouse button is double clicked.
             */
            bool is_mouse_button_double_clicked() const;

            /**
             * @brief Checks if concrete mouse button is down
             * @param _Button checked mouse button code
             * @return returns true if _Button mouse button is down 
             */
            bool is_mouse_button_down(const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief Checks if concrete mouse button is hold
             * @param _Button checked mouse button code
             * @return returns true if _Button mouse button is hold 
             */
            bool is_mouse_button_hold(const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief Checks if concrete mouse button is pressed
             * @param _Button checked mouse button code
             * @return returns true if _Button mouse button is pressed 
             */
            bool is_mouse_button_pressed(const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief Checks if concrete mouse button is released
             * @param _Button checked mouse button code
             * @return returns true if _Button mouse button is released 
             */
            bool is_mouse_button_released(const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief Checks if concrete mouse button is clicked
             * @param _Button checked mouse button code
             * @return returns true if _Button mouse button is clicked 
             */
            bool is_mouse_button_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief Checks if concrete mouse button is double clicked
             * @param _Button checked mouse button code
             * @return returns true if _Button mouse button is double clicked 
             */
            bool is_mouse_button_double_clicked(const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief Checks if any keyboard key is down
             * @return returns true if any keyboard key is down
             */
            bool is_key_down() const;

            /**
             * @brief Checks if any keyboard key is hold
             * @return returns true if any keyboard key is hold
             */
            bool is_key_hold() const;

            /**
             * @brief Checks if any keyboard key is pressed
             * @return returns true if any keyboard key is pressed
             */
            bool is_key_pressed() const;

            /**
             * @brief Checks if any keyboard key is released
             * @return returns true if any keyboard key is released
             */
            bool is_key_released() const;

            /**
             * @brief Checks if any keyboard key is clicked
             * @return returns true if any keyboard key is clicked
             */
            bool is_key_clicked() const;

            /**
             * @brief Checks if concrete keyboard key is down
             * @param _Key keyboard key code
             * @return returns true if _Key keyboard key is down
             */
            bool is_key_down(const ApplicationPlatformBackendKey::Key& _Key) const;

            /**
             * @brief Checks if concrete keyboard key is hold
             * @param _Key keyboard key code
             * @return returns true if _Key keyboard key is hold
             */
            bool is_key_hold(const ApplicationPlatformBackendKey::Key& _Key) const;

            /**
             * @brief Checks if concrete keyboard key is pressed
             * @param _Key keyboard key code
             * @return returns true if _Key keyboard key is pressed
             */
            bool is_key_pressed(const ApplicationPlatformBackendKey::Key& _Key ) const;

            /**
             * @brief Checks if concrete keyboard key is released
             * @param _Key keyboard key code
             * @return returns true if _Key keyboard key is released
             */
            bool is_key_released(const ApplicationPlatformBackendKey::Key& _Key) const;

            /**
             * @brief Checks if concrete keyboard key is clicked
             * @param _Key keyboard key code
             * @return returns true if _Key keyboard key is clicked
             */
            bool is_key_clicked(const ApplicationPlatformBackendKey::Key& _Key) const;

            /**
             * @brief Checks if any modifier is applied to mouse button or keyboard key input
             * @param _Modifier modifier code
             * @return returns true if any modifier is applied to mouse button or keyboard key input
             */
            bool has_modifier(const ApplicationPlatformBackendKeyModifier::Modifier& _Modifier) const;

        private:
            ImmediateUserInterfaceContextLayer* m_Context = nullptr;
        };

        // This is the base UI node class that stores UI node state
        struct ImmediateUserInterfaceNode
        {
            ImmediateUserInterfaceNode(const std::string& _Name);
            virtual ~ImmediateUserInterfaceNode();

            virtual void render(ImmediateUserInterfaceContextLayer* _Context);
            virtual void layout(ImmediateUserInterfaceContextLayer* _Context);
            virtual void measure(ImmediateUserInterfaceContextLayer* _Context);
            virtual bool events(ImmediateUserInterfaceContextLayer* _Context);
            virtual void attach_child(ImmediateUserInterfaceNode* _Child);

            virtual bool create_contents(
                ImmediateUserInterfaceContextLayer*       _Context, 
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr);
            
            virtual void load_state(ImmediateUserInterfaceContextLayer*);
            virtual void save_state(ImmediateUserInterfaceContextLayer*);
            virtual void restore();

            // getters
            virtual gs_2dboxf get_clipping_box(ImmediateUserInterfaceContextLayer*) const;
            virtual gs_2dboxf get_visible_rect(ImmediateUserInterfaceContextLayer*) const;
            virtual bool      is_partially_visible(ImmediateUserInterfaceContextLayer*) const;
            virtual bool      is_catching_event(ImmediateUserInterfaceContextLayer*) const;
            virtual bool      is_enabled(const ImmediateUserInterfaceContextLayer*) const;

            int place_in_follow();

            int get_rendering_order() const
            {
                return RenderingOrder;
            }

            void set_rendering_order(const int& _RenderingOrder)
            {
                if(!NextRenderingOrder.has_value())
                    RenderingOrder = _RenderingOrder;
            }

            void next_rendering_order()
            {
                if(NextRenderingOrder.has_value())
                    RenderingOrder = NextRenderingOrder.value();
                NextRenderingOrder.reset();
            }

            void enable()
            {
                Active = true;
            }

            void disable()
            {
                Active = false;
            }

            struct Data
            {
                // rendering
                int                                            Depth                       {0};     // depth along Z-axis
                int                                            SelfThickness               {0};     // thickness of rendered content
                int                                            RenderingIndex              {0};     // index of the node within context rendering list
                int                                            MaximumChildDepth           {0};     // depth of the deepest child
                int                                            MaximumChildThickness       {0};     // thickness of the 'fattest' child
                bool                                           PlaceInFollow               {false}; // shows if the node places it's children in follow along Z-axis

                // geometry
                gs_2dboxf                                      BoundingBox                 {gs_2dboxf(gs_vec2f(32.f, 32.f), gs_vec2f(1024.f, 512.f))}; // node bounding box
                gs_vec2f                                       ContentSize                 {gs_vec2f(0.f, 0.f)};                                       // node contents size
                gs_vec2f                                       MinimumSize                 {gs_vec2f(32.f, 32.f)};                                     // node minimum size
                gs_vec2f                                       MaximumSize                 {gs_vec2f(gs_huge<float>(), gs_huge<float>())};             // node maximum size

                // hierarchy
                ImmediateUserInterfaceNode*                    Parent                      {nullptr}; // node hierarchical parent
                ImmediateUserInterfaceNode*                    Scope                       {nullptr}; // node from which scope this node was created

                // visibility
                mutable Frenchie::Core::Optional<gs_2dboxf>    ClippingBox;

                // settings
                ImmediateUserInterfaceNodeSettings             Settings                    {ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable | ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable};

                // events
                ImmediateUserInterfaceNodeEvents               Events                      {ImmediateUserInterfaceNodeEvents_::ImmediateUserInterfaceNodeEvents_None};
                bool                                           Selected                    {false};

                // layout hints
                int                                            NextLine                    {1  }; // vertical indents count which need to be placed after this node within scrollarea
                float                                          Indent                      {0.f}; // horizontal indents count which  need to be placed after this node within scrollarea

                // mouse hover
                ImmediateUserInterfaceNodeMouseHover           MouseHover                  {ImmediateUserInterfaceNodeMouseHover_::ImmediateUserInterfaceNodeMouseHover_None};
                Frenchie::Core::Clock::TimePoint               MouseEnterTimer             {Frenchie::Core::Clock::TimePoint()};
                Frenchie::Core::Clock::TimePoint               MouseLeaveTimer             {Frenchie::Core::Clock::TimePoint()};
            };

            Data State {Data()};
            Data Cache {Data()};

            Frenchie::Core::Optional<int> NextRenderingOrder;

        //private:
            std::string Name  = "UINode";
            std::string Hash  = "###UINode";
            int         Count = 0;

        private:
            bool Active{true};

            int RenderingOrder              {ImmedidateUserInterfaceRenderingOrder_::ImmedidateUserInterfaceRenderingOrder_Main};    // index of the node while rendering
        };

        // This class plays role of UI nodes hierarchy tree.
        // It's built and sorted once and is used for events processing, layoputing e.t.c
        struct ImmedidateUserInterfaceHierarchy
        {
            ImmedidateUserInterfaceHierarchy(const std::function<ImmediateUserInterfaceNode*(const ImmediateUserInterfaceNode*)> _GetParent =
                [](const ImmediateUserInterfaceNode* _Node)->ImmediateUserInterfaceNode*
                {
                    return _Node != nullptr ? _Node->State.Parent : nullptr;
                }) : GetParent(_GetParent){}

            ~ImmedidateUserInterfaceHierarchy(){}

            mutable std::vector<int>                                                              Indexes;
            mutable std::vector<int>                                                              Entries;
            mutable std::vector<ImmediateUserInterfaceNode*>                                      Singletons;
            mutable std::vector<ImmediateUserInterfaceNode*>                                      Sorted;
            mutable std::function<ImmediateUserInterfaceNode*(const ImmediateUserInterfaceNode*)> GetParent;

            std::vector<ImmediateUserInterfaceNode*>::iterator begin(const ImmediateUserInterfaceNode* _Node) const
            {
                if( _Node == nullptr                                            ||
                    _Node->State.RenderingIndex          >= (int)Indexes.size() ||
                    Indexes[_Node->State.RenderingIndex] >= (int)Sorted.size())
                {
                    return Sorted.end();
                }

                return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[_Node->State.RenderingIndex];
            }

            std::vector<ImmediateUserInterfaceNode*>::iterator end(const ImmediateUserInterfaceNode* _Node) const
            {
                if(_Node == nullptr                                                 ||
                    _Node->State.RenderingIndex + 1          >= (int)Indexes.size() ||
                    Indexes[_Node->State.RenderingIndex + 1] >= (int)Sorted.size())
                {
                    return Sorted.end();
                }

                return Sorted.empty() ? Sorted.end() : Sorted.begin() + Indexes[_Node->State.RenderingIndex + 1];
            }

            int size(const ImmediateUserInterfaceNode* _Node) const
            {
                return (int)(end(_Node) - begin(_Node));
            }

            template<typename FrameProcessor>
            int count(const ImmediateUserInterfaceNode* _Node, const FrameProcessor& _Filter) const
            {
                int counter = 0;

                for(auto it = begin(_Node); it != end(_Node); it++)
                {
                    if(_Filter(*it))
                        counter++;
                }

                return counter;
            }

            void build(const std::vector<ImmediateUserInterfaceNode*>& _Nodes)
            {
                std::vector<int> workspace(_Nodes.size()+1);

                Indexes.resize(_Nodes.size() + 1);
                Entries.resize(_Nodes.size());
                Sorted.resize(_Nodes.size());
                Singletons.clear();

                for(int i = 0; i < (int)Entries.size(); i++)
                {
                    Entries[i] = 0;
                    Indexes[i] = 0;
                    Sorted [i] = nullptr;

                    if(get_parent(_Nodes[i]) == nullptr)
                        Singletons.push_back(_Nodes[i]);
                }

                // count items
                for (int i = 0; i < (int)_Nodes.size(); i++)
                {
                    if(get_parent(_Nodes[i]) == nullptr)
                        continue;

                    ++Entries[get_parent(_Nodes[i])->State.RenderingIndex];
                }

                // cumulative sum
                int sum = 0;
                for (int i = 0; i < _Nodes.size(); i++)
                {
                    Indexes  [i] = sum;
                    workspace[i] = sum;
                    sum += Entries[i];
                }
                Indexes[_Nodes.size()] = sum;

                bool allIsNull = true;

                for(int i = 0; i < _Nodes.size(); i++ )
                {
                    if(get_parent(_Nodes[i]) == nullptr)
                        continue;

                    Sorted[workspace[get_parent(_Nodes[i])->State.RenderingIndex]++] = _Nodes[i];
                    allIsNull = false;
                }

                if(allIsNull) Sorted.clear();
            }

        //private:

            template<typename Type = ImmediateUserInterfaceNode>
            Type* get_parent(const ImmediateUserInterfaceNode* _Node) const
            {
                auto parent = (GetParent != nullptr ? GetParent(_Node) : nullptr);

                while (parent)
                {
                    if(dynamic_cast<Type*>(parent))
                        return dynamic_cast<Type*>(parent);

                    parent = (GetParent != nullptr ? GetParent(parent) : nullptr);
                }
                

                return nullptr;
            }
        };

        // This class is used for UI state serialization and deserialization
        class ImmediateUserInterfaceContextConfiguration final
        {
        public:
            ImmediateUserInterfaceContextConfiguration();
            ~ImmediateUserInterfaceContextConfiguration();

            template<typename Type>
            Type get(const std::string& _Section, const std::string& _Name);

            template<typename Type>
            void set(const std::string& _Section, const std::string& _Name, const Type& _Value);

            bool contains(const std::string& _Section, const std::string& _Name) const;
            bool empty() const;
            void clear();

            bool read(const std::u32string& _Path);
            bool write(const std::u32string& _Path);

        private:

        std::map<
            std::string,     // section
            std::map<
                std::string, // key
                std::string  // value
                >
                > m_Configuration; 
        };

        // This is the base UI controller class.
        // UI context has controllers list that are processed every frame
        class ImmediateUserInterfaceContextController
        {
        public:
            ImmediateUserInterfaceContextController(){}
            virtual ~ImmediateUserInterfaceContextController(){}

            virtual bool awake(ImmediateUserInterfaceContextLayer*){return true;}
            virtual void frame_start(ImmediateUserInterfaceContextLayer*){}
            virtual void frame_update(ImmediateUserInterfaceContextLayer*){}
            virtual void frame_input(ImmediateUserInterfaceContextLayer*){}
            virtual void frame_render(ImmediateUserInterfaceContextLayer*){}
            virtual void frame_finish(ImmediateUserInterfaceContextLayer*){}
        };

        struct ImmediateUserInterfaceVerticalClipper final
        {
            ImmediateUserInterfaceVerticalClipper(
                const ImmediateUserInterfaceNode* _ScorllArea    = nullptr,
                const int&                        _ElementsCount = 0,
                const float&                      _CellSize      = 0.f,
                const float&                      _Offset        = 0.f);

            int SourceElement = 0;
            int TargetElement = 0;
        };

        struct ImmediateUserInterfaceHorizontalClipper final
        {
            ImmediateUserInterfaceHorizontalClipper(
                const ImmediateUserInterfaceNode* _ScorllArea    = nullptr,
                const int&                        _ElementsCount = 0,
                const float&                      _CellSize      = 0.f,
                const float&                      _Offset        = 0.f);

            int SourceElement = 0;
            int TargetElement = 0;
        };

        /**
         * @brief This class plays role of UI context
         * @class ImmediateUserInterfaceContextLayer
         */
        class ImmediateUserInterfaceContextLayer : public Layer
        {
        public:
            ImmediateUserInterfaceContextLayer();
            virtual ~ImmediateUserInterfaceContextLayer();

            // Layer API
            virtual bool awake()        override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_input()  override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual bool allows_multiple_instances() const override;

            // UI scoped elements API

            /**
             * @brief This is the base function that creates UI nodes
             * @param _ID unique node ID
             * @param _Settings node settings
             * @param _Render boolean that defines if to push the node within rendering queue or not
             * @return returns true if node is successfully created and pushed within rendering queue
             */
            template<typename Type>
            bool begin_node(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings,
                bool*                                     _Render = nullptr)
            {
                // check if we need to render the node
                if(_Render != nullptr && !(*_Render))
                {
                    end_node<Type>();
                    return false;
                }

                // create node (output is never nullptr)
                ImmediateUserInterfaceNode* node = create_node<Type>(_ID);
                setup_created_node(node, _Settings);
                m_NodesRenderingList.push_back(node);
                m_NodesRenderingStack.push_back(node);

                // check node activity
                if(!node->is_enabled(this) || !node->create_contents(this, _ID, _Settings, _Render))
                {
                    end_node<Type>();
                    return false;
                }

                return true;
            }

            /**
             * @brief This is the base function that ends the scope of UI node
             */
            template<typename Type>
            void end_node()
            {
                restore_created_node();

                // clean up rendering stack and filled rendered nodes stack
                if(m_NodesRenderingStack.empty())
                    return;

                ImmediateUserInterfaceNode* node =
                    m_NodesRenderingStack[m_NodesRenderingStack.size() - 1];

                // as the node can contain nested items and store pointers to them
                // we need to load state when the node finishes it's hierarchy
                if(!m_IniFileState.empty())
                    node->load_state(this);

                GS_ASSERT((dynamic_cast<Type*>(node) != nullptr));

                m_NodesRenderedStack.push_back(node);
                m_NodesRenderingStack.pop_back();
            }

            /**
             * @brief This function creates window
             * @param _ID unique ID
             * @param _Settings settings
             * @param _Opened if this pointer is passed the close button is activated and window becomes closable
             * @return returns true if the window is opened
             */
            bool begin_window(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
                bool*                                     _Opened   = nullptr);

            /**
             * @brief This function ends the scope of a window
             */
            void end_window();

            /**
             * @brief This function creates dialog window
             * 
             * @param _ID unique ID
             * @param _Settings settings
             * @param _Opened if this pointer is passed the close button is activated and window becomes closable
             * @return returns true if dialog is opened
             */
            bool begin_dialog(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
                bool*                                     _Opened   = nullptr);

            /**
             * @brief This function ends dialog window scope
             */
            void end_dialog();

            /**
             * @brief This function creates scrollable area that places it's contents one by one vertically unless
             * same_line(), next_line() or indent() functions are called (see appropriate functions descriptions)
             * @param _ID unique ID
             * @param _Settings settings
             * @return returns true if scrollarea successfully created and added to rendering queue. 
             */
            bool begin_scrollarea(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            
            /**
             * @brief This function ends scrollarea scope
             */
            void end_scrollarea();

            /**
             * @brief This function creates UI panel that stretches it's content to fill all available space
             * @param _ID unique ID
             * @param _Settings settings
             * @return returns true if panel is successfully created and added to rendering queue.
             */
            bool begin_panel(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            
            /**
             * @brief This function ends panel scope
             */
            void end_panel();

            /**
             * @brief This function creates layout box that places it's elements one by one vertically. The width of all elements equals to vertical stack width.
             * The height of each element is calculated as a fill rate height relative to the all elements heights summ.
             * @param _ID unique ID
             * @param _Settings settings
             * @return returns true if vertical stack is successfully created and added to rendering queue.
             */
            bool begin_vertical_stack(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            
            /**
             * @brief This function ends vertical stack scope
             */
            void end_vertical_stack();

            /**
             * @brief This function creates layout box that places it's elements one by one horizontally. The height of all elements equals to horizontal stack height.
             * The width of each element is calculated as a fill rate height relative to the all elements widths summ.
             * @param _ID unique ID
             * @param _Settings settings
             * @return returns true if horizontal stack is successfully created and added to rendering queue.
             */
            bool begin_horizontal_stack(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            
            /**
             * @brief This function ends horizontal stack scope
             */
            void end_horizontal_stack();

            /**
             * @brief This function creates layout box that places it's elements in grid.
             * @param _ID unique ID
             * @param _Settings settings
             * @return returns true if grid successfully created and added to rendering queue.
             */
            bool begin_grid(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            
            /**
             * @brief This function ends grid scope
             */
            void end_grid();

            /**
             * @brief This function creates layout box that plays the role of a grid place.
             * @param _Row grid place row index
             * @param _Column grid place column index
             * @param _Settings settings
             * @return returns true if grid place successfully created and added to rendering queue.
             */
            bool begin_grid_place(
                const int&                                _Row,
                const int&                                _Column,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            
            /**
             * @brief This function ends grid place scope
             */
            void end_grid_place();

            /**
             * @brief This fuction creates popup menu that can be called by a right mouse click and that can play role of window menu
             * @param _ID unique ID
             * @return returns true if popup menu is successfully created and added to rendering queue.
             */
            bool begin_menu(const std::string& _ID);

            /**
             * @brief This function ends popup menu scope
             */
            void end_menu();

            /**
             * @brief This fuction creates window menubar.
             * @param _ID unique ID
             * @return returns true if menubar is successfully created and added to rendering queue.
             */
            bool begin_menubar(const std::string& _ID);
            
            /**
             * @brief This function ends menubar scope
             */
            void end_menubar();

            /**
             * @brief This function creates combobox widget.
             * @param _ID unique ID
             * @param _Preview preview text of combobox widget
             * @return returns true if combobox is successfully created and added to rendering queue.
             */
            bool begin_combobox(const std::string& _ID, const std::string& _Preview = "None");

            /**
             * @brief This function ends combobox scope
             */
            void end_combobox();

            /**
             * @brief This function creates 'what is it' popup
             * 
             * @param _ID unique ID
             * @param _Node node for which you want to generate 'what is it' popup
             * @return returns true if 'what is it' popup is successfully created and added to rendering queue. 
             */
            bool begin_what_is_it(const std::string& _ID, const ImmediateUserInterfaceNode* _Node);

            /**
             * @brief This function ends 'what is it' popup scope
             */
            void end_what_is_it();

            /**
             * @brief This function creates tree node. Tree node supports custom opened/closed state textures.
             * When no custom opened/closed state textures provided simple triangle is rendered.
             * You also can optionally render connection lines between tree nodes.
             * 
             * @param _ID unique ID
             * @param _Settings settings
             * @param _TextureOpened texture displayed on the left side of a tree node when it's opened
             * @param _TextureClosed texture displayed on the left side of a tree node when it's closed
             * @return returns true if tree node is opened. 
             */
            bool begin_tree_node(
                const std::string&                            _ID,
                const ImmediateUserInterfaceTreeNodeSettings& _Settings      = ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_Defaults,
                const ApplicationRenderingBackendTexture&     _TextureOpened = ApplicationRenderingBackendTexture(),
                const ApplicationRenderingBackendTexture&     _TextureClosed = ApplicationRenderingBackendTexture());
            
            /**
             * @brief This function ends tree node scope
             */
            void end_tree_node();
            
            /**
             * @brief  This function creates table
             * @param _ID unique ID
             * @param _RowsCount number of rows
             * @param _ColumnsCount number of columns
             * @param _CellSize size of table cell
             * @return returns true if table is successfully created and added to rendering queue.
             * @details If you do not pass '_RowsCount' and '_ColumnsCount' they will be computed dynamically by the cells indexes.
             * If you pass '_RowsCount' and '_ColumnsCount' you will be able to extract and use clipper (see current_table_grid_clipper())
             * that clips all invisible content, so you can render very large tables.
             */
            bool begin_table(
                const std::string& _ID,
                const int&         _RowsCount    = 0,
                const int&         _ColumnsCount = 0,
                const gs_vec2f&    _CellSize     = gs_vec2f(256.f, 128.f));

            /**
             * @brief This function ends table node scope
             */
            void end_table();

            /**
             * @brief This function creates table column header cell
             * @param _Index index of the title column
             * @param _Settings cell settings
             * @return returns true if  column header cell is successfully created and added to rendering queue.
             * @details Column header cells can be used only within table. Function asserts if you try to use horizontal title cell outside table.
             */
            bool begin_table_column_header(
                const int&                                _Index,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);

            /**
             * @brief This function ends column header cell scope
             */
            void end_table_column_header();

            /**
             * @brief This function creates table row header cell
             * @param _Index index of table row
             * @param _Settings cell settings
             * @return returns true if row header cell is successfully created and added to rendering queue.
             * @details Row title cells can be used only within table. Function asserts if you try to use horizontal title cell outside table.
             */
            bool begin_table_row_header(
                const int&                                _Index,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);

            /**
             * @brief This function ends row header cell scope
             */
            void end_table_row_header();

            /**
             * @brief This function creates corner header cell
             * @param _Settings cell settings
             * @return returns true if corner header cell is successfully created and added to rendering queue. 
             * @details Corner title cells can be used only within table. Function asserts if you try to use horizontal title cell outside table.
             */
            bool begin_table_corner_header(const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            
            /**
             * @brief This function ends corner header cell scope
             */
            void end_table_corner_header();

            /**
             * @brief This function renders cell within table
             * @param _Row cell row number
             * @param _Column cell column number
             * @param _Settings cell settings
             * @return returns true if data cell is successfully created and added to rendering queue. 
             * @details Cells are essentially the panels, so you can add any content you want to cells.
             * Cells can be added only to table. If you try to use cell outside table this function asserts.
             */
            bool begin_table_data_cell(const int& _Row, const int& _Column, const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);
            
            /**
             * @brief This function ends table data cell scope
             */
            void end_table_data_cell();

            /**
             * @brief This function renders 2D plots container widget
             * @param _ID unique ID
             * @param _Settings plot widget settings
             * @return returns true if 2D plots container widget is successfully created and added to rendering queue. 
             */
            bool begin_plot(const std::string& _ID, const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults);

            /**
             * @brief This function ends 2D plots container widget scope
             */
            void end_plot();

            // UI widgets API
            /**
             * @brief This function creates empty placeholder node
             * @param _ID unique ID
             * @param _Settings settings
             * @param _Color fill color of an empty node
             */
            void empty_node(
                const std::string&                        _ID,
                const ImmediateUserInterfaceNodeSettings& _Settings = ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
                const gs_color&                           _Color    = gs_color_rgba(255, 255, 255, 0));

            /**
             * @brief This function creates a simple push button widget
             * @param _ID unique ID
             * @return returns true if button is clicked 
             */
            bool push_button(const std::string& _ID);

            /**
             * @brief This function creates image button
             * @param _ID unique ID
             * @param _Color mask color
             * @param _Texture texture
             * @return true 
             * @return false 
             */
            bool image_button(
                const std::string&                        _ID,
                const gs_color&                           _Color,
                const ApplicationRenderingBackendTexture& _Texture = ApplicationRenderingBackendTexture());

            /**
             * @brief This function creates checkbutton that can be redered as checkbox, radiobutton or slider button depending on the settings.
             * @param _ID unique ID
             * @param _Checked input boolean that defines checkbutton state, checkbutton may or may not change this variable depending on settings
             * @param _Settings checkbutton settings
             * @return retruns true if checkbutton is checked
             */
            bool check_button(
                const std::string&                               _ID,
                bool&                                            _Checked,
                const ImmediateUserInterfaceCheckButtonSettings& _Settings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Defaults);

            // This function creates menu action button
            // _ID - unique ID

            /**
             * @brief This function creates menu action button
             * @param _ID unique ID
             * @return returns true if is clicked
             */
            bool menu_action(const std::string& _ID);

            /**
             * @brief This function creates combobox action button
             * @param _ID unique ID
             * @return returns true if is clicked 
             */
            bool combobox_item(const std::string& _ID);

            /**
             * @brief This function creates a simple textual label
             * @param _ID unique ID
             * @param _Text text string to display
             * @param _Settings label settings
             * @param _MaxSymbolsCount maximum symbols count of label
             */
            void label(
                const std::string&                         _ID,
                const std::string&                         _Text,
                const ImmediateUserInterfaceLabelSettings& _Settings        = ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_None,
                const int&                                 _MaxSymbolsCount = gs_huge<int>());

            /**
             * @brief This function creates editable multiline text widget
             * 
             * @param _ID unique ID
             * @param _Text text string to display and edit
             * @param _Settings settings
             * @param _InputTextFilter input text filtering function
             * @return returns true if text is edited or changed depending on settings
             */
            bool input_string_multiline(
                const std::string&                               _ID,
                std::string&                                     _Text,
                const ImmediateUserInterfaceInputStringSettings& _Settings                             = ImmediateUserInterfaceInputStringSettings_StopEditOnEscape,
                bool                                           (*_InputTextFilter)(const std::string&) = nullptr);

            /**
             * @brief This function creates editable singleline text widget
             * 
             * @param _ID unique ID
             * @param _Text text string to display and edit
             * @param _Settings settings
             * @param _InputTextFilter input text filtering function
             * @return returns true if text is edited or changed depending on settings
             */
            bool input_string_singleline(
                const std::string&                               _ID,
                std::string&                                     _Text,
                const ImmediateUserInterfaceInputStringSettings& _Settings                             = ImmediateUserInterfaceInputStringSettings_StopEditOnEscape | ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter,
                bool                                           (*_InputTextFilter)(const std::string&) = nullptr);

            /**
             * @brief This function creates widget aimed at scalar values editing
             * @param _ID unique ID
             * @param _Input input scalar value
             * @param _Min input scalar value minimum value
             * @param _Max input scalar value maximum value
             * @param _Settings settings
             * @return returns true if value is edited or changed depending on settings 
             */
            template<typename Type>
            bool input_scalar(
                const std::string&                               _ID,
                Type&                                            _Input,
                const Type&                                      _Min      = gs_tiny<Type>(),
                const Type&                                      _Max      = gs_huge<Type>(),
                const ImmediateUserInterfaceInputScalarSettings& _Settings = ImmediateUserInterfaceInputScalarSettings_Defaults);

            /**
             * @brief This function creates widget aimed at scalar values editing
             * @param _ID unique ID
             * @param _Input input scalar value
             * @param _Min input scalar minimum value
             * @param _Max input scalar maximum value
             * @param _Delta slider minimum delta in % of [_Min; _Max] range
             * @param _Settings settings
             * @return returns true if value is edited or changed depending on settings 
             */
            template<typename Type>
            bool input_scalar_slider(
                const std::string&                               _ID,
                Type&                                            _Input,
                const Type&                                      _Min      = gs_tiny<Type>(),
                const Type&                                      _Max      = gs_huge<Type>(),
                const int&                                       _Delta    = 1,
                const ImmediateUserInterfaceInputScalarSettings& _Settings = ImmediateUserInterfaceInputScalarSettings_Defaults);

            /**
             * @brief This function creates progress bar widget
             * @param _ID unique ID
             * @param _Input input scalar value
             * @param _Min input minimum value
             * @param _Max input maximum value
             */
            template<typename Type>
            void progressbar_default(const std::string& _ID, Type& _Input, const Type& _Min, const Type& _Max);

            /**
             * @brief This function creates circular progress bar widget
             * @param _ID unique ID
             * @param _Input input scalar value
             * @param _Min input minimum value
             * @param _Max input maximum value
             */
            template<typename Type>
            void progressbar_circular(const std::string& _ID, Type& _Input, const Type& _Min, const Type& _Max);

            /**
             * @brief This function creates widget for color eiditing
             * 
             * @param _ID unique ID
             * @param _Color input color
             * @param _Settings settings
             * @return returns true if color button of editor is clicked
             */
            bool input_color(
                const std::string&                               _ID,
                gs_color&                                        _Color,
                const ImmediateUserInterfaceColorPickerSettings& _Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults);

            /**
             * @brief This function creates classic RGBA color picker widget
             * @param _ID unique ID
             * @param _Color input color
             * @param _Settings settings
             */
            void color_picker_rgba(
                const std::string&                               _ID,
                gs_color&                                        _Color,
                const ImmediateUserInterfaceColorPickerSettings& _Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults);

            /**
             * @brief This function creates classic HSVA color picker widget
             * @param _ID unique ID
             * @param _Color input color
             * @param _Settings settings
             */
            void color_picker_hsva(
                const std::string&                               _ID,
                gs_color&                                        _Color,
                const ImmediateUserInterfaceColorPickerSettings& _Settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults);

            /**
             * @brief This function creates image display widget
             * @param _ID unique ID
             * @param _Color mask color
             * @param _Texture texture
             */
            void image(
                const std::string&                        _ID,
                const gs_color&                           _Color,
                const ApplicationRenderingBackendTexture& _Texture = ApplicationRenderingBackendTexture());

            /**
             * @brief Adds X axis onto 2D plots widget
             * @param _ID unique ID
             * @param _Min minimum axis value
             * @param _Max maximum axis value 
             * @param _TicksCount axis ticks count
             * @details next created plot (line, stem, e.t.c) is going to be attached to this X axis. Axis can only be created within plots container widget.
             * If you try to create axis outside of plots container widget the function asserts.
             */
            void plot_axis_x(const std::string& _ID, const float& _Min, const float& _Max, const int& _TicksCount = 10);

            /**
             * @brief Adds Y axis onto 2D plots widget
             * @param _ID unique ID
             * @param _Min minimum axis value
             * @param _Max maximum axis value 
             * @param _TicksCount axis ticks count
             * @details next created plot (line, stem, e.t.c) is going to be attached to this Y axis. Axis can only be created within plots container widget.
             * If you try to create axis outside of plots container widget the function asserts.
             */
            void plot_axis_y(const std::string& _ID, const float& _Min, const float& _Max, const int& _TicksCount = 10);

            /**
             * @brief Creates XY line plot
             * @param _ID unique ID
             * @param _X x axis values
             * @param _Y y axis values
             * @param _N x, y axis values arrays size
             * @param _Color color of a curve
             * @param _Width line curve width
             * @param _Settings plot settings
             * @param _Range range of x, y values [Xmin, Ymin, Xmax, Ymax]
             * @details This primitive can only be created within plots container widget. Also, the plot MUST BE attached to X, Y axis
             * that are created by plot_axis_x(...) and plot_axis_y(...). If you try to create the plot outside of plots container widget
             * or you don't attach the plot to x, y axis the function asserts.
             */
            Frenchie::Core::Optional<gs_vec4f> plot_line(
                const std::string&                            _ID,
                const float                                   _X[],
                const float                                   _Y[],
                const int&                                    _N,
                const gs_color&                               _Color,
                const float&                                  _Width,
                const ImmediateUserInterfacePlotLineSettings& _Settings = ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_Defaults,
                const Frenchie::Core::Optional<gs_vec4f>&     _Range    = Frenchie::Core::Optional<gs_vec4f>());


            /**
             * @brief Creates pie chart
             * @param _Names names of pie chart sectors
             * @param _Values values of pie chart sectors
             * @param _Colors colors of pie chart sectors
             * @param _Count number of pie chart sectors
             * @details This primitive can only be created within plots container widget created by begin_plot(...).
             * If you try to create the pie chart outside of plots container widget the function asserts.
             */
            void plot_pie(
                const std::string _Names [],
                const float       _Values[],
                const gs_color    _Colors[],
                const int&        _Count);

            /**
             * @brief Returns text line height considering frames width, radius and font size
             * @return returns text line height considering frames width, radius and font size 
             */
            float get_text_line_height();

            /**
             * @brief Returns default content margin for scroll area considering frames radius and width
             * @return returns default content margin for scroll area considering frames radius and width
             */
            gs_vec4f get_content_default_margin();

            // next node API

            /**
             * @brief This function generates a unique ID for UI element. It constructs ID as {Name}###{Hash} where {Name} is what is going to be diplayed and {Hash} is what is going to be used as a widget unique ID.
             * If no Hash passed to a function it generates unique ID as {Name}
             * @param _Name UI element name
             * @param _Hash UI element hash
             * @return returns widget ID
             */
            std::string next_id(const std::string& _Name, const std::string& _Hash = std::string());

            /**
             * @brief This function sets next node rendering order. The value is set every frame
             * @param _Order rendering order of the next node
             */
            void next_rendering_order(const ImmedidateUserInterfaceRenderingOrder& _Order);

            /**
             * @brief This function sets next plot axis scale. The value is set every frame
             * @param _Value next plot axis scale
             */
            void next_axis_scale(const gs_vec2f& _Value);

            /**
             * @brief This function sets next plot axis offset. The value is set every frame
             * @param _Value next plot axis offset
             */
            void next_axis_offset(const gs_vec2f& _Value);

            /**
             * @brief This function pushes the node onto a next line if it's within scollarea.
             * All nodes are actually pushed onto next line by default but if you call this function several times
             * it adds vertical indent equal to N * maxHeight where N is the number of calls of next_line()
             * and maxHeight is maximum height within current scroll area row.
             */
            void next_line();

            /**
             * @brief This function pushes the node onto a same line with it's neighbours if it's within scollarea
             */
            void same_line();

            /**
             * @brief This function adds horizontal indent between nodes within scroll area.
             * @param _Value horizontal indent value
             */
            void indent(const float& _Value = 32.f);

            /**
             * @brief This function sets the width of the next created node. The value is set every frame.
             * @param _Value next node width
             */
            void next_width(const float& _Value);

            /**
             * @brief This function sets the height of the next created node. The value is set every frame.
             * @param _Value next node height
             */
            void next_height(const float& _Value);

            /**
             * @brief This function sets the size of the next created node. The value is set every frame.
             * @param _Value next node size
             */
            void next_size(const gs_vec2f& _Value);

            /**
             * @brief This function sets the position of the next created node. The value is set every frame.
             * @param _Value next node size
             */
            void next_position(const gs_vec2f& _Value);

            /**
             * @brief This function sets the minimum width of the next created node. The value is set every frame.
             * @param _Value next node minimum width
             */
            void next_minimum_width(const float& _Value);

            /**
             * @brief This function sets the minimum height of the next created node. The value is set every frame.
             * @param _Value next node minimum height
             */
            void next_minimum_height(const float& _Value);

            /**
             * @brief This function sets the minimum size of the next created node. The value is set every frame.
             * @param _Value next node minimum size
             */
            void next_minimum_size(const gs_vec2f& _Value);

            /**
             * @brief This function sets the maximum width of the next created node. The value is set every frame.
             * @param _Value next node maximum width
             */
            void next_maximum_width(const float& _Value);

            /**
             * @brief This function sets the maximum height of the next created node. The value is set every frame.
             * @param _Value next node maximum height
             */
            void next_maximum_height(const float& _Value);

            /**
             * @brief This function sets the maximum size of the next created node. The value is set every frame.
             * @param _Value next node maximum size
             */
            void next_maximum_size(const gs_vec2f& _Value);

            /**
             * @brief This function sets content margin of the next created node. The value is set every frame.
             * @param _Value content margin values {top, left, right, bottom}
             */
            void next_content_margin(const gs_vec4f& _Value);

            /**
             * @brief This function sets content pading of the next created node. The value is set every frame.
             * @param _Value content padding values {top, left, right, bottom}
             */
            void next_content_padding(const gs_vec4f& _Value);

            /**
             * @brief This function sets scroll offset of the next created scrollarea. The value is set every frame.
             * @param _Value scroll offset {horizontal, vertical}
             */
            void next_scroll_offset(const gs_vec2f& _Value);

            /**
             * @brief This function makes the next create node to sort it's children in follow. The value is set every frame.
             */
            void next_order_in_follow();

            /**
             * @brief This function makes the next create node to sort it's children in parallel. The value is set every frame.
             */
            void next_order_in_parallel();

            // current node API

            /**
             * @brief This function returns node bounding box
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @return returns node bounding box
             */
            gs_2dboxf current_bounding_box(const ImmediateUserInterfaceNode* _Node) const;

            /**
             * @brief This function returns current node maximum size
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @return returns current node maximum size
             */
            gs_vec2f  current_maximum_size(const ImmediateUserInterfaceNode* _Node) const;

            /**
             * @brief This function returns current node minimum size
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @return returns current node minimum size 
             */
            gs_vec2f  current_minimum_size(const ImmediateUserInterfaceNode* _Node) const;

            /**
             * @brief This function returns current scrollarea scrollbar offset
             * @param _Node if true returns content relative scroll offset, if false returns scroll area size relative offset
             * @param _Scaled node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions (if it's not scrollarea then zero vector is returned)
             * @return returns current scrollarea scrollbar offset
             */
            gs_vec2f  current_scroll_offset(const ImmediateUserInterfaceNode* _Node, const bool& _Scaled = true) const;

            /**
             * @brief This function returns current vertical clipper if it exists
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @return returns current vertical clipper if it exists
             */
            ImmediateUserInterfaceVerticalClipper current_vertical_clipper(const ImmediateUserInterfaceNode* _Node) const;

            /**
             * @brief This function returns current horizontal clipper if it exists
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @return returns current horizontal clipper if it exists
             */
            ImmediateUserInterfaceHorizontalClipper current_horizontal_clipper(const ImmediateUserInterfaceNode* _Node) const;

            /**
             * @brief This function shows if currently rendered node is being hovered by a mouse cursor
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @return returns true if _Node is hovered by mouse cursor
             */
            bool is_current_node_mouse_hovered(const ImmediateUserInterfaceNode* _Node) const;

            /**
             * @brief This function shows if mouse button is being down over currently rendered node
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @param _Button mouse button
             * @return returns true if mouse button is being down over currently rendered node
             */
            bool is_current_node_mouse_down(const ImmediateUserInterfaceNode* _Node, const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief This function shows if mouse button is being pressed over currently rendered node
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @param _Button mouse button
             * @return returns true if mouse button is being pressed over currently rendered node
             */
            bool is_current_node_mouse_pressed(const ImmediateUserInterfaceNode* _Node, const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief This function shows if mouse button is being released over currently rendered node
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @param _Button mouse button
             * @return returns true if mouse button is being released over currently rendered node
             */
            bool is_current_node_mouse_released(const ImmediateUserInterfaceNode* _Node, const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief This function shows if mouse button is being clicked over currently rendered node
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @param _Button mouse button
             * @return returns true if mouse button is being clicked over currently rendered node
             */
            bool is_current_node_mouse_clicked(const ImmediateUserInterfaceNode* _Node, const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief This function shows if mouse button is being double clicked over currently rendered node
             * @param _Node node retrieved from rendering queue by get_rendering_stack_top() or get_rendered_stack_top() functions
             * @param _Button mouse button
             * @return returns true if mouse button is being double clicked over currently rendered node
             */
            bool is_current_node_mouse_double_clicked(const ImmediateUserInterfaceNode* _Node, const ApplicationPlatformBackendMouseButton::Button& _Button) const;

            /**
             * @brief This function shows that geometry has not been computed yet
             * @return returns true if geometry has already been computed 
             */
            bool dirty_geomery() const;

            /**
             * @brief This function returns controller of a type 'Type'
             * @tparam Type controller type
             * @return returns controller of a type 'Type'
             */
            template<typename Type> Type* get_controller() const
            {
                for(auto& controller : m_Controllers)
                {
                    if(dynamic_cast<Type*>(controller.get()))
                        return dynamic_cast<Type*>(controller.get());
                }

                return nullptr;
            }

            /**
             * @brief This function retrieves the node from the top of currently rendered nodes stack and tries to cast it to a type 'Type'
             * @tparam Type node type
             * @return returns node of type 'Type'
             */
            template<typename Type = ImmediateUserInterfaceNode>
            Type* get_rendering_stack_top() const
            {
                return !m_NodesRenderingStack.empty() ? dynamic_cast<Type*>(m_NodesRenderingStack[m_NodesRenderingStack.size() - 1]) : nullptr;
            }

            /**
             * @brief This function retrieves the node from the top of rendered nodes stack and tries to cast it to a type 'Type'
             * @tparam Type node type
             * @return returns node of type 'Type'
             */
            template<typename Type = ImmediateUserInterfaceNode>
            Type* get_rendered_stack_top() const
            {
                return !m_NodesRenderedStack.empty() ? dynamic_cast<Type*>(m_NodesRenderedStack[m_NodesRenderedStack.size() - 1]) : nullptr;
            }

            // info

            // hierarchy and cache
            mutable std::map<std::string, std::unique_ptr<ImmediateUserInterfaceNode>> m_Cache;
            mutable ImmedidateUserInterfaceHierarchy                                   m_Hierarchy;

            // rendering
            mutable std::shared_ptr<RenderingQueue2D>                                  m_Renderer{nullptr};
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingList;
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderingStack;
            mutable std::vector<ImmediateUserInterfaceNode*>                           m_NodesRenderedStack;

            // style
            mutable ImmedidateUserInterfaceStyle                                       m_Style;

            // ini file
            ImmediateUserInterfaceContextConfiguration                                 m_IniFileState;

            // input
            ImmedidateUserInterfaceInput                                               m_Input;

            // settings
            ImmediateUserInterfaceContextSettings                                      m_Settings =
                  ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWorkspaceDocking
                | ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_EnableWindowsDocking
                | ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_HighlighHoveredNodes
                | ImmediateUserInterfaceContextSettings_::ImmediateUserInterfaceContextSettings_SaveStyleSettingsToIniFile;

        private:

            // info
            std::vector<std::unique_ptr<ImmediateUserInterfaceContextController>> m_Controllers;
            std::string                                                           m_CurrentHash;
            std::string                                                           m_CurrentName;
            std::u32string                                                        m_IniFilePath = U"Frenchie.ini";

            // This function creates the node of a type 'Type' and saves it into cache.
            // If the node has already been created earlier the function retrieves if from cache.
            // A unique _ID is used as key by which the function looks for the node within cache.
            // A unique _ID can contain a unique hashable part and changable naming part. 
            // Both hashable and naming parts are separated by sequence '###' as follows {Name}###Hash
            // _ID - the unique ID of the node
            template<typename Type>
            Type* create_node(const std::string& _ID)
            {
                // clean-up hash and name buffers
                m_CurrentHash.clear();
                m_CurrentName.clear();

                // determine hashable part of input id
                int hashable   = 0;
                int sharpCount = 0;

                for (;hashable < (int)_ID.size(); hashable++)
                {
                    if(_ID[hashable] == '#')
                    {
                        sharpCount = 1;
                        if(hashable + 1 < (int)_ID.size() && _ID[hashable + 1] == '#') ++sharpCount;
                        if(hashable + 2 < (int)_ID.size() && _ID[hashable + 2] == '#') ++sharpCount;
                        
                        if(sharpCount >= 3)
                            break;
                    }
                }
                
                // create node
                m_CurrentHash.append(
                    ((hashable + sharpCount) < _ID.size() ? _ID.c_str() + (hashable + sharpCount) : _ID.c_str()),
                    ((hashable + sharpCount) < _ID.size() ? _ID.size()  - (hashable + sharpCount) : _ID.size()));

                if(m_Cache.find(m_CurrentHash) == m_Cache.end())
                    m_Cache[m_CurrentHash] = std::make_unique<Type>(m_CurrentHash);

                ImmediateUserInterfaceNode* node = m_Cache[m_CurrentHash].get();
                GS_ASSERT((++node->Count) <= 1);

                // setup node name
                m_CurrentName.append(_ID.c_str(), _ID.c_str() + hashable);
                if(node->Name != m_CurrentName)
                    node->Name = m_CurrentName;

                return dynamic_cast<Type*>(node);
            }

            void setup_created_node(ImmediateUserInterfaceNode*, const ImmediateUserInterfaceNodeSettings&);
            void restore_created_node();
        };

        /*! @} */
    };
}