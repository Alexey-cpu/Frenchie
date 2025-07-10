#include <FrenchieApplicationTimerLayer.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// STL
#include <iostream>

using namespace Frenchie::Application;

TimeProviderLayer::TimeProviderLayer() : Layer(STRINGIFY(TimeProviderLayer)){}
TimeProviderLayer::~TimeProviderLayer(){}

double TimeProviderLayer::get_time_delta() const
{
    return m_CurrentFrame - m_PreviousFrame;
}

void TimeProviderLayer::frame_update()
{
    m_PreviousFrame = m_CurrentFrame;

    m_CurrentFrame = glfwGetTime();
}