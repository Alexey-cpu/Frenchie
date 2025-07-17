#include <FrenchieRendererScene3DMousePicker.hpp>
#include <FrenchieRendererScene3D.hpp>

using namespace Frenchie::Renderer;

Scene3DMousePicker::Scene3DMousePicker() : 
    Component::Registry<Scene3DMousePicker>(STRINGIFY(Scene3DMousePicker)){}

Scene3DMousePicker::~Scene3DMousePicker(){}

Scene3DMousePicker::PickedObjects Scene3DMousePicker::pick(const glm::vec3& _CursorNDCPosition) const
{
    auto scene = get_object<Scene3D>();

    if(scene == nullptr || !is_enabled()) 
        return PickedObjects();

    auto camera = scene->get_component<Camera>();

    if(camera == nullptr) 
        return PickedObjects();

    // compute projection matrix
    auto projectionMatrix = 
        camera->get_projection_matrix() * camera->get_view_matrix();

    // create a ray pointing from cursor to the end of the scene
    Ray ray(_CursorNDCPosition, glm::vec3(0.f, 0.f, -1.f));

    PickedObjects pickedObjects;

    // cast ray
    scene->apply_to_children_recursive(
        [&ray, &pickedObjects, &projectionMatrix](Object* _Object)
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

void Scene3DMousePicker::draw_editor(){}

Component::TReturnType Scene3DMousePicker::create()
{
    return std::make_unique<Scene3DMousePicker>();
}