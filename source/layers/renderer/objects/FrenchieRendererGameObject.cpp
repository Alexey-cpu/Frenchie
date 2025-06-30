#include <FrenchieRendererGameObject.hpp>

#include <FrenchieRendererTransformComponent.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Renderer;

GameObject::GameObject(const std::string _Name) : Object(_Name)
{
    add_component<Transform>();
}

GameObject::~GameObject(){}