#pragma once

#include <FrenchieCoreObject.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// STL
#include <set>

namespace Frenchie
{
    namespace Renderer
    {
        class Scene3DMousePicker : public Core::Component::Registry<Scene3DMousePicker>, public IEditor
        {
        public:
            struct PickedObject
            {
                PickedObject(Core::Object* _Object, glm::vec3 _Position, float _DistanceFromRay) : 
                    Object(_Object), 
                    Position(_Position), 
                    DistanceFromRay(_DistanceFromRay){}
                
                ~PickedObject(){}

                Core::Object* Object          = nullptr;
                glm::vec3     Position        = glm::vec3(0.f);
                float         DistanceFromRay = 0.f;

                // nested types
                struct TransparentComparator
                {
                    using is_transparent = PickedObject;

                public:

                    bool operator()(const PickedObject& _A, const PickedObject& _B) const
                    {
                        return  _A.DistanceFromRay < _B.DistanceFromRay;
                    }
                };
            };

            typedef std::set<Scene3DMousePicker::PickedObject, 
                Scene3DMousePicker::PickedObject::TransparentComparator> PickedObjects;

            Scene3DMousePicker();
            virtual ~Scene3DMousePicker();

            PickedObjects pick(const glm::vec3&) const;

            // IEditor
            virtual void draw_editor() override;

            // Component::Register<Transform>
            static TReturnType create();
        };
    }
}