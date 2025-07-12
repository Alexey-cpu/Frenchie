#include <FrenchieRendererScene3DMousePicker.hpp>
#include <FrenchieRendererScene3D.hpp>

using namespace Frenchie::Renderer;

#include <iostream>

Scene3DMousePicker::Scene3DMousePicker() : 
    Component::Registry<Scene3DMousePicker>(STRINGIFY(Scene3DMousePicker))
{}

Scene3DMousePicker::~Scene3DMousePicker(){}

Scene3DMousePicker::PickedObjects Scene3DMousePicker::pick(const glm::vec3& _CursorNDCPosition) const
{
    auto scene = get_object<Scene3D>();

    if(scene == nullptr) 
        return PickedObjects();

    auto camera = scene->get_component<Camera>();

    if(camera == nullptr) 
        return PickedObjects();

    // retrieve viewport and camera transform matrixes
    //auto viewportScaleMatrix    = scene->get_component<Transform>()->get_model_matrix();
    auto cameraViewMatrix       = camera->get_view_matrix();
    auto cameraProjectionMatrix = camera->get_projection_matrix();
    auto screenTransformMatrix  = cameraProjectionMatrix * cameraViewMatrix;
    //auto cameraWorldPosition    = glm::vec3(viewportScaleMatrix * glm::vec4(camera->get_position(), 1.f));

    // create a ray pointing from cursor to the end of the scene
    Ray ray(
        glm::vec3(glm::inverse(screenTransformMatrix) * glm::vec4(_CursorNDCPosition, 1.f)), 
        glm::vec3(0.f, 0.f, -1.f));

    PickedObjects pickedObjects;

    // cast ray
    scene->apply_to_children_recursive(
        [&cameraViewMatrix, 
            &cameraProjectionMatrix, 
            &screenTransformMatrix, 
            &ray, 
            &pickedObjects](Object* _Object)
        {
            auto transform = _Object->get_component<Transform>();
            auto mesh      = _Object->get_component<IMesh>();

            if(transform == nullptr || mesh == nullptr) 
                return;

            auto modelMatrix         = transform->get_model_matrix();
            auto aabbTransformMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f / (screenTransformMatrix * modelMatrix)[3][3]));

            if(!mesh->get_mesh()->get_aabb().transform(aabbTransformMatrix * modelMatrix).intersects(ray)) 
                return;

            PickedObject object = 
            {
                _Object, 
                transform->get_position(), 
                glm::length(transform->get_world_position() - ray.Origin)
            };

            pickedObjects.insert(object);
        }
    );

    return pickedObjects;
}

void Scene3DMousePicker::draw_editor() 
{
}

Component::TReturnType Scene3DMousePicker::create()
{
    return std::make_unique<Scene3DMousePicker>();
}