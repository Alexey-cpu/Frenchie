#pragma once

#include <FrenchieImmediateUserInterfaceModelViewControllerLayer.hpp>

class FrenchieImmediateUserInterfaceCustomWidgetController : public Frenchie::Application::ImmediateUserInterfaceViewController
{
public:

    virtual ~FrenchieImmediateUserInterfaceCustomWidgetController(){}

    virtual bool setup(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override
    {
        _Model->request<std::function<void(Frenchie::Application::ImmediateUserInterfaceContextLayer*)>>("Directory") =
            [this](Frenchie::Application::ImmediateUserInterfaceContextLayer* _Context)
            {
                if(_Context->begin_scrollarea(_Context->next_id("ScrollArea")))
                {
                    render_directory(_Context, std::filesystem::current_path());
                    _Context->end_scrollarea();
                }
            };

        return true;
    }

    virtual void update(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override{}

    virtual void destroy(std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceViewModel>& _Model) override{}

    std::vector<float> m_XValues{std::vector<float>()};
    std::vector<float> m_YValues{std::vector<float>()};

    void render_directory(Frenchie::Application::ImmediateUserInterfaceContextLayer* _Context, const std::filesystem::path& _Path)
    {
        if (!std::filesystem::exists(_Path)) return;
        
        if(_Context->begin_tree_node(
            _Context->next_id(
                Frenchie::Core::String::convert_utf32_to_utf8(_Path.filename().u32string()),
                Frenchie::Core::String::convert_utf32_to_utf8(_Path.u32string())), 
            Frenchie::Application::ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_Defaults))
        {
            if(std::filesystem::is_directory(_Path))
            {
                for (const auto& entry : std::filesystem::directory_iterator(_Path, std::filesystem::directory_options::skip_permission_denied))
                    render_directory(_Context, entry.path());
            }

            _Context->end_tree_node();
        }
    }
};