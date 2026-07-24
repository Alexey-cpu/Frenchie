// Core
#include <FrenchieCoreMathTest.hpp>

// STL
#include <iostream>

#define EPSILON 1e-4

void frenchie_math_core_gs_complex_test()
{
    printf("%s\n", GS_STRINGIFY(frenchie_math_core_gs_complex_test()));

    // auxiliary lambdas
    auto print_complex = [](const gs_complex<float>& _Complex)
    {
        printf("{%s %.4f %s %.4f}",
            gs_sign(gs_realf(_Complex)) >= 0 ? "+" : "-",
            gs_abs(gs_realf(_Complex)),
            gs_sign(gs_imagf(_Complex)) >= 0 ? "+i" : "-i",
            gs_abs(gs_imagf(_Complex)));
    };

    // test code
    float re = gs_pseudo_random<float>(-10.f, +10.f);
    float im = gs_pseudo_random<float>(-10.f, +10.f);
    gs_complex a = gs_complex(re, im);
    gs_complex b = gs_complex(gs_pseudo_random<float>(-10.f, +10.f), gs_pseudo_random<float>(-10.f, +10.f));

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
        gs_complex c = a + b;
        print_complex(a); printf(" + "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(c) - (gs_realf(a) + gs_realf(b))) < EPSILON &&
            gs_abs(gs_imagf(c) - (gs_imagf(a) + gs_imagf(b))) < EPSILON
        );
    }

    // sub
    {
        gs_complex c = a - b;
        print_complex(a); printf(" - "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(c) - (gs_realf(a) - gs_realf(b))) < EPSILON &&
            gs_abs(gs_imagf(c) - (gs_imagf(a) - gs_imagf(b))) < EPSILON
        );
    }

    // mul
    {
        gs_complex c = a * b;
        print_complex(a); printf(" * "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(c) - (gs_realf(a) * gs_realf(b) - gs_imagf(a) * gs_imagf(b))) < EPSILON &&
            gs_abs(gs_imagf(c) - (gs_realf(a) * gs_imagf(b) + gs_imagf(a) * gs_realf(b))) < EPSILON
        );
    }

    // div
    {
        gs_complex c = a / b;
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
        gs_complex sqr = gs_csqrtf(a);
        printf("sqrt("); print_complex(a); printf(") = "); print_complex(sqr); printf("\n");

        float abs = gs_cabsf(a);
        float arg = gs_cargf(a);
        gs_complex res = gs_complex<float>(cos(arg * 0.5), sin(arg * 0.5)) * sqrt(abs);

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
            gs_complex pw = gs_cpowf(a, power);
            printf("pow("); print_complex(a); printf(", %f) = ", power); print_complex(pw); printf("\n");

            float abs = gs_cabsf(a);
            float arg = gs_cargf(a);
            
            gs_complex res = gs_complex(cos(arg * power), sin(arg * power)) * pow(abs, power);

            GS_ASSERT(
                gs_abs(gs_realf(pw) - gs_realf(res)) < EPSILON &&
                gs_abs(gs_imagf(pw) - gs_imagf(res)) < EPSILON
            );
        }
    }

    // conj
    {
        gs_complex conj = gs_conjf(a);
        printf("conj("); print_complex(a); printf(") = "); print_complex(conj); printf("\n");

        GS_ASSERT(
            gs_abs(gs_realf(a) - gs_realf(conj)) < EPSILON &&
            gs_abs(gs_imagf(a) + gs_imagf(conj)) < EPSILON
        );
    }

    // norm
    {
        gs_complex nrm = gs_cnormf(a);
        gs_complex res = gs_complex(gs_realf(a) / gs_cabsf(a), gs_imagf(a) / gs_cabsf(a));
        printf("normalize( "); print_complex(a); printf(") = "); print_complex(nrm); printf(" --> "); print_complex(res); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(nrm) - gs_realf(res)) < EPSILON &&
            gs_abs(gs_imagf(nrm) - gs_imagf(res)) < EPSILON
        );
    }

    // rot
    {
        float arg = 30.f;
        gs_complex rot = gs_crotf<float>(gs_to_radians<float>(arg));
        printf("rot(%f) = ", arg); print_complex(rot); printf("\n");

        GS_ASSERT(
            gs_abs(gs_realf(rot) - cos(gs_to_radians(arg))) < EPSILON &&
            gs_abs(gs_imagf(rot) - sin(gs_to_radians(arg))) < EPSILON
        );
    }

    // sinh
    {
        gs_complex res = gs_sinhf(a);
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
        gs_complex res = gs_coshf(a);
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

void frenchie_math_core_gs_vector_test()
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

    printf("\n");
}

void frenchie_math_core_gs_matrix_test()
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

    printf("\n");
}

#undef EPSILON