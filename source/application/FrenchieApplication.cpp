#include <FrenchieApplication.hpp>

// Core
#include <FrenchieCoreSingleton.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        class ApplicationInputHandler
        {
        public:

            // converters
            static unsigned int application_mouse_cursor_to_glfw_mouse_cursor(int _MouseCursor)
            {
                switch (_MouseCursor)
                {
                case ApplicationMouseCursor::Cursor::ApplicationMouseCursor_Arrow:
                    return GLFW_ARROW_CURSOR;
                case ApplicationMouseCursor::Cursor::ApplicationMouseCursor_TextInput:
                    return GLFW_IBEAM_CURSOR;
                case ApplicationMouseCursor::Cursor::ApplicationMouseCursor_Crosshair:
                    return GLFW_CROSSHAIR_CURSOR;
                case ApplicationMouseCursor::Cursor::ApplicationMouseCursor_PointingHand:
                    return GLFW_POINTING_HAND_CURSOR;
                case ApplicationMouseCursor::Cursor::ApplicationMouseCursor_HorizontalDoubleHeaded:
                    return GLFW_RESIZE_EW_CURSOR;
                case ApplicationMouseCursor::Cursor::ApplicationMouseCursor_VerticalDoubleHeaded:
                    return GLFW_RESIZE_NS_CURSOR;
                case ApplicationMouseCursor::Cursor::ApplicationMouseCursor_TopLeftToBottomRightHeaded:
                    return GLFW_RESIZE_NWSE_CURSOR;
                case ApplicationMouseCursor::Cursor::ApplicationMouseCursor_TopRighToBottomLeftHeaded:
                    return GLFW_RESIZE_NESW_CURSOR;
                }
                return GLFW_ARROW_CURSOR;
            }

            static ApplicationMouseButton::Button glfw_mouse_button_to_application_mouse_button(int _MouseButton)
            {
                switch (_MouseButton)
                {
                case GLFW_MOUSE_BUTTON_LEFT:
                    return ApplicationMouseButton::Button::ApplicationMouseButton_Left;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    return ApplicationMouseButton::Button::ApplicationMouseButton_Right;
                case GLFW_MOUSE_BUTTON_MIDDLE:
                    return ApplicationMouseButton::Button::ApplicationMouseButton_Middle;
                }

                return ApplicationMouseButton::Button::ApplicationMouseButton_End;
            }

            static bool glfw_boolean_to_application_boolean(int _Boolean)
            {
                return _Boolean == GLFW_TRUE;
            }

            // window callbacks
            static void glfw_on_window_resize_callback(GLFWwindow* _Window, int _Width, int _Height)
            {
                (void)_Window;
                glViewport(0, 0, _Width, _Height);
            }

            static void glfw_on_window_maximized_callback(GLFWwindow* _Window, int _Maximized)
            {
                (void)_Window;
                int width  = 0;
                int height = 0;
                glfwGetWindowSize(_Window, &width, &height);
                glViewport(0, 0, width, height);
            }

            static void glfw_oon_window_focused_callback(GLFWwindow* _Window, int _Focused)
            {
                application()->m_Input.Window.Focused =
                    glfw_boolean_to_application_boolean(_Focused);;
            }

            // cursor callbacks
            static void glfw_on_cursor_moved_callback(GLFWwindow* _Window, double _X, double _Y)
            {
                (void)_Window;
                if(application()->m_Input.MouseCursor.Entered)
                    application()->m_Input.MouseCursor.Position = gs_vec2f(_X, _Y);
            }

            static void glfw_on_cursor_enter_callback(GLFWwindow* _Window, int _Entered)
            {
                (void)_Window;
                application()->m_Input.MouseCursor.Entered =
                    glfw_boolean_to_application_boolean(_Entered);
            }

            // mouse callbacks
            static void glfw_on_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
            {
                (void)window;

                // trigger mouse press event
                application()->m_Input.MouseButtons[glfw_mouse_button_to_application_mouse_button(button)].Pressed  = action == GLFW_PRESS;

                // trigger mouse release event
                application()->m_Input.MouseButtons[glfw_mouse_button_to_application_mouse_button(button)].Released = action == GLFW_RELEASE;
            }
        };
    }
}

// translator functions
void OpenGLPlatformBackendMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}

ApplicationInstance::ApplicationInstance()
{
}

ApplicationInstance::~ApplicationInstance()
{
}

std::string ApplicationInstance::get_window_name() const
{
    return std::string(glfwGetWindowTitle(reinterpret_cast<GLFWwindow*>(m_Context)));
}

gs_vec2f ApplicationInstance::get_window_size() const
{
    int x = 0;
    int y = 0;
    glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(m_Context), &x, &y);
    return {x, y};
}

gs_vec2f ApplicationInstance::get_window_position() const
{
    // window
    int x = 0;
    int y = 0;
    glfwGetWindowPos(reinterpret_cast<GLFWwindow*>(m_Context), &x, &y);
    return {x, y};
}

gs_vec2f ApplicationInstance::get_window_cursor_position() const
{
    return m_Input.MouseCursor.Position;
}

gs_vec2f ApplicationInstance::get_window_cursor_dragdelta() const
{
    return m_Input.MouseCursor.DragDelta;
}

gs_vec2f ApplicationInstance::get_window_framebuffer_size() const
{
    // execute backend
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_Context), &display_w, &display_h);
    return gs_vec2f(display_w, display_h);
}

bool ApplicationInstance::is_mouse_button_down(const ApplicationMouseButton::Button& _Button) const
{
    return m_Input.MouseButtons[_Button].Down;
}

bool ApplicationInstance::is_mouse_button_hold(const ApplicationMouseButton::Button& _Button) const
{
    return m_Input.MouseButtons[_Button].Hold;
}

bool ApplicationInstance::is_mouse_button_pressed(const ApplicationMouseButton::Button& _Button) const
{
    return m_Input.MouseButtons[_Button].Pressed;
}

bool ApplicationInstance::is_mouse_button_released(const ApplicationMouseButton::Button& _Button) const
{
    return m_Input.MouseButtons[_Button].Released;
}

bool ApplicationInstance::is_mouse_button_clicked(const ApplicationMouseButton::Button& _Button) const
{
    return m_Input.MouseButtons[_Button].Clicked;
}

bool ApplicationInstance::is_mouse_button_double_clicked(const ApplicationMouseButton::Button& _Button) const
{
    return m_Input.MouseButtons[_Button].DoubleClicked;
}

void ApplicationInstance::set_window_name(const std::string& _Name)
{
    glfwSetWindowTitle(reinterpret_cast<GLFWwindow*>(m_Context), _Name.c_str());
}

bool ApplicationInstance::awake()
{
    if(m_Context != nullptr)
        return true;

    // initialization
    if(glfwInit() == GLFW_FALSE)
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_ICONIFIED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    // create context
    m_Context = glfwCreateWindow(512, 256, "Application", nullptr, nullptr);

    if(m_Context == nullptr)
    {
        glfwTerminate();
        return false;
    }

    // configure context
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(m_Context));
    glfwSetWindowSizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_window_resize_callback);
    glfwSetFramebufferSizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_window_resize_callback);
    glfwSetWindowMaximizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_window_maximized_callback);
    glfwSetWindowFocusCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_oon_window_focused_callback);
    glfwSetCursorEnterCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_cursor_enter_callback);
    glfwSetCursorPosCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_cursor_moved_callback);
    glfwSetMouseButtonCallback(reinterpret_cast<GLFWwindow*>(m_Context), ApplicationInputHandler::glfw_on_mouse_button_callback);
    // glfwSetScrollCallback(vd->Window, ImGui_ImplGlfw_ScrollCallback);
    // glfwSetKeyCallback(vd->Window, ImGui_ImplGlfw_KeyCallback);
    // glfwSetCharCallback(vd->Window, ImGui_ImplGlfw_CharCallback);
    // glfwSetWindowCloseCallback(vd->Window, ImGui_ImplGlfw_WindowCloseCallback);
    // glfwSetWindowPosCallback(vd->Window, ImGui_ImplGlfw_WindowPosCallback);
    // glfwSetWindowSizeCallback(vd->Window, ImGui_ImplGlfw_WindowSizeCallback);

    // load OpenGL interface using GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    // maximize context window
    glfwMaximizeWindow(reinterpret_cast<GLFWwindow*>(m_Context));

    // call window maximize callback if the Window has been maximized
    ApplicationInputHandler::glfw_on_window_maximized_callback(
        reinterpret_cast<GLFWwindow*>(m_Context),
        glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_Context), GLFW_MAXIMIZED));

    // create standart cursors
    for (int mouseCursor = ApplicationMouseCursor::ApplicationMouseCursor_Begin;
             mouseCursor < ApplicationMouseCursor::ApplicationMouseCursor_End;
             mouseCursor++)
    {
        m_Input.MouseCursor.Cursors[mouseCursor] =
            (uintptr_t)glfwCreateStandardCursor(ApplicationInputHandler::application_mouse_cursor_to_glfw_mouse_cursor(mouseCursor));
    }

    return true;
}

void ApplicationInstance::ApplicationInstance::frame_start()
{
    // awake layers
    for(auto it = m_Awakes.begin(); it != m_Awakes.end(); it++)
    {
        if((*it)->awake())
            m_Layers.push_back((*it));
    }
    
    m_Awakes.clear();

    // execute backend
    glfwPollEvents();
    glfwSwapInterval(1);

    // remove layers that are closed
    for(auto it = m_Layers.begin(); it != m_Layers.end(); it++)
    {
        if((*it)->is_closed())
        {
            (*it)->finish();
            auto rm = it;
            it++;
            m_Layers.erase(rm);

            if(it == m_Layers.end())
                break;
        }
    }

    // execute layers
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden()) 
            layer->frame_start();
    }

    // update application input input

    // mouse buttons
    for (int mouseButton = ApplicationMouseButton::Button::ApplicationMouseButton_Begin;
             mouseButton < ApplicationMouseButton::Button::ApplicationMouseButton_End;
             mouseButton++)
    {
        if(Frenchie::Core::elapsed<std::chrono::milliseconds>(m_Input.MouseButtons[mouseButton].ReleaseTime, std::chrono::high_resolution_clock::now()) > 200)
        {
            m_Input.MouseButtons[mouseButton].Clicks = 0;
        }

        if(m_Input.MouseButtons[mouseButton].Pressed)
        {
            m_Input.MouseButtons[mouseButton].Down      = true;
            m_Input.MouseButtons[mouseButton].PressTime = Frenchie::Core::tic();

            m_Input.MouseCursor.MousePressPosition = m_Input.MouseCursor.Position;
        }
        
        if(m_Input.MouseButtons[mouseButton].Down)
        {
            m_Input.MouseCursor.DragDelta =
                m_Input.MouseCursor.Position - m_Input.MouseCursor.MousePressPosition;

            m_Input.MouseButtons[mouseButton].Hold =
                Frenchie::Core::elapsed<std::chrono::milliseconds>(
                    m_Input.MouseButtons[mouseButton].PressTime,
                    Frenchie::Core::tic()) > 100;
        }

        if(m_Input.MouseButtons[mouseButton].Released)
        {
            m_Input.MouseButtons[mouseButton].Down        = false;
            m_Input.MouseButtons[mouseButton].Hold        = false;
            m_Input.MouseButtons[mouseButton].ReleaseTime = Frenchie::Core::tic();

            m_Input.MouseButtons[mouseButton].Clicked =
                Frenchie::Core::elapsed<std::chrono::milliseconds>(m_Input.MouseButtons[mouseButton].ReleaseTime, m_Input.MouseButtons[mouseButton].PressTime) < 500;

            m_Input.MouseButtons[mouseButton].DoubleClicked =
                m_Input.MouseButtons[mouseButton].Clicked && ++m_Input.MouseButtons[mouseButton].Clicks >= 2;
        }        
    }

    // cursor
    glfwSetCursor(reinterpret_cast<GLFWwindow*>(m_Context),
                  reinterpret_cast<GLFWcursor*>(m_Input.MouseCursor.Cursors[m_Input.MouseCursor.View]));

    glfwSetInputMode(reinterpret_cast<GLFWwindow*>(m_Context), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void ApplicationInstance::ApplicationInstance::frame_update()
{
    // execute backend
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_Context), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    // execute layers
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden())
            layer->frame_update();
    }
}

void ApplicationInstance::ApplicationInstance::frame_render()
{
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden())
            layer->frame_render();
    }
}

void ApplicationInstance::ApplicationInstance::frame_debug()
{
    for(auto layer : m_Layers) 
    {
        if(!layer->is_hidden())
            layer->frame_debug();
    }
}

void ApplicationInstance::ApplicationInstance::frame_finish()
{
    // swap buffers
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(m_Context));

    for(auto layer : m_Layers)
    {
        if(!layer->is_hidden())
            layer->frame_finish();
    }

    for (int mouseButton = ApplicationMouseButton::Button::ApplicationMouseButton_Begin;
             mouseButton < ApplicationMouseButton::Button::ApplicationMouseButton_End;
             mouseButton++)
    {
        m_Input.MouseButtons[mouseButton].Released      = false;
        m_Input.MouseButtons[mouseButton].Pressed       = false;
        m_Input.MouseButtons[mouseButton].Clicked       = false;
        m_Input.MouseButtons[mouseButton].DoubleClicked = false;
    }

    m_Input.MouseCursor.DragDelta = gs_vec2f(0.f, 0.f);
}

void ApplicationInstance::ApplicationInstance::finish()
{
    for(auto layer : m_Layers)
        layer->finish();
}

void ApplicationInstance::ApplicationInstance::quit()
{
    // deinitialize all application layers
    for(auto layer : m_Layers) 
    {
        layer->close();
        layer->quit();
    }

    // remove all application layers
    m_Layers.clear();

    // destroy context
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_Context));
    glfwTerminate();
    m_Context = nullptr;
}

bool ApplicationInstance::is_closed()
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context));
}

void ApplicationInstance::close()
{
    glfwSetWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context), GL_TRUE);
}

int ApplicationInstance::execute()
{
    if(!awake()) 
        return -1;

    while (!is_closed())
    {
        frame_start();
        frame_update();
        frame_debug();
        frame_render();
        frame_finish();
    }

    finish();
    quit();

    return 1;
}

ApplicationInstance::const_iterator ApplicationInstance::begin() const
{
    return m_Layers.begin();
}

ApplicationInstance::const_iterator ApplicationInstance::end() const
{
    return m_Layers.end();
}

size_t ApplicationInstance::size() const
{
    return m_Layers.size();
}

Frenchie::Application::ApplicationInstance* Frenchie::Application::application()
{
    return Frenchie::Core::Singleton<Frenchie::Application::ApplicationInstance>::instance();
}