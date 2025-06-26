#include <FrenchieApplication.hpp>
#include <FrenchieImGuiTestLayer.hpp>
#include <FrenchieRenderingTestLayer.hpp>     

#include <FrenchieImGuiOpenGLScene.hpp>

int main(int, char**)
{
    // setup logger
    Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::stdout_color_sink_mt>();

    // setup application
    auto application = Frenchie::Application::GLApplication::instance();
    application->set_window_size(glm::vec2(2048, 1024));
    application->set_maximized(true);

    // push application layers
    //application->push<Frenchie::Application::Test::RenderingTest>("RenderingTest");

    application->push<Frenchie::Application::Scene>("RenderingTest");

    return application->execute();
}