#include <FrenchieCoreMath.hpp>

// [UTILITY]

// huge
template<> int8_t             gs_huge(){return INT8_MAX;  }
template<> int16_t            gs_huge(){return INT16_MAX; }
template<> int32_t            gs_huge(){return INT32_MAX; }
template<> int64_t            gs_huge(){return INT64_MAX; }
template<> float              gs_huge(){return FLT_MAX;   }
template<> double             gs_huge(){return DBL_MAX;   }
template<> long double        gs_huge(){return LDBL_MAX;  }
template<> uint8_t            gs_huge(){return UINT8_MAX; }
template<> uint16_t           gs_huge(){return UINT16_MAX;}
template<> uint32_t           gs_huge(){return UINT32_MAX;}
template<> uint64_t           gs_huge(){return UINT64_MAX;}
template<> unsigned long      gs_huge(){return ULONG_MAX; }

// tiny
template<> int8_t             gs_tiny(){return INT8_MIN; }
template<> int16_t            gs_tiny(){return INT16_MIN;}
template<> int32_t            gs_tiny(){return INT32_MIN;}
template<> int64_t            gs_tiny(){return INT64_MIN;}
template<> float              gs_tiny(){return FLT_MIN;  }
template<> double             gs_tiny(){return DBL_MIN;  }
template<> long double        gs_tiny(){return LDBL_MIN; }
template<> uint8_t            gs_tiny(){return 0;        }
template<> uint16_t           gs_tiny(){return 0;        }
template<> uint32_t           gs_tiny(){return 0;        }
template<> uint64_t           gs_tiny(){return 0;        }
template<> unsigned long      gs_tiny(){return 0;        }

// epsilon
template<> int8_t             gs_epsilon(){return 0;           }
template<> int16_t            gs_epsilon(){return 0;           }
template<> int32_t            gs_epsilon(){return 0;           }
template<> int64_t            gs_epsilon(){return 0;           }
template<> float              gs_epsilon(){return FLT_EPSILON; }
template<> double             gs_epsilon(){return DBL_EPSILON; }
template<> long double        gs_epsilon(){return LDBL_EPSILON;}
template<> uint8_t            gs_epsilon(){return 0;           }
template<> uint16_t           gs_epsilon(){return 0;           }
template<> uint32_t           gs_epsilon(){return 0;           }
template<> uint64_t           gs_epsilon(){return 0;           }
template<> unsigned long      gs_epsilon(){return 0;           }

gs_color gs_rgba_color(const gs_color& _R, const gs_color& _G, const gs_color& _B, const gs_color& _A)
{
    return (((gs_color)(_A)<<24) | ((gs_color)(_B)<<16) | ((gs_color)(_G)<<8) | ((gs_color)(_R)<<0));
}

gs_color gs_rgba_color_get_r(const gs_color& _Color)
{
    return (_Color >> 0) & 0xFF;
}

gs_color gs_rgba_color_get_g(const gs_color& _Color)
{
    return (_Color >> 8) & 0xFF;
}

gs_color gs_rgba_color_get_b(const gs_color& _Color)
{
    return (_Color >> 16) & 0xFF;
}

gs_color gs_rgba_color_get_a(const gs_color& _Color)
{
    return (_Color >> 24) & 0xFF;
}

gs_color gs_rbg_color_lerp(gs_color _SourceColor, gs_color _TargetColor, float _Fraction)
{
    int r1 = gs_rgba_color_get_r(_SourceColor);
    int g1 = gs_rgba_color_get_g(_SourceColor);
    int b1 = gs_rgba_color_get_b(_SourceColor);

    int r2 = gs_rgba_color_get_r(_TargetColor);
    int g2 = gs_rgba_color_get_g(_TargetColor);
    int b2 = gs_rgba_color_get_b(_TargetColor);

    return gs_rgba_color(
        (gs_color)(r1 + (r2 - r1) * _Fraction),
        (gs_color)(g1 + (g2 - g1) * _Fraction),
        (gs_color)(b1 + (b2 - b1) * _Fraction),
        255);
}

double gs_rgb_color_distance(const gs_color _A, const gs_color _B)
{
    return gs_vector_length(
        gs_vec3f(gs_rgba_color_get_r(_A), gs_rgba_color_get_g(_A), gs_rgba_color_get_b(_A)) -
        gs_vec3f(gs_rgba_color_get_r(_B), gs_rgba_color_get_g(_B), gs_rgba_color_get_b(_B)));
}

// [ALGEBRA]