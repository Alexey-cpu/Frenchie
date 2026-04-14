#include <FrenchieCoreMath.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

int main(int argc, char *argv[])
{
    // ortho projections test
    {
        std::cout << "running ortho projections test ...\n";

        float top    = 32.f;
        float left   = 64.f;
        float right  = 2048.f;
        float bottom = 1024.f;
        float near   = -0.5f;
        float far    = +1.f;

        // GLM results
        glm::mat4 glmOrthoRHNO = glm::orthoRH_NO(left, right, bottom, top, near, far);
        glm::mat4 glmOrthoRHZO = glm::orthoRH_ZO(left, right, bottom, top, near, far);
        glm::mat4 glmOrthoLHNO = glm::orthoLH_NO(left, right, bottom, top, near, far);
        glm::mat4 glmOrthoLHZO = glm::orthoLH_ZO(left, right, bottom, top, near, far);

        // GS results
        gs_mat4f gsOrthoRHNO = gs_matrix_ortho(left, right, bottom, top, near, far, true, true);
        gs_mat4f gsOrthoRHZO = gs_matrix_ortho(left, right, bottom, top, near, far, true, false);
        gs_mat4f gsOrthoLHNO = gs_matrix_ortho(left, right, bottom, top, near, far, false, true);
        gs_mat4f gsOrthoLHZO = gs_matrix_ortho(left, right, bottom, top, near, far, false, false);

        for (int j = 0; j < gsOrthoRHNO.columns(); j++)
        {
            for (int i = 0; i < gsOrthoRHNO.rows(); i++)
            {
                GS_ASSERT(gs_abs(glmOrthoRHNO[j][i] - gsOrthoRHNO[j][i]) < gs_epsilon<float>());
                GS_ASSERT(gs_abs(glmOrthoRHZO[j][i] - gsOrthoRHZO[j][i]) < gs_epsilon<float>());
                GS_ASSERT(gs_abs(glmOrthoLHNO[j][i] - gsOrthoLHNO[j][i]) < gs_epsilon<float>());
                GS_ASSERT(gs_abs(glmOrthoLHZO[j][i] - gsOrthoLHZO[j][i]) < gs_epsilon<float>());
            }
        }
    }
    
    // perspective projection tests
    {
        std::cout << "running perspective projection tests ...\n";

        float fov    = gs_to_radians(90.f);
        float aspect = 1.f;
        float near   = -0.5f;
        float far    = +1.f;

        // GLM results
        glm::mat4 glmOrthoRHNO = glm::perspectiveRH_NO(fov, aspect, near, far);
        glm::mat4 glmOrthoRHZO = glm::perspectiveRH_ZO(fov, aspect, near, far);
        glm::mat4 glmOrthoLHNO = glm::perspectiveLH_NO(fov, aspect, near, far);
        glm::mat4 glmOrthoLHZO = glm::perspectiveLH_ZO(fov, aspect, near, far);

        // GS results
        gs_mat4f gsOrthoRHNO = gs_matrix_perspective(fov, aspect, near, far, true, true);
        gs_mat4f gsOrthoRHZO = gs_matrix_perspective(fov, aspect, near, far, true, false);
        gs_mat4f gsOrthoLHNO = gs_matrix_perspective(fov, aspect, near, far, false, true);
        gs_mat4f gsOrthoLHZO = gs_matrix_perspective(fov, aspect, near, far, false, false);

        for (int j = 0; j < gsOrthoRHNO.columns(); j++)
        {
            for (int i = 0; i < gsOrthoRHNO.rows(); i++)
            {
                GS_ASSERT(gs_abs(glmOrthoRHNO[j][i] - gsOrthoRHNO[j][i]) < gs_epsilon<float>());
                GS_ASSERT(gs_abs(glmOrthoRHZO[j][i] - gsOrthoRHZO[j][i]) < gs_epsilon<float>());
                GS_ASSERT(gs_abs(glmOrthoLHNO[j][i] - gsOrthoLHNO[j][i]) < gs_epsilon<float>());
                GS_ASSERT(gs_abs(glmOrthoLHZO[j][i] - gsOrthoLHZO[j][i]) < gs_epsilon<float>());
            }
        }
    }

    // camera look at test
    {
        std::cout << "running camera look at test ...\n";

        gs_vec3f _CameraWorldFrontAxisDirection = gs_vec3f(0.f, 0.f, 1.f);
        gs_vec3f _CameraWorldUpAxisDirection    = gs_vec3f(0.f, 1.f, 0.f);

        gs_vec3f cameraLocalFrontAxisDirection = gs_vector_normalize(_CameraWorldFrontAxisDirection);
        gs_vec3f cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, _CameraWorldUpAxisDirection));
        gs_vec3f cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vector_cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

        // GLM results
        glm::mat4 glmLookAtRH = glm::lookAtRH(
            glm::vec3(0.f, 0.f, 1),
            glm::vec3(0.f, 0.f, 1) + glm::vec3(cameraLocalFrontAxisDirection.x, cameraLocalFrontAxisDirection.y, cameraLocalFrontAxisDirection.z),
            glm::vec3(cameraLocalUpAxisDirection.x, cameraLocalUpAxisDirection.y, cameraLocalUpAxisDirection.z));
        
        glm::mat4 glmLookAtLH = glm::lookAtLH(
            glm::vec3(0.f, 0.f, 1),
            glm::vec3(0.f, 0.f, 1) + glm::vec3(cameraLocalFrontAxisDirection.x, cameraLocalFrontAxisDirection.y, cameraLocalFrontAxisDirection.z),
            glm::vec3(cameraLocalUpAxisDirection.x, cameraLocalUpAxisDirection.y, cameraLocalUpAxisDirection.z));

        // GS results
        gs_mat4f gsLookAtRH = gs_matrix_look_at(
            gs_vec3f(0.f, 0.f, 1),
            gs_vec3f(0.f, 0.f, 1) + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection, true);

        gs_mat4f gsLookAtLH = gs_matrix_look_at(
            gs_vec3f(0.f, 0.f, 1),
            gs_vec3f(0.f, 0.f, 1) + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection, false);

        for (int j = 0; j < gsLookAtRH.columns(); j++)
        {
            for (int i = 0; i < gsLookAtRH.rows(); i++)
            {
                GS_ASSERT(gs_abs(glmLookAtRH[j][i] - gsLookAtRH[j][i]) < gs_epsilon<float>());
                GS_ASSERT(gs_abs(glmLookAtLH[j][i] - gsLookAtLH[j][i]) < gs_epsilon<float>());
            }
        }
    }

    std::cout << "success ...\n";

    return 0;
}