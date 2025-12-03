#pragma once

#include <FrenchieCoreMath.hpp>

namespace Frenchie
{
    namespace Application
    {
        class Camera2D
        {
        public:
            void set_resolution(){}

        protected:
            gs_vec3f cameraWorldPosition          {+0.0f, +0.f, +1.f};
            gs_vec3f cameraWorldUpAxisDirection   {+0.0f, +1.f, +0.f};
            gs_vec3f cameraLocalFrontAxisDirection{0.f, 0.f, 0.f};
            gs_vec3f cameraLocalRightAxisDirection{0.f, 0.f, 0.f};
            gs_vec3f cameraLocalUpAxisDirection   {0.f, 0.f, 0.f};
            gs_vec3f eulerAngles                  {0.f, 0.f, 0.f};
            gs_vec2f resolution                   {2048.f, 1024.f};
            float    depthOfView                  {1000.f};
        };
    }
};