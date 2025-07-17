#include <FrenchieApplicationCursorWatcherLayer.hpp>

#include <FrenchieApplication.hpp>

using namespace Frenchie::Application;

CursorWatcher::CursorWatcher() : Layer(STRINGIFY(CursorWatcher)){}

CursorWatcher::~CursorWatcher(){}

void CursorWatcher::frame_update()
{
    m_PositionDelta    = m_CurrentPosition - m_PreviousPosition;
    m_PreviousPosition = m_CurrentPosition;
}

void CursorWatcher::mouse_callback(GLFWwindow* _Window, double _X, double _Y)
{
    m_CurrentPosition  = glm::vec3(+_X, -_Y, 0.f);
}

void CursorWatcher::hide_cursor()
{
    glfwSetInputMode(Application::instance()->get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void CursorWatcher::show_cursor()
{
    glfwSetInputMode(Application::instance()->get_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}