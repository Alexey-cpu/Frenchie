#include <FrenchieCoreMathTest.hpp>

void frenchie_math_core_gs_complex_test()
{
    std::cout << GS_STRINGIFY(frenchie_math_core_gs_complex_test()) << "\n";

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
            gs_abs(gs_realf(a) - re) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(a) - im) < gs_epsilon<float>());
    }

    // add
    {
        gs_complex c = a + b;
        print_complex(a); printf(" + "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(c) - (gs_realf(a) + gs_realf(b))) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(c) - (gs_imagf(a) + gs_imagf(b))) < gs_epsilon<float>()
        );
    }

    // sub
    {
        gs_complex c = a - b;
        print_complex(a); printf(" - "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(c) - (gs_realf(a) - gs_realf(b))) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(c) - (gs_imagf(a) - gs_imagf(b))) < gs_epsilon<float>()
        );
    }

    // mul
    {
        gs_complex c = a * b;
        print_complex(a); printf(" * "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(c) - (gs_realf(a) * gs_realf(b) - gs_imagf(a) * gs_imagf(b))) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(c) - (gs_realf(a) * gs_imagf(b) + gs_imagf(a) * gs_realf(b))) < gs_epsilon<float>()
        );
    }

    // div
    {
        gs_complex c = a / b;
        print_complex(a); printf(" / "); print_complex(b); printf(" = "); print_complex(c); printf("\n");
        float scal = gs_realf(b) * gs_realf(b) + gs_imagf(b) * gs_imagf(b);
        GS_ASSERT(
            gs_abs(gs_realf(c) - ((+gs_realf(a) * gs_realf(b) + gs_imagf(a) * gs_imagf(b)) / scal)) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(c) - ((-gs_realf(a) * gs_imagf(b) + gs_imagf(a) * gs_realf(b)) / scal)) < gs_epsilon<float>()
        );
    }

    // abs
    {
        float abs = gs_cabsf(a);
        printf("|"); print_complex(a); printf("|"); printf(" = "); printf("%f", abs); printf("\n");

        GS_ASSERT(
            gs_abs(abs - sqrtf(gs_realf(a) * gs_realf(a) + gs_imagf(a) * gs_imagf(a))) < gs_epsilon<float>()
        );
    }

    // arg
    {
        float arg = gs_to_degrees(gs_cargf(a));
        printf("arg("); print_complex(a); printf(")"); printf(" = "); printf("%f", arg); printf("\n");

        GS_ASSERT(
            gs_abs(arg - gs_to_degrees(atan2(gs_imagf(a), gs_realf(a)))) < gs_epsilon<float>()
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
            gs_abs(gs_realf(sqr) - gs_realf(res)) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(sqr) - gs_imagf(res)) < gs_epsilon<float>()
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
                gs_abs(gs_realf(pw) - gs_realf(res)) < gs_epsilon<float>() &&
                gs_abs(gs_imagf(pw) - gs_imagf(res)) < gs_epsilon<float>()
            );
        }
    }

    // conj
    {
        gs_complex conj = gs_conjf(a);
        printf("conj("); print_complex(a); printf(") = "); print_complex(conj); printf("\n");

        GS_ASSERT(
            gs_abs(gs_realf(a) - gs_realf(conj)) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(a) + gs_imagf(conj)) < gs_epsilon<float>()
        );
    }

    // norm
    {
        gs_complex nrm = gs_cnormf(a);
        gs_complex res = gs_complex(gs_realf(a) / gs_cabsf(a), gs_imagf(a) / gs_cabsf(a));
        printf("normalize( "); print_complex(a); printf(") = "); print_complex(nrm); printf(" --> "); print_complex(res); printf("\n");
        GS_ASSERT(
            gs_abs(gs_realf(nrm) - gs_realf(res)) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(nrm) - gs_imagf(res)) < gs_epsilon<float>()
        );
    }

    // rot
    {
        float arg = 30.f;
        gs_complex rot = gs_crotf<float>(gs_to_radians<float>(arg));
        printf("rot(%f) = ", arg); print_complex(rot); printf("\n");

        GS_ASSERT(
            gs_abs(gs_realf(rot) - cos(gs_to_radians(arg))) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(rot) - sin(gs_to_radians(arg))) < gs_epsilon<float>()
        );
    }

    // sinh
    {
        gs_complex res = gs_sinhf(a);
        float re_ = (exp(gs_realf(a)) * cos(gs_imagf(a)) - exp(-gs_realf(a)) * cos(-gs_imagf(a))) * static_cast<float>(0.5);
        float im_ = (exp(gs_realf(a)) * sin(gs_imagf(a)) - exp(-gs_realf(a)) * sin(-gs_imagf(a))) * static_cast<float>(0.5);
        printf("sinh("); print_complex(a); printf(") = "); print_complex(res); printf("\n");

        GS_ASSERT(
            gs_abs(gs_realf(res) - re_) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(res) - im_) < gs_epsilon<float>()
        );
    }

    // cosh
    {
        gs_complex res = gs_coshf(a);
        float re_ = (exp(gs_realf(a)) * cos(gs_imagf(a)) + exp(-gs_realf(a)) * cos(-gs_imagf(a))) * static_cast<float>(0.5);
        float im_ = (exp(gs_realf(a)) * sin(gs_imagf(a)) + exp(-gs_realf(a)) * sin(-gs_imagf(a))) * static_cast<float>(0.5);
        printf("cosh("); print_complex(a); printf(") = "); print_complex(res); printf("\n");

        GS_ASSERT(
            gs_abs(gs_realf(res) - re_) < gs_epsilon<float>() &&
            gs_abs(gs_imagf(res) - im_) < gs_epsilon<float>()
        );
    }
}

void frenchie_math_core_gs_vector_test()
{
    std::cout << GS_STRINGIFY(frenchie_math_core_gs_vector_test()) << "\n";

    gs_vec4f a = {1.f, 2.f, 3.f, 4.f};
    gs_vec4f b = {5.f, 6.f, 7.f, 8.f};

    // add
    {
        gs_vec4f c = a + b;

        printf("%s:\n", GS_STRINGIFY(c = a + b));

        for (int i = 0; i < c.size(); i++)
        {
            printf("c[%d]=%f\n", i, c[i]);
            GS_ASSERT(
                gs_abs((a[i] + b[i]) - c[i]) < gs_epsilon<float>()
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
                gs_abs((a[i] - b[i]) - c[i]) < gs_epsilon<float>()
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
                gs_abs((a[i] * b[i]) - c[i]) < gs_epsilon<float>()
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
                gs_abs((a[i] / b[i]) - c[i]) < gs_epsilon<float>()
            );
        }
    }
}