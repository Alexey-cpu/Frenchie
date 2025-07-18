#include <FrenchieRendererScene3DCursor.hpp>
#include <FrenchieRendererScene3D.hpp>

using namespace Frenchie::Renderer;

Scene3DCursor::Scene3DCursor() : 
    Component::Registry<Scene3DCursor>(STRINGIFY(Scene3DCursor)){}

Scene3DCursor::~Scene3DCursor(){}

Scene3DCursor::PickedObjects Scene3DCursor::pick(const glm::vec3& _CursorNDCPosition) const
{
    if(!is_enabled()) 
        return PickedObjects();

    // get scene
    auto scene = get_object() != nullptr ? get_object()->get_parent_recursive<Scene3D>() : nullptr;
    if(scene == nullptr) 
        scene = get_object<Scene3D>();

    if(scene == nullptr) 
        return PickedObjects();

    auto camera = get_object()->get_component<Camera>();

    if(camera == nullptr) 
        return PickedObjects();

    // compute projection matrix
    auto projectionMatrix = camera->get_projection_matrix();

    // create a ray pointing from cursor to the end of the scene
    Ray ray(_CursorNDCPosition, glm::vec3(0.f, 0.f, -1.f));

    PickedObjects pickedObjects;

    // cast ray
    scene->apply_to_children_recursive(
        [&ray, &pickedObjects, &projectionMatrix](objectRef _Object)
        {
            auto transform = _Object->get_component<Transform>();
            auto mesh      = _Object->get_component<IMesh>();

            if(transform == nullptr || mesh == nullptr) 
                return;

            auto objectTransformMatrix = projectionMatrix * transform->get_model_matrix();
            objectTransformMatrix      = glm::scale(glm::mat4(1.f), glm::vec3(1.f / (objectTransformMatrix)[3][3])) * objectTransformMatrix;

            if(!mesh->get_mesh()->get_aabb().transform(objectTransformMatrix).intersects(ray)) 
                return;

            pickedObjects.insert(
                PickedObject(
                    _Object, 
                    transform->get_position(), 
                    glm::length(transform->get_world_position() - ray.Origin)
                )
            );
        }
    );

    return pickedObjects;
}

void Scene3DCursor::draw_editor(){}

Component::TReturnType Scene3DCursor::create()
{
    return std::make_unique<Scene3DCursor>();
}