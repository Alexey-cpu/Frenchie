// Core
#include <FrenchieCoreMathTest.hpp>

// STL
#include <iostream>

using namespace Frenchie::Core::Tests;

#define EPSILON 1e-4

void Frenchie::Core::Tests::frenchie_math_core_gs_complex_test()
{
    printf("%s\n", GS_STRINGIFY(frenchie_math_core_gs_complex_test()));

    typedef gs_complex<float> COMPLEX_NUMBER;

    // auxiliary lambdas
    auto print_complex = [](const COMPLEX_NUMBER& _Number)
    {
        printf("{%s %.4f %s %.4f}",
            gs_sign(gs_realf(_Number)) >= 0 ? "+" : "-",
            gs_abs(gs_realf(_Number)),
            gs_sign(gs_imagf(_Number)) >= 0 ? "+i" : "-i",
            gs_abs(gs_imagf(_Number)));
    };

    // test code
    float re = gs_pseudo_random<float>(-10.f, +10.f);
    float im = gs_pseudo_random<float>(-10.f, +10.f);
    COMPLEX_NUMBER a = COMPLEX_NUMBER(re, im);
    COMPLEX_NUMBER b = COMPLEX_NUMBER(gs_pseudo_random<float>(-10.f, +10.f), gs_pseudo_random<float>(-10.f, +10.f));

    // re/im
    {
        printf("re("); print_complex(a); printf(") = "); printf("%f\n", re);
        printf("im("); print_complex(a); printf(") = "); printf("%f\n", im);

        GS_ASSERT(
            gs_abs(gs_realf(a) - re) < EPSILON &&
            gs_abs(gs_imagf(a) - im) < EPSILON);
    }

    {
        GS_ASSERT(a == a);
        GS_ASSERT(a != b);
    }

    // add
    {
        COMPLEX_NUMBER c = a + b;
        print_complex(a); printf(" + "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(c) - (gs_realf(a) + gs_realf(b))) < EPSILON &&
            gs_abs(gs_imagf(c) - (gs_imagf(a) + gs_imagf(b))) < EPSILON
        );
    }

    // sub
    {
        COMPLEX_NUMBER c = a - b;
        print_complex(a); printf(" - "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(c) - (gs_realf(a) - gs_realf(b))) < EPSILON &&
            gs_abs(gs_imagf(c) - (gs_imagf(a) - gs_imagf(b))) < EPSILON
        );
    }

    // mul
    {
        COMPLEX_NUMBER c = a * b;
        print_complex(a); printf(" * "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(c) - (gs_realf(a) * gs_realf(b) - gs_imagf(a) * gs_imagf(b))) < EPSILON &&
            gs_abs(gs_imagf(c) - (gs_realf(a) * gs_imagf(b) + gs_imagf(a) * gs_realf(b))) < EPSILON
        );
    }

    // div
    {
        COMPLEX_NUMBER c = a / b;
        print_complex(a); printf(" / "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        float scal = gs_realf(b) * gs_realf(b) + gs_imagf(b) * gs_imagf(b);
        GS_ASSERT(
            gs_abs(gs_realf(c) - ((+gs_realf(a) * gs_realf(b) + gs_imagf(a) * gs_imagf(b)) / scal)) < EPSILON &&
            gs_abs(gs_imagf(c) - ((-gs_realf(a) * gs_imagf(b) + gs_imagf(a) * gs_realf(b)) / scal)) < EPSILON
        );
    }

    // abs
    {
        float abs = gs_cabsf(a);
        printf("|"); print_complex(a); printf("|"); printf(" = "); printf("%f", abs); printf("\n");

        GS_ASSERT(
            gs_abs(abs - sqrtf(gs_realf(a) * gs_realf(a) + gs_imagf(a) * gs_imagf(a))) < EPSILON
        );
    }

    // arg
    {
        float arg = gs_to_degrees(gs_cargf(a));
        printf("arg("); print_complex(a); printf(")"); printf(" = "); printf("%f", arg); printf("\n");

        GS_ASSERT(
            gs_abs(arg - gs_to_degrees(atan2(gs_imagf(a), gs_realf(a)))) < EPSILON
        );
    }

    // sqrt
    {
        COMPLEX_NUMBER sqr = gs_csqrtf(a);
        printf("sqrt("); print_complex(a); printf(") = "); print_complex(sqr); printf("\n");

        float abs = gs_cabsf(a);
        float arg = gs_cargf(a);
        COMPLEX_NUMBER res = COMPLEX_NUMBER(cos(arg * 0.5), sin(arg * 0.5)) * sqrtf(abs);

        GS_ASSERT(
            gs_abs(gs_realf(sqr) - gs_realf(res)) < EPSILON &&
            gs_abs(gs_imagf(sqr) - gs_imagf(res)) < EPSILON
        );
    }

    // pow
    {
        for(int i = 1; i < 10; i++)
        {
            float power = (float)i;
            COMPLEX_NUMBER pw = gs_cpowf(a, power);
            printf("pow("); print_complex(a); printf(", %f) = ", power); print_complex(pw); printf("\n");

            float abs = gs_cabsf(a);
            float arg = gs_cargf(a);
            
            COMPLEX_NUMBER res = COMPLEX_NUMBER(cos(arg * power), sin(arg * power)) * powf(abs, power);

            GS_ASSERT(
                gs_abs(gs_realf(pw) - gs_realf(res)) < EPSILON &&
                gs_abs(gs_imagf(pw) - gs_imagf(res)) < EPSILON
            );
        }
    }

    // conj
    {
        COMPLEX_NUMBER conj = gs_conjf(a);
        printf("conj("); print_complex(a); printf(") = "); print_complex(conj); printf("\n");

        GS_ASSERT(
            gs_abs(gs_realf(a) - gs_realf(conj)) < EPSILON &&
            gs_abs(gs_imagf(a) + gs_imagf(conj)) < EPSILON
        );
    }

    // norm
    {
        COMPLEX_NUMBER nrm = gs_cnormf(a);
        COMPLEX_NUMBER res = COMPLEX_NUMBER(gs_realf(a) / gs_cabsf(a), gs_imagf(a) / gs_cabsf(a));
        printf("normalize( "); print_complex(a); printf(") = "); print_complex(nrm); printf(" --> "); print_complex(res); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(nrm) - gs_realf(res)) < EPSILON &&
            gs_abs(gs_imagf(nrm) - gs_imagf(res)) < EPSILON
        );
    }

    // rot
    {
        float arg = 30.f;
        COMPLEX_NUMBER rot = gs_crotf<float>(gs_to_radians<float>(arg));
        printf("rot(%f) = ", arg); print_complex(rot); printf("\n");

        GS_ASSERT(
            gs_abs(gs_realf(rot) - cos(gs_to_radians(arg))) < EPSILON &&
            gs_abs(gs_imagf(rot) - sin(gs_to_radians(arg))) < EPSILON
        );
    }

    // sinh
    {
        COMPLEX_NUMBER res = gs_sinhf(a);
        float re_ = (exp(gs_realf(a)) * cos(gs_imagf(a)) - exp(-gs_realf(a)) * cos(-gs_imagf(a))) * static_cast<float>(0.5);
        float im_ = (exp(gs_realf(a)) * sin(gs_imagf(a)) - exp(-gs_realf(a)) * sin(-gs_imagf(a))) * static_cast<float>(0.5);
        printf("sinh("); print_complex(a); printf(") = "); print_complex(res); printf("\n");

        GS_ASSERT(
            gs_abs(gs_realf(res) - re_) < EPSILON &&
            gs_abs(gs_imagf(res) - im_) < EPSILON
        );
    }

    // cosh
    {
        COMPLEX_NUMBER res = gs_coshf(a);
        float re_ = (exp(gs_realf(a)) * cos(gs_imagf(a)) + exp(-gs_realf(a)) * cos(-gs_imagf(a))) * static_cast<float>(0.5);
        float im_ = (exp(gs_realf(a)) * sin(gs_imagf(a)) + exp(-gs_realf(a)) * sin(-gs_imagf(a))) * static_cast<float>(0.5);
        printf("cosh("); print_complex(a); printf(") = "); print_complex(res); printf("\n");

        GS_ASSERT(
            gs_abs(gs_realf(res) - re_) < EPSILON &&
            gs_abs(gs_imagf(res) - im_) < EPSILON
        );
    }

    printf("\n");
}

void Frenchie::Core::Tests::frenchie_math_core_gs_vector_test()
{
    printf("%s\n", GS_STRINGIFY(frenchie_math_core_gs_vector_test()));

    gs_vec4f a, b;

    for (int i = 0; i < a.size(); i++)
    {
        a[i] = gs_pseudo_random<float>(-10.f, +10.f);
        b[i] = gs_pseudo_random<float>(-10.f, +10.f);
    }

    // add
    {
        gs_vec4f c = a + b;

        printf("%s:\n", GS_STRINGIFY(c = a + b));

        for (int i = 0; i < c.size(); i++)
        {
            printf("c[%d]=%f\n", i, c[i]);
            GS_ASSERT(
                gs_abs((a[i] + b[i]) - c[i]) < EPSILON
            );
        }
    }

    // sub
    {
        gs_vec4f c = a - b;
        printf("%s:\n", GS_STRINGIFY(c = a - b));
        for (int i = 0; i < c.size(); i++)
        {
            printf("c[%d]=%f\n", i, c[i]);
            GS_ASSERT(
                gs_abs((a[i] - b[i]) - c[i]) < EPSILON
            );
        }
    }

    // mul
    {
        gs_vec4f c = a * b;
        printf("%s:\n", GS_STRINGIFY(c = a * b));
        for (int i = 0; i < c.size(); i++)
        {
            printf("c[%d]=%f\n", i, c[i]);
            GS_ASSERT(
                gs_abs((a[i] * b[i]) - c[i]) < EPSILON
            );
        }
    }

    // div
    {
        gs_vec4f c = a / b;
        printf("%s:\n", GS_STRINGIFY(c = a / b));
        for (int i = 0; i < c.size(); i++)
        {
            printf("c[%d]=%f\n", i, c[i]);
            GS_ASSERT(
                gs_abs((a[i] / b[i]) - c[i]) < EPSILON
            );
        }
    }

    // == and !=
    {
        GS_ASSERT(a == a);
        GS_ASSERT(a != b);
    }

    // length
    {
        float len = gs_vector_length(a);
        float sum = 0.f;
        for (int i = 0; i < a.size(); i++)
            sum += a[i] * a[i];
        sum = sqrtf(sum);
        GS_ASSERT(gs_abs(sum - len) < EPSILON);
    }

    // normalize
    {
        gs_vec4f c = gs_vector_normalize(a);
        float len = gs_vector_length(a);

        for (int i = 0; i < a.size(); i++)
            GS_ASSERT(gs_abs(c[i] - a[i] / len) < EPSILON);
    }

    // dot product
    {
        float dot = gs_vectors_dot(a, b);
        float sum = 0.f;
        for (int i = 0; i < a.size(); i++)
            sum += a[i] * b[i];
        GS_ASSERT(gs_abs(dot - sum) < EPSILON);
    }

    // cross product 2D
    {
        gs_vec2f a, b;

        for (int i = 0; i < a.size(); i++)
        {
            a[i] = gs_pseudo_random<float>(-10.f, +10.f);
            b[i] = gs_pseudo_random<float>(-10.f, +10.f);
        }

        const float Ax = a[0];
        const float Ay = a[1];
        const float Bx = b[0];
        const float By = b[1];
        float res = Ax * By - Ay * Bx;

        GS_ASSERT(gs_abs(gs_vector_cross(a, b) - res) < EPSILON);
    }

    // cross product 3D
    {
        gs_vec3f a, b;

        for (int i = 0; i < a.size(); i++)
        {
            a[i] = gs_pseudo_random<float>(-10.f, +10.f);
            b[i] = gs_pseudo_random<float>(-10.f, +10.f);
        }

        gs_vec3f c = gs_vector_cross(a, b);

        GS_ASSERT(gs_abs(gs_vectors_dot(c, a)) < 1e-4 && gs_abs(gs_vectors_dot(c, b)) < 1e-4);
    }

    // deterministic dimensional and zero-vector edge cases
    {
        const gs_vec2f vector2(3.f, 4.f);
        const gs_vec3f vector3(0.f, 3.f, 4.f);
        const gs_vec4f zero(0.f);
        const gs_vec2f normalized2 = gs_vector_normalize(vector2);
        const gs_vec3f normalized3 = gs_vector_normalize(vector3);
        const gs_vec4f normalizedZero = gs_vector_normalize(zero);

        GS_ASSERT(gs_vector_length(vector2) == 5.f);
        GS_ASSERT(gs_vector_length(vector3) == 5.f);
        GS_ASSERT(gs_abs(normalized2.x - 0.6f) < EPSILON && gs_abs(normalized2.y - 0.8f) < EPSILON);
        GS_ASSERT(gs_abs(normalized3.x) < EPSILON && gs_abs(normalized3.y - 0.6f) < EPSILON && gs_abs(normalized3.z - 0.8f) < EPSILON);
        GS_ASSERT(normalizedZero == gs_vec4f(1.f, 0.f, 0.f, 0.f));
    }

    // scalar-on-left subtraction and division preserve operand order
    {
        const gs_vec2f vector(2.f, 5.f);
        const gs_vec2f difference = 10.f - vector;
        const gs_vec2f quotient = 10.f / vector;
        GS_ASSERT(difference == gs_vec2f(8.f, 5.f));
        GS_ASSERT(quotient == gs_vec2f(5.f, 2.f));
    }

    printf("\n");
}

void Frenchie::Core::Tests::frenchie_math_core_gs_matrix_test()
{
    printf("%s\n", GS_STRINGIFY(frenchie_math_core_gs_matrix_test()));

    gs_mat4f a, b;

    for (int i = 0; i < a.columns(); i++)
    {
        for (int j = 0; j < a.rows(); j++)
        {
            a[i][j] = gs_pseudo_random<float>(-10.f, +10.f);
            b[i][j] = gs_pseudo_random<float>(-10.f, +10.f);
        }
    }
    
    // == and !=
    {
        GS_ASSERT(a == a);
        GS_ASSERT(a != b);
    }

    // add
    {
        gs_mat4f c = a + b;

        for (int i = 0; i < a.columns(); i++)
        {
            for (int j = 0; j < a.rows(); j++)
            {
                GS_ASSERT(gs_abs(c[i][j] - (a[i][j] + b[i][j])) < EPSILON);
            }
        }
    }

    // sub
    {
        gs_mat4f c = a - b;

        for (int i = 0; i < a.columns(); i++)
        {
            for (int j = 0; j < a.rows(); j++)
            {
                GS_ASSERT(gs_abs(c[i][j] - (a[i][j] - b[i][j])) < EPSILON);
            }
        }
    }

    // solve
    {
        gs_mat4f c;

        for (int i = 0; i < a.columns(); i++)
        {
            for (int j = 0; j < a.rows(); j++)
            {
                c[i][j] = gs_pseudo_random<float>(-10.f, +10.f);
            }
        }

        gs_mat4f d = gs_matrix_solve_square(a, c);
        gs_mat4f e = a * d;

        for (int i = 0; i < a.columns(); i++)
        {
            for (int j = 0; j < a.rows(); j++)
            {
                GS_ASSERT(gs_abs(c[i][j] - e[i][j]) < 1e-4);
            }
        }
    }

    // determinant must account for row pivoting
    {
        gs_matrix<float, 2, 2> matrix;
        matrix[0][0] = 0.f;
        matrix[0][1] = 1.f;
        matrix[1][0] = 1.f;
        matrix[1][1] = 0.f;
        GS_ASSERT(gs_matrix_determinant_square(matrix) == -1.f);

        gs_matrix<float, 2, 2> pivoted;
        pivoted[0][0] = 0.f;
        pivoted[0][1] = 2.f;
        pivoted[1][0] = 1.f;
        pivoted[1][1] = 3.f;
        gs_matrix<float, 2, 1> rhs;
        rhs[0][0] = 5.f;
        rhs[0][1] = 23.f;
        const auto solution = gs_matrix_solve_square(pivoted, rhs);
        GS_ASSERT(gs_abs(solution[0][0] - 4.f) < EPSILON);
        GS_ASSERT(gs_abs(solution[0][1] - 5.f) < EPSILON);
    }

    // transpose supports non-square matrices and swaps dimensions
    {
        gs_matrix<float, 2, 3> matrix;
        matrix[0][0] = 1.f;
        matrix[0][1] = 2.f;
        matrix[1][0] = 3.f;
        matrix[1][1] = 4.f;
        matrix[2][0] = 5.f;
        matrix[2][1] = 6.f;

        const gs_matrix<float, 3, 2> transposed = gs_matrix_transpose(matrix);
        GS_ASSERT(transposed.rows() == 3 && transposed.columns() == 2);
        GS_ASSERT(transposed[0][0] == 1.f && transposed[0][1] == 3.f && transposed[0][2] == 5.f);
        GS_ASSERT(transposed[1][0] == 2.f && transposed[1][1] == 4.f && transposed[1][2] == 6.f);
    }

    // color conversion primary colors and achromatic edge case
    {
        const gs_color redHsv = gs_color_rgb_to_hsv(gs_color_rgb(255, 0, 0));
        GS_ASSERT(gs_color_hsv_get_h(redHsv) == 0);
        GS_ASSERT(gs_color_hsv_get_s(redHsv) == 255 && gs_color_hsv_get_v(redHsv) == 255);

        const gs_color greenRgb = gs_color_hsv_to_rgb(gs_color_hsv(85, 255, 255));
        GS_ASSERT(gs_color_rgba_get_r(greenRgb) == 0);
        GS_ASSERT(gs_color_rgba_get_g(greenRgb) == 255 && gs_color_rgba_get_b(greenRgb) == 0);

        const gs_color grayHsv = gs_color_rgb_to_hsv(gs_color_rgb(102, 102, 102));
        GS_ASSERT(gs_color_hsv_get_s(grayHsv) == 0 && gs_color_hsv_get_v(grayHsv) == 102);
    }

    printf("\n");
}

#undef EPSILON