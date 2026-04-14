#pragma once

// preprocessor
#define _CRT_SECURE_NO_WARNINGS

// C
#include <cmath>
#include <cstdio>
#include <cassert>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#define GS_ASSERT assert
#define GS_STRINGIFY(INPUT) #INPUT
#define GS_OFFSET_OF(s,m) ((::size_t)&reinterpret_cast<char const volatile&>((((s*)0)->m)))

//------------------------------------------------------------------------------------------------------------------------------------------------
// [GENERAL]
//------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef GS_TO_DEGREES_CONVERSION_MULTIPLYER__
#define GS_TO_DEGREES_CONVERSION_MULTIPLYER__ 57.295779513082320876798154814105
#endif

#ifndef GS_TO_RADIANS_CONVERSION_MULTIPLYER__
#define GS_TO_RADIANS_CONVERSION_MULTIPLYER__ 0.01745329251994329576923690768489
#endif

#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

#ifndef PI_2
#define PI_2 1.5707963267948966192313216916398
#endif

#ifndef PI_4
#define PI_4 0.78539816339744830961566084581988
#endif

// [UTILITY]
template<typename Type> Type gs_huge();
template<typename Type> Type gs_tiny();
template<typename Type> Type gs_epsilon();

template<typename Type>
inline Type gs_clamp(const Type& _Value, const Type& _Min, const Type& _Max)
{
    if(_Value < _Min) return _Min;
    if(_Value > _Max) return _Max;
    return _Value;
}

inline int gs_round_to_even(const int & _V)
{
    return ((((_V) + 1) / 2) * 2);
}

template<typename Type>
inline Type gs_to_degrees(const Type& _Angle)
{
    return _Angle * (Type)GS_TO_DEGREES_CONVERSION_MULTIPLYER__;
}

template<typename Type>
inline Type gs_to_radians(const Type& _Angle)
{
    return _Angle * (Type)GS_TO_RADIANS_CONVERSION_MULTIPLYER__;
}

template<typename Type>
inline Type gs_abs(const Type& _A)
{
    return _A < 0 ? -_A : +_A;
}

template<typename Type>
inline Type gs_sign(Type _A)
{
    if(_A == static_cast<Type>(0))
        return 0;

    return _A > 0 ? static_cast<Type>(1) : -static_cast<Type>(1);
}

template<typename Type>
inline Type gs_max(const Type& _A, const Type& _B)
{
    return _A > _B ? _A : _B;
}

template<typename Type, typename ... Args>
inline Type gs_max(const Type& _A, const Type& _B, Args... _Args)
{
    return gs_max(gs_max(_A, _B), _Args...);
}

template<typename Type>
inline Type gs_min(const Type& _A, const Type& _B)
{
    return _A < _B ? _A : _B;
}

template<typename Type, typename ... Args>
inline Type gs_min(const Type& _A, const Type& _B, Args... _Args)
{
    return gs_min(gs_min(_A, _B), _Args...);
}

template<typename Type>
inline void gs_swap(Type& _A, Type& _B)
{
    Type _C = _A;
    _A = _B;
    _B = _C;
}

template<typename Type> Type gs_pseudo_random(
    const uint_fast64_t& _Min  = gs_tiny<uint_fast64_t>(),
    const uint_fast64_t& _Max  = gs_huge<uint_fast64_t>(),
    const uint_fast64_t& _Seed = gs_huge<uint_fast64_t>())
{
    // auxiliary lambdas
    auto linearFeedbackShiftRegister64bit = [](const uint_fast64_t& _Seed)->uint_fast64_t
    {
        static uint_fast64_t seed  = _Seed;
        static uint_fast64_t value = _Seed;

        if(seed != _Seed)
        {
            seed  = _Seed;
            value = seed;
        }

        value = ((((value >> 63) ^ (value >> 62) ^ (value >> 61) ^ (value >> 59) ^ (value >> 57) ^ value ) & (uint64_t)1 ) << 63 ) | (value >> 1);
        return value;
    };

    long double integer  = (long double)(_Min + linearFeedbackShiftRegister64bit(_Seed) % ((_Max + 1 ) - _Min));
    long double floating = (long double)(linearFeedbackShiftRegister64bit(_Seed) % 1024);
    while(floating > 1.0) floating /= 1024;
    return (Type)(integer + floating);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
// [COMPLEX]
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename Type = float>
struct gs_complex
{
public:

    gs_complex()
    {
        m_data.REAL = 0;
        m_data.IMAG = 0;
    }

    gs_complex(Type _Value)
    {
        m_data.REAL = _Value;
        m_data.IMAG = 0;
    }

    gs_complex(Type _Real, Type _Imag)
    {
        m_data.REAL = _Real;
        m_data.IMAG = _Imag;
    }

    gs_complex(Type _Number[2])
    {
        m_data.REAL = _Number[0];
        m_data.IMAG = _Number[1];
    }

    inline gs_complex<Type> operator+=(const gs_complex<Type>& _Value)
    {
        complex_plain _c3 = complex_add( this->m_data, _Value.m_data );
        this->m_data.REAL = _c3.REAL;
        this->m_data.IMAG = _c3.IMAG;
        return *this;
    }

    inline gs_complex<Type> operator-=(const gs_complex<Type>& _Value)
    {
        complex_plain _c3 = complex_substract(this->m_data , _Value.m_data);
        this->m_data.REAL = _c3.REAL;
        this->m_data.IMAG = _c3.IMAG;
        return *this;
    }

    inline gs_complex<Type> operator*=(const gs_complex<Type>& _Value)
    {
        complex_plain _c3 = complex_multiply(this->m_data , _Value.m_data);
        this->m_data.REAL = _c3.REAL;
        this->m_data.IMAG = _c3.IMAG;
        return *this;
    }

    inline gs_complex<Type> operator*=(const Type& _Value) const
    {
        this->m_data.REAL *=_Value;
        this->m_data.IMAG *=_Value;
        return *this;
    }

    inline gs_complex<Type> operator/=(const gs_complex<Type>& _Value) const
    {
        complex_plain _c3 = complex_divide(this->m_data , _Value.m_data);
        this->m_data.REAL = _c3.REAL;
        this->m_data.IMAG = _c3.IMAG;
        return *this;
    }

    inline gs_complex<Type> operator/=(const Type& _Value) const
    {
        this->m_data.REAL /= _Value;
        this->m_data.IMAG /= _Value;
        return *this;
    }

    static gs_complex<Type> zero()
    {
        return gs_complex<Type>(static_cast<Type>(0), static_cast<Type>(0));
    }

    static gs_complex<Type> one()
    {
        return gs_complex<Type>(static_cast<Type>(1), static_cast<Type>(0));
    }

protected:

    // nested types
    struct complex_plain
    {
        Type REAL = Type();
        Type IMAG = Type();
    };

    // info
    mutable complex_plain m_data;

    // service methods
    static complex_plain complex_add(const complex_plain& _Number1, const complex_plain& _Number2)
    {
        return
        {
            _Number1.REAL + _Number2.REAL,
            _Number1.IMAG + _Number2.IMAG
        };
    }

    static complex_plain complex_substract(const complex_plain& _Number1, const complex_plain& _Number2)
    {
        return
        {
            _Number1.REAL - _Number2.REAL,
            _Number1.IMAG - _Number2.IMAG
        };
    }

    static complex_plain complex_multiply(const complex_plain& _Number1, const complex_plain& _Number2)
    {
        return
        {
            _Number1.REAL * _Number2.REAL - _Number1.IMAG * _Number2.IMAG,
            _Number1.REAL * _Number2.IMAG + _Number1.IMAG * _Number2.REAL
        };
    }

    static complex_plain complex_divide(const complex_plain _c1, const  complex_plain _c2)
    {
        Type scal = _c2.REAL * _c2.REAL + _c2.IMAG * _c2.IMAG;

        return
        {
            (+_c1.REAL * _c2.REAL + _c1.IMAG * _c2.IMAG) / scal,
            (-_c1.REAL * _c2.IMAG + _c1.IMAG * _c2.REAL) / scal
        };
    }

    static Type complex_abs(const complex_plain& _Number)
    {
        return (Type)(_Number.REAL == 0.0 && _Number.IMAG == 0.0 ? 0.0 : sqrt(_Number.REAL * _Number.REAL + _Number.IMAG * _Number.IMAG));
    }

    static Type complex_argument(const complex_plain& _Number)
    {
        return atan2(_Number.IMAG, _Number.REAL);
    }

    static bool complex_greater(const complex_plain& _Number1, const complex_plain& _Number2)
    {
        return complex_abs(_Number1) > complex_abs(_Number2);
    }

    static bool complex_lower(const complex_plain& _Number1, const complex_plain& _Number2)
    {
        return complex_abs(_Number1) < complex_abs(_Number2);
    }

    static bool complex_equal(const complex_plain& _Number1, const complex_plain& _Number2)
    {
        return complex_abs(_Number1) == complex_abs(_Number2);
    }

    static bool complex_greater_or_equal(const complex_plain& _Number1, const complex_plain& _Number2)
    {
        return complex_abs(_Number1) >= complex_abs(_Number2);
    }

    static bool complex_lower_or_equal(const complex_plain& _Number1, const complex_plain& _Number2)
    {
        return complex_abs(_Number1) <= complex_abs(_Number2);
    }

    // friends
    template<typename T> friend T gs_realf(const gs_complex<T>&);
    template<typename T> friend T gs_imagf(const gs_complex<T>&);
    template<typename T> friend T gs_cabsf (const gs_complex<T>&);
    template<typename T> friend T gs_cargf (const gs_complex<T>&);

    // [COMPLEX VS COMPLEX]
    template<typename T> friend gs_complex<T> operator+(const gs_complex<T>&, const gs_complex<T>&);
    template<typename T> friend gs_complex<T> operator-(const gs_complex<T>&, const gs_complex<T>&);
    template<typename T> friend gs_complex<T> operator*(const gs_complex<T>&, const gs_complex<T>&);
    template<typename T> friend gs_complex<T> operator/(const gs_complex<T>&, const gs_complex<T>&);
    template<typename T> friend bool operator> (const gs_complex<T>&, const gs_complex<T>&);
    template<typename T> friend bool operator>=(const gs_complex<T>&, const gs_complex<T>&);
    template<typename T> friend bool operator< (const gs_complex<T>&, const gs_complex<T>&);
    template<typename T> friend bool operator<=(const gs_complex<T>&, const gs_complex<T>&);
    template<typename T> friend bool operator==(const gs_complex<T>&, const gs_complex<T>&);
    template<typename T> friend bool operator!=(const gs_complex<T>&, const gs_complex<T>&);

    // [COMPLEX VS SCALAR]
    template<typename T> friend gs_complex<T> operator*(const gs_complex<T>&, const T&);
    template<typename T> friend gs_complex<T> operator/(const gs_complex<T>&, const T&);
    template<typename T> friend gs_complex<T> operator*(const T&, const gs_complex<T>&);
    template<typename T> friend gs_complex<T> operator/(const T&, const gs_complex<T>&);
};

template<typename Type> Type
gs_realf(const gs_complex<Type>& _Number)
{
    return _Number.m_data.REAL;
}

template<typename Type> Type
gs_realf(const Type& _Number)
{
    return _Number;
}

template<typename Type> Type
gs_imagf(const gs_complex<Type>& _Number)
{
    return _Number.m_data.IMAG;
}

template<typename Type> Type
gs_imagf(const Type& _Number)
{
    (void)_Number;
    return 0.0;
}

template<typename Type>
inline Type gs_cabsf(const gs_complex<Type>& _complex)
{
    return gs_complex<Type>::complex_abs(_complex.m_data);
}

template<typename Type>
Type gs_cargf(const gs_complex<Type>& _Number)
{
    return gs_complex<Type>::complex_argument(_Number.m_data);
}

template<typename Type> gs_complex<Type>
gs_csqrtf(const gs_complex<Type>& _Number)
{
    Type abs = gs_cabsf(_Number);
    Type arg = gs_cargf(_Number);
    return gs_complex<Type>(cos(arg * 0.5), sin(arg * 0.5)) * sqrt(abs);
}

template<typename Type> gs_complex<Type>
gs_cpowf(const gs_complex<Type>& _Number, const Type& _Power)
{
    Type abs = gs_cabsf(_Number);
    Type arg = gs_cargf(_Number);
    return gs_complex<Type>(cos(arg * _Power), sin(arg * _Power)) * pow(abs, _Power);
}

template<typename Type> gs_complex<Type>
gs_conjf(const gs_complex<Type>& _Number)
{
    return gs_complex<Type>(gs_realf(_Number), -gs_imagf(_Number));
}

template<typename Type> gs_complex<Type>
gs_cnormf(const gs_complex<Type>& _Number)
{
    Type abs = gs_cabsf(_Number);
    abs = (Type)(abs < 1e-12 ? (Type)1.0 : abs);
    return gs_complex<Type>(gs_realf(_Number) / abs, gs_imagf(_Number) / abs);
}

template<typename Type> gs_complex<Type>
gs_crotf(const float& _Argument)
{
    return gs_complex<Type>(cos(_Argument), sin(_Argument));
}

template<typename Type> gs_complex<Type>
gs_sinhf(const gs_complex<Type>& _Number)
{
    Type re = (exp(gs_realf(_Number)) * cos(gs_imagf(_Number)) - exp(-gs_realf(_Number)) * cos(-gs_imagf(_Number))) * static_cast<Type>(0.5);
    Type im = (exp(gs_realf(_Number)) * sin(gs_imagf(_Number)) - exp(-gs_realf(_Number)) * sin(-gs_imagf(_Number))) * static_cast<Type>(0.5);
    return gs_complex<Type>( re , im );
}

template<typename Type> gs_complex<Type>
gs_coshf(const gs_complex<Type>& _Number)
{
    float re = (exp(gs_realf(_Number)) * cos(gs_imagf(_Number)) + exp(-gs_realf(_Number)) * cos(-gs_imagf(_Number))) * static_cast<Type>(0.5);
    float im = (exp(gs_realf(_Number)) * sin(gs_imagf(_Number)) + exp(-gs_realf(_Number)) * sin(-gs_imagf(_Number))) * static_cast<Type>(0.5);
    return gs_complex<Type>(re , im);
}

template<typename Type> gs_complex<Type>
gs_tanhf(const gs_complex<Type>& _Number)
{
    return gs_sinhf(_Number) / gs_coshf(_Number);
}


template<typename Type> gs_complex<Type>
gs_ctnhf(const gs_complex<Type>& _Number)
{
    return gs_coshf(_Number) / gs_sinhf(_Number);
}

//------------------------------------------------------------------------------------------------------------------------------------------------
// [VECTOR]
//------------------------------------------------------------------------------------------------------------------------------------------------
template<typename Type, int Size>
struct gs_vector_data
{
    mutable Type Data[Size]{0};
};

template<typename Type>
struct gs_vector_data<Type, 1>
{
    mutable Type Data[1]{0};
    Type& x = Data[0];
};

template<typename Type>
struct gs_vector_data<Type, 2>
{
    mutable Type Data[2]{0};
    Type& x = Data[0];
    Type& y = Data[1];
};

template<typename Type>
struct gs_vector_data<Type, 3>
{
    mutable Type Data[3]{0};
    Type& x = Data[0];
    Type& y = Data[1];
    Type& z = Data[2];
};

template<typename Type>
struct gs_vector_data<Type, 4>
{
    mutable Type Data[4]{0};
    Type& x = Data[0];
    Type& y = Data[1];
    Type& z = Data[2];
    Type& w = Data[3];
};

template<typename Type, int Size>
struct gs_vector final : public gs_vector_data<Type, Size>
{
    typedef Type value_type;

    gs_vector()
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] = 0;
    }

    gs_vector(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] = _Value;
    }

    gs_vector(const gs_vector<Type, Size>& _Other)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] = _Other[i];
    }

    template<int OtherSize>
    gs_vector(const gs_vector<Type, OtherSize>& _Other)
    {
        for (int i = 0; i < gs_min(OtherSize, Size); i++)
            this->Data[i] = _Other[i];
    }

    template <int OtherSize, typename... Args>
    gs_vector(const gs_vector<Type, OtherSize>& _Other, Args... _Args) 
    {
        int i = 0;

        for (i = 0; i < gs_min(OtherSize, Size); i++)
            this->Data[i] = _Other[i];

        if(i < Size)
            recursive_template_vector_initialization(static_cast<int>(i), static_cast<Type>(_Args)...);
    }

    template <typename... Args>
    gs_vector(Args... _Args) 
    {
        GS_ASSERT(sizeof...(Args) <= Size);
        recursive_template_vector_initialization(static_cast<int>(0), static_cast<Type>(_Args)...);
    }

    const int size() const
    {
        return Size;
    }

    // &[]
    Type& operator[](const int& _Index)
    {
        GS_ASSERT(_Index < Size);
        return this->Data[_Index];
    }

    // const Type[]&
    const Type& operator[](const int& _Index) const
    {
        GS_ASSERT(_Index < Size);
        return this->Data[_Index];
    }

    // +=
    gs_vector<Type, Size> operator+=(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] += _Value;
        return *this;
    }

    gs_vector<Type, Size> operator+=(const gs_vector<Type, Size>& _Value)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] += _Value[i];
        return *this;
    }

    // -=
    gs_vector<Type, Size> operator-=(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] -= _Value;
        return *this;
    }

    gs_vector<Type, Size> operator-=(const gs_vector<Type, Size>& _Value)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] -= _Value[i];
        return *this;
    }

    // *=
    gs_vector<Type, Size> operator*=(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] *= _Value;
        return *this;
    }

    gs_vector<Type, Size> operator*=(const gs_vector<Type, Size>& _Value)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] *= _Value[i];
        return *this;
    }

    // /=
    gs_vector<Type, Size> operator/=(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] /= _Value;
        return *this;
    }

    gs_vector<Type, Size> operator/=(const gs_vector<Type, Size>& _Value)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] /= _Value[i];
        return *this;
    }

    // =
    gs_vector<Type, Size>& operator=(const gs_vector<Type, Size>& _Other)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] = _Other[i];
        return *this;
    }

    template<int OtherSize>
    gs_vector<Type, Size>& operator=(const gs_vector<Type, OtherSize>& _Other)
    {
        for (int i = 0; i < gs_min(Size, OtherSize); i++)
            this->Data[i] = _Other[i];
        return *this;
    }

private:

    // service methods
    template<typename ... Args>
    void recursive_template_vector_initialization();

    template<typename... Tail>
    void recursive_template_vector_initialization(const int& _Index, const Type& _Head, Tail... _Tail) 
    {
        this->Data[_Index] = _Head;
        recursive_template_vector_initialization(_Index + 1, static_cast<Type>(_Tail)...);
    }

    void recursive_template_vector_initialization(const int& _Index, const Type& _Head)
    {
        this->Data[_Index] = _Head;
    }

    void recursive_template_vector_initialization(const int&){}
};

template<typename Type>
inline double gs_sum_of_squares(Type _A)
{
    return _A * _A;
}

template<typename Type, typename ... Args>
inline double gs_sum_of_squares(Type _A, Args... _Args)
{
    return gs_sum_of_squares(_A) + gs_sum_of_squares(_Args ...);
}

template<typename Type, int Size>
inline double gs_sum_of_squares(const gs_vector<Type, Size>& _Vector)
{
    double sumOfSquares = 0;
    for (int i = 0; i < _Vector.size(); ++i)
        sumOfSquares += _Vector[i] * _Vector[i];
    return sumOfSquares;
}

template<typename Type, typename ... Args>
inline double gs_vector_length(Type _A, Type _B, Args... _Args)
{
    double sumOfSquares = gs_sum_of_squares(_A, _B, _Args ...);
    return sumOfSquares > 0 ? sqrt(sumOfSquares) : 0;
}

template<typename Type, int Size>
inline double gs_vector_length(const gs_vector<Type, Size>& _Vector)
{
    double sumOfSquares = gs_sum_of_squares<Type, Size>(_Vector);
    return sumOfSquares > 0 ? sqrt(sumOfSquares) : 0;
}

template<typename Type, int Size>
inline gs_vector<Type, Size> gs_vector_normalize(const gs_vector<Type, Size>& _Vector)
{
    gs_vector<Type, Size> result(static_cast<Type>(0));
    const Type length = static_cast<Type>(gs_vector_length(_Vector));

    if(length < gs_epsilon<Type>()) 
    {
        result[0] = static_cast<Type>(static_cast<Type>(1));
        return result;
    }

    const Type inverseLength = static_cast<Type>(1) / static_cast<Type>(length);
    for (int i = 0; i < Size; i++)
        result[i] = _Vector[i] * inverseLength;
    return result;
}

template<typename Type, int Size>
inline double gs_vector_argument(const gs_vector<Type, Size>& _Vector)
{
    gs_vector<Type, Size> normalized = gs_vector_normalize(_Vector);
    return atan2(normalized.y, normalized.x);
}

template<typename Type, int Size>
inline Type gs_vectors_dot(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    Type dot = 0;
    for (int i = 0; i < Size; i++)
        dot += _A[i] * _B[i];
    return dot;
}

// cross product of 2D vectors returns the scalar value equal
// to the area of the parallelogram formed by two input vectors
template<typename Type>
inline Type gs_vector_cross(const gs_vector<Type, 2>& _A, const gs_vector<Type, 2> _B)
{
    const Type Ax = _A[0];
    const Type Ay = _A[1];
    const Type Bx = _B[0];
    const Type By = _B[1];
    return Ax * By - Ay * Bx;
}

// cross product of 3D vectors returns the vector perpedicular multiplied vectors and the length
// of resulting vector is equal to the area of the parallelogram formed by two input vectors
template<typename Type>
inline gs_vector<Type, 3> gs_vector_cross(const gs_vector<Type, 3>& _A, const gs_vector<Type, 3>& _B)
{
    const Type Ax = _A.x;
    const Type Ay = _A.y;
    const Type Az = _A.z;

    const Type Bx = _B.x;
    const Type By = _B.y;
    const Type Bz = _B.z;
    return gs_vector<Type, 3>(Ay * Bz - By * Az, Az * Bx - Bz * Ax, Ax * By - Bx * Ay);
}

// Function to check if a point is inside a polygon using
// the ray-casting algorithm
template<typename Type>
int gs_point_in_2D_polygon(const gs_vector<Type, 2>* _Polygon, const int _VertexesCount, const gs_vector<Type, 2>& _Point)
{
    int i, j, c = 0;

    for (i = 0, j = _VertexesCount-1; i < _VertexesCount; j = i++) 
    {
        if(((_Polygon[i].y > _Point.y) != (_Polygon[j].y > _Point.y)) &&
            (_Point.x < (_Polygon[j].x - _Polygon[i].x) * (_Point.y - _Polygon[i].y) / (_Polygon[j].y-_Polygon[i].y) + _Polygon[i].x)) c = !c;
    }

    return c;
}

template<typename Type>
gs_vector<Type, 2> gs_vector_convert_to_NDC(const gs_vector<Type, 2>& _Position, const gs_vector<Type, 2>& _Screen)
{
    return gs_vector<Type, 2>(
        (2.0f * _Position.x) / _Screen.x - 1.0f,
        1.0f - (2.0f * _Position.y) / _Screen.y);
}

template<typename Type, int Size>
inline gs_vector<Type, Size> gs_min(
    const gs_vector<Type, Size>& _A,
    const gs_vector<Type, Size>& _B)
{
    gs_vector<Type, Size> Vector;
    for (int i = 0; i < Size; ++i)
        Vector[i] = gs_min(_A[i], _B[i]);
    return Vector;
}

template<typename Type, int Size>
inline gs_vector<Type, Size> gs_max(
    const gs_vector<Type, Size>& _A,
    const gs_vector<Type, Size>& _B)
{
    gs_vector<Type, Size> Vector;
    for (int i = 0; i < Size; ++i)
        Vector[i] = gs_max(_A[i], _B[i]);
    return Vector;
}

template<typename Type, int Size>
inline gs_vector<Type, Size> gs_clamp(
    const gs_vector<Type, Size>& _Value,
    const gs_vector<Type, Size>& _Min,
    const gs_vector<Type, Size>& _Max)
{
    gs_vector<Type, Size> Vector;
    for (int i = 0; i < Size; ++i)
        Vector[i] = gs_clamp(_Value[i], _Min[i], _Max[i]);
    return Vector;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// [MATRIX]
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
template<typename Type, int Rows, int Columns>
struct gs_matrix final
{
    typedef Type value_type;

    gs_matrix(const Type& _Value = static_cast<Type>(0))
    {
        for (int i = 0; i < Columns; ++i)
            Data[i * Columns + i] = _Value;
    }

    gs_matrix(const gs_matrix<Type, Rows, Columns>& _Matrix)
    {
        for (int i = 0; i < Size; ++i)
            Data[i] = _Matrix.Data[i]; 
    }

    int rows() const
    {
        return Rows;
    }

    int columns() const
    {
        return Columns;
    }

    Type* operator[](const int& _Column)
    {
        GS_ASSERT(_Column < Columns);
        return &Data[_Column * Rows];
    }

    const Type* operator[](const int& _Column) const
    {
        GS_ASSERT(_Column < Columns);
        return &Data[_Column * Rows];
    }

    // +=
    gs_matrix<Type, Rows, Columns> operator+=(const gs_matrix<Type, Rows, Columns>& _Mat)
    {
        gs_matrix<Type, Rows, Columns> result;
        add_mat(*this, _Mat, result);
        
        for (int i = 0; i < Size; i++)
            Data[i] = result.Data[i];
        
        return *this;
    }

    // -=
    gs_matrix<Type, Rows, Columns> operator-=(const gs_matrix<Type, Rows, Columns>& _Mat)
    {
        gs_matrix<Type, Rows, Columns> result;
        sub_mat(*this, _Mat, result);

        for (int i = 0; i < Size; i++)
            Data[i] = result.Data[i];

        return *this;
    }

    // *=
    template<int Dimention>
    gs_matrix<Type, Rows, Dimention> operator*=(const gs_matrix<Type, Columns, Dimention>& _Mat)
    {
        gs_matrix<Type, Rows, Dimention> result;
        mul_mat(*this, _Mat, result);

        for (int i = 0; i < Size; i++)
            Data[i] = result.Data[i];

        return *this;
    }

    // =
    gs_matrix<Type, Rows, Columns>& operator=(const gs_matrix<Type, Rows, Columns>& _Matrix)
    {
        for (int i = 0; i < Size; ++i)
            Data[i] = _Matrix.Data[i];
        return *this;
    }

private:

    // friends
    template<typename T, int R, int C>
    friend bool operator!=(const gs_matrix<T, R, C>& _A, const gs_matrix<T, R, C>& _B);

    template<typename T, int R, int C>
    friend bool operator==(const gs_matrix<T, R, C>& _A, const gs_matrix<T, R, C>& _B);

    template<typename T, int R, int C>
    friend gs_matrix<T, R, C> operator*(const gs_matrix<T, R, C>& _A, const gs_matrix<T, R, C>& _B);

    template<typename T, int R, int C>
    friend gs_vector<T, R> operator*(const gs_matrix<T, R, C>& _A, const gs_vector<T, R>& _V);

    template<typename T, int R, int C>
    friend gs_matrix<T, R, C> operator+(const gs_matrix<T, R, C>& _A, const gs_matrix<T, R, C>& _B);

    template<typename T, int R, int C>
    friend gs_matrix<T, R, C> operator-(const gs_matrix<T, R, C>& _A, const gs_matrix<T, R, C>& _B);

    Type Data[Rows * Columns]{};
    int  Size{Rows * Columns};

    // service methods
    void add_mat(
        const gs_matrix<Type, Rows, Columns>& _A,
        const gs_matrix<Type, Rows, Columns>& _B,
        gs_matrix<Type, Rows, Columns>&       _C) const
    {
        for (int i = 0; i < Size; i++)
            _C.Data[i] = _A.Data[i] + _B.Data[i];
    }

    void sub_mat(
        const gs_matrix<Type, Rows, Columns>& _A,
        const gs_matrix<Type, Rows, Columns>& _B,
        gs_matrix<Type, Rows, Columns>&       _C) const
    {
        for (int i = 0; i < Size; i++)
            _C.Data[i] = _A.Data[i] - _B.Data[i];
    }

    template<int Dimention>
    void mul_mat(
        const gs_matrix<Type, Rows, Columns>&      _A,
        const gs_matrix<Type, Columns, Dimention>& _B,
        gs_matrix<Type, Rows, Dimention>&          _C) const
    {
        GS_ASSERT(_A.columns() == _B.rows());
        GS_ASSERT(_C.columns() == _B.columns());

        for (int i = 0; i < Dimention; i++)
        {
            for (int j = 0; j < Columns; j++)
            {
                for (int k = 0; k < Rows; k++)
                {
                    _C[i][k] += _A[j][k] * _B[i][j];
                }
            }
        }
    }

    void mul_vec(
        const gs_matrix<Type, Rows, Columns>& _Matrix,
        const gs_vector<Type, Rows>&          _Vector,
        gs_vector<Type, Rows>&                _Result) const
    {
        for (int i = 0; i < Columns; i++)
        {
            for (int j = 0; j < Rows; j++)
            {
                _Result[j] += _Matrix[i][j] * _Vector[i];
            }
        }
    }
};

template<typename Type, int Rows, int Columns>
inline gs_matrix<Type, Rows, Columns> gs_matrix_transpose(const gs_matrix<Type, Rows, Columns>& _Matrix)
{
    gs_matrix<Type, Columns, Rows> transposed(0);

    for (int i = 0; i < Columns; i++)
    {
        for (int j = 0; j < Rows; j++)
            transposed[i][j] = _Matrix[j][i];
    }
    
    return transposed;
}

template<typename Type, int Size>
auto gs_matrix_factor_square(const gs_matrix<Type, Size, Size>& _Matrix)
{
    struct
    {
        gs_matrix<Type, Size, Size> Matrix;
        gs_vector<int, Size>        InverseRowsPermutations;
    } result = {_Matrix, gs_vector<int, Size>(0)};

    for(int i = 0; i < Size; i++)
    {
        // initialize permutations vector
        result.InverseRowsPermutations[i] = i;

        // search pivot within the column
        Type vmax = result.Matrix[i][i];
        int  imax = i;

        for(int j = i + 1; j < Size; j++ )
        {
            Type temp = gs_abs(result.Matrix[i][j]);

            if(temp > vmax)
            {
                vmax = temp;
                imax = j;
            }
        }

        if(gs_abs(vmax) <= 0)
            continue;

        // intercnange rows
        result.InverseRowsPermutations[i] = imax;

        if(i != imax)
        {
            for(int j = 0 ; j < Size; j++)
                gs_swap(result.Matrix[j][i], result.Matrix[j][imax]);
        }

        // scale column:  A(j,i) = A(j,i) /  A(i,i)
        for(int j = i + 1; j < Size; j++)
            result.Matrix[i][j] /= result.Matrix[i][i];

        // compute Schur complement: A(k,j) = A(k,j) - A(i,j) * A(k,i)
        for(int j = i + 1; j < Size; j++)
        {
            Type temp = result.Matrix[j][i];

            for(int k = i + 1; k < Size; k++)
                result.Matrix[j][k] -= temp * result.Matrix[i][k];
        }
    }

    return result;
}

template<typename Type, int Size, int Dimention>
gs_matrix<Type, Size, Dimention> gs_matrix_solve_square(
    const gs_matrix<Type, Size, Size>&      _Matrix,
    const gs_matrix<Type, Size, Dimention>& _RightHandSide)
{
    // get ready
    gs_matrix<Type, Size, Dimention> solution = _RightHandSide;

    // A(p, :) = L * U, where:
    // p - inverse rows permutations vector, i.e we need to swap A[p[i], :] and A[i, :]
    // L - lower triangular matrix
    // U - upper triangular matrix
    auto factorization = gs_matrix_factor_square(_Matrix);

    // solve L * U = b(p)
    for (int k = 0; k < Dimention; ++k)
    {
        // permute solution rows
        for (int i = 0; i < Size; ++i)
            gs_swap(solution[k][i], solution[k][factorization.InverseRowsPermutations[i]]);
        
        // solve using lower triangular matrix
        for (int i = 0; i < Size; ++i)
        {
            for (int j = Size - 1; j > i; --j)
                solution[k][j] -= solution[k][i] * factorization.Matrix[i][j];
        }
        
        // solve using upper triangular matrix
        for (int i = Size - 1; i >= 0; --i)
        {
            solution[k][i] /= factorization.Matrix[i][i];
            for (int j = 0; j < i; ++j)
                solution[k][j] -= solution[k][i] * factorization.Matrix[i][j];
        }
    }

    return solution;
}

template<typename Type, int Size>
gs_matrix<Type, Size, Size> gs_matrix_invert_square(const gs_matrix<Type, Size, Size>& _Matrix)
{
    gs_matrix<Type, Size, Size> eye(0);
    for (int i = 0; i < Size; i++)
        eye[i][i] = 1.0;

    return gs_matrix_solve_square(_Matrix, eye);
}

template<typename Type>
inline gs_matrix<Type, 4, 4> gs_matrix_scale(const gs_matrix<Type, 4, 4>& _Matrix, const gs_vector<Type, 3>& _Transform)
{
    gs_matrix<Type, 4, 4> transform(1);
    transform[0][0] = _Transform[0];
    transform[1][1] = _Transform[1];
    transform[2][2] = _Transform[2];
    return _Matrix * transform;
}

template<typename Type>
inline gs_matrix<Type, 4, 4> gs_matrix_translate(const gs_matrix<Type, 4, 4>& _Matrix, const gs_vector<Type, 3>& _Transform)
{
    gs_matrix<Type, 4, 4> transform(1);
    transform[3][0] = _Transform[0];
    transform[3][1] = _Transform[1];
    transform[3][2] = _Transform[2];
    return _Matrix * transform;
}

template<typename Type>
inline gs_matrix<Type, 4, 4> gs_matrix_rotate(const gs_matrix<Type, 4, 4>& _Matrix, const Type& _Angle, const gs_vector<Type, 3>& _Axis)
{
    Type const a = _Angle;
    Type const c = cos(a);
    Type const s = sin(a);

    gs_vector<Type, 3> axis(gs_vector_normalize(_Axis));
    gs_vector<Type, 3> temp(axis * (static_cast<Type>(1) - c));

    gs_matrix<Type, 4, 4> transform(1.f);
    transform[0][0] = c + temp[0] * axis[0];
    transform[0][1] = temp[0] * axis[1] + s * axis[2];
    transform[0][2] = temp[0] * axis[2] - s * axis[1];

    transform[1][0] = temp[1] * axis[0] - s * axis[2];
    transform[1][1] = c + temp[1] * axis[1];
    transform[1][2] = temp[1] * axis[2] + s * axis[0];

    transform[2][0] = temp[2] * axis[0] + s * axis[1];
    transform[2][1] = temp[2] * axis[1] - s * axis[0];
    transform[2][2] = c + temp[2] * axis[2];

    return _Matrix * transform;
}

template<typename Type>
inline gs_vector<Type, 3> gs_matrix_retrieve_transform_translation_vector(const gs_matrix<Type, 4, 4>& _Matrix)
{
    return {_Matrix[3][0], _Matrix[3][1], _Matrix[3][2]};
}


//------------------------------------------------------------------
// ortho
//------------------------------------------------------------------
template<typename T>
inline gs_matrix<T, 4, 4> gs_matrix_ortho(
    const T& left,
    const T& right,
    const T& bottom,
    const T& top,
    const T& zNear,
    const T& zFar,
    const bool& RH = true,
    const bool& NO = true)
{
    // auxiliary lambdas
    auto gs_matrix_ortho_lh_zo = [](
        const T& left,
        const T& right,
        const T& bottom,
        const T& top,
        const T& zNear,
        const T& zFar)->gs_matrix<T, 4, 4>
    {
        gs_matrix<T, 4, 4> Result(1);
        Result[0][0] = static_cast<T>(2) / (right - left);
        Result[1][1] = static_cast<T>(2) / (top - bottom);
        Result[2][2] = static_cast<T>(1) / (zFar - zNear);
        Result[3][0] = - (right + left) / (right - left);
        Result[3][1] = - (top + bottom) / (top - bottom);
        Result[3][2] = - zNear / (zFar - zNear);
        return Result;
    };

    auto gs_matrix_ortho_lh_no = [](
        const T& left,
        const T& right,
        const T& bottom,
        const T& top,
        const T& zNear,
        const T& zFar)->gs_matrix<T, 4, 4>
    {
        gs_matrix<T, 4, 4> Result(1);
        Result[0][0] = static_cast<T>(2) / (right - left);
        Result[1][1] = static_cast<T>(2) / (top - bottom);
        Result[2][2] = static_cast<T>(2) / (zFar - zNear);
        Result[3][0] = - (right + left) / (right - left);
        Result[3][1] = - (top + bottom) / (top - bottom);
        Result[3][2] = - (zFar + zNear) / (zFar - zNear);
        return Result;
    };

    auto gs_matrix_ortho_rh_zo = [](
        const T& left,
        const T& right,
        const T& bottom,
        const T& top,
        const T& zNear,
        const T& zFar)->gs_matrix<T, 4, 4>
    {
        gs_matrix<T, 4, 4> Result(1);
        Result[0][0] = static_cast<T>(2) / (right - left);
        Result[1][1] = static_cast<T>(2) / (top - bottom);
        Result[2][2] = - static_cast<T>(1) / (zFar - zNear);
        Result[3][0] = - (right + left) / (right - left);
        Result[3][1] = - (top + bottom) / (top - bottom);
        Result[3][2] = - zNear / (zFar - zNear);
        return Result;
    };

    auto gs_matrix_ortho_rh_no = [](
        const T& left,
        const T& right,
        const T& bottom,
        const T& top,
        const T& zNear,
        const T& zFar)->gs_matrix<T, 4, 4>
    {
        gs_matrix<T, 4, 4> Result(1);
        Result[0][0] = static_cast<T>(2) / (right - left);
        Result[1][1] = static_cast<T>(2) / (top - bottom);
        Result[2][2] = - static_cast<T>(2) / (zFar - zNear);
        Result[3][0] = - (right + left) / (right - left);
        Result[3][1] = - (top + bottom) / (top - bottom);
        Result[3][2] = - (zFar + zNear) / (zFar - zNear);
        return Result;
    };

    // right hand
    if(RH)
    {
        return NO ?
            gs_matrix_ortho_rh_no(left, right, bottom, top, zNear, zFar) :
                gs_matrix_ortho_rh_zo(left, right, bottom, top, zNear, zFar);
    }

    // left hand
    return NO ?
        gs_matrix_ortho_lh_no(left, right, bottom, top, zNear, zFar) :
            gs_matrix_ortho_lh_zo(left, right, bottom, top, zNear, zFar);
}

//------------------------------------------------------------------
// look at
//------------------------------------------------------------------
template<typename Type>
inline gs_matrix<Type, 4, 4> gs_matrix_look_at(
    const gs_vector<Type, 3>& eye,
    const gs_vector<Type, 3>& center,
    const gs_vector<Type, 3>& up,
    const bool&               RH = true)
{
    auto gs_matrix_look_at_rh = [](
        const gs_vector<Type, 3>& eye,
        const gs_vector<Type, 3>& center,
        const gs_vector<Type, 3>& up)->gs_matrix<Type, 4, 4>
    {
        gs_vector<Type, 3> const f(gs_vector_normalize(center - eye));
        gs_vector<Type, 3> const s(gs_vector_normalize(gs_vector_cross(f, up)));
        gs_vector<Type, 3> const u(gs_vector_cross(s, f));

        gs_matrix<Type, 4, 4> Result(1);
        Result[0][0] = s.x;
        Result[1][0] = s.y;
        Result[2][0] = s.z;
        Result[0][1] = u.x;
        Result[1][1] = u.y;
        Result[2][1] = u.z;
        Result[0][2] =-f.x;
        Result[1][2] =-f.y;
        Result[2][2] =-f.z;
        Result[3][0] =-gs_vectors_dot(s, eye);
        Result[3][1] =-gs_vectors_dot(u, eye);
        Result[3][2] = gs_vectors_dot(f, eye);
        return Result;
    };

    auto gs_matrix_look_at_lh = [](
        const gs_vector<Type, 3>& eye,
        const gs_vector<Type, 3>& center,
        const gs_vector<Type, 3>& up)->gs_matrix<Type, 4, 4>
    {
		gs_vector<Type, 3> const f(gs_vector_normalize(center - eye));
		gs_vector<Type, 3> const s(gs_vector_normalize(gs_vector_cross(up, f)));
		gs_vector<Type, 3> const u(gs_vector_cross(f, s));

		gs_matrix<Type, 4, 4> Result(1);
		Result[0][0] = s.x;
		Result[1][0] = s.y;
		Result[2][0] = s.z;
		Result[0][1] = u.x;
		Result[1][1] = u.y;
		Result[2][1] = u.z;
		Result[0][2] = f.x;
		Result[1][2] = f.y;
		Result[2][2] = f.z;
		Result[3][0] = -gs_vectors_dot(s, eye);
		Result[3][1] = -gs_vectors_dot(u, eye);
		Result[3][2] = -gs_vectors_dot(f, eye);
        return Result;
    };

    return RH ? gs_matrix_look_at_rh(eye, center, up) : gs_matrix_look_at_lh(eye, center, up);
}

//------------------------------------------------------------------
// prespective
//------------------------------------------------------------------
template<typename Type>
inline gs_matrix<Type, 4, 4> gs_matrix_perspective(
    const Type& fovy,
    const Type& aspect,
    const Type& zNear,
    const Type& zFar,
    const bool& RH = true,
    const bool& NO = true)
{
    // auxiliary lambdas
    auto gs_matrix_perspective_rh_zo = [](
        const Type& fovy,
        const Type& aspect,
        const Type& zNear,
        const Type& zFar)->gs_matrix<Type, 4, 4>
    {
        GS_ASSERT(gs_abs(aspect - gs_epsilon<Type>()) > static_cast<Type>(0));

        Type const tanHalfFovy = tan(fovy / static_cast<Type>(2));

		gs_matrix<Type, 4, 4> Result(static_cast<Type>(0));
		Result[0][0] = static_cast<Type>(1) / (aspect * tanHalfFovy);
		Result[1][1] = static_cast<Type>(1) / (tanHalfFovy);
		Result[2][2] = zFar / (zNear - zFar);
		Result[2][3] = - static_cast<Type>(1);
		Result[3][2] = -(zFar * zNear) / (zFar - zNear);
        return Result;
    };

    auto gs_matrix_perspective_rh_no = [](
        const Type& fovy,
        const Type& aspect,
        const Type& zNear,
        const Type& zFar)->gs_matrix<Type, 4, 4>
    {
        GS_ASSERT(gs_abs(aspect - gs_epsilon<Type>()) > static_cast<Type>(0));

        Type const tanHalfFovy = tan(fovy / static_cast<Type>(2));

        gs_matrix<Type, 4, 4> Result(static_cast<Type>(0));
        Result[0][0] = static_cast<Type>(1) / (aspect * tanHalfFovy);
        Result[1][1] = static_cast<Type>(1) / (tanHalfFovy);
        Result[2][2] = - (zFar + zNear) / (zFar - zNear);
        Result[2][3] = - static_cast<Type>(1);
        Result[3][2] = - (static_cast<Type>(2) * zFar * zNear) / (zFar - zNear);
        return Result;
    };

    auto gs_matrix_perspective_lh_zo = [](
        const Type& fovy,
        const Type& aspect,
        const Type& zNear,
        const Type& zFar)->gs_matrix<Type, 4, 4>
    {
        GS_ASSERT(gs_abs(aspect - gs_epsilon<Type>()) > static_cast<Type>(0));

        Type const tanHalfFovy = tan(fovy / static_cast<Type>(2));

		gs_matrix<Type, 4, 4> Result(static_cast<Type>(0));
		Result[0][0] = static_cast<Type>(1) / (aspect * tanHalfFovy);
		Result[1][1] = static_cast<Type>(1) / (tanHalfFovy);
		Result[2][2] = zFar / (zFar - zNear);
		Result[2][3] = static_cast<Type>(1);
		Result[3][2] = -(zFar * zNear) / (zFar - zNear);
        return Result;
    };

    auto gs_matrix_perspective_lh_no = [](
        const Type& fovy,
        const Type& aspect,
        const Type& zNear,
        const Type& zFar)->gs_matrix<Type, 4, 4>
    {
        GS_ASSERT(gs_abs(aspect - gs_epsilon<Type>()) > static_cast<Type>(0));

        Type const tanHalfFovy = tan(fovy / static_cast<Type>(2));

		gs_matrix<Type, 4, 4> Result(static_cast<Type>(0));
		Result[0][0] = static_cast<Type>(1) / (aspect * tanHalfFovy);
		Result[1][1] = static_cast<Type>(1) / (tanHalfFovy);
		Result[2][2] = (zFar + zNear) / (zFar - zNear);
		Result[2][3] = static_cast<Type>(1);
		Result[3][2] = - (static_cast<Type>(2) * zFar * zNear) / (zFar - zNear);
        return Result;
    };

    // right hand
    if(RH)
    {
        return NO ?
            gs_matrix_perspective_rh_no(fovy, aspect, zNear, zFar) :
                gs_matrix_perspective_rh_zo(fovy, aspect, zNear, zFar);
    }

    // left hand
    return NO ?
        gs_matrix_perspective_lh_no(fovy, aspect, zNear, zFar) :
            gs_matrix_perspective_lh_zo(fovy, aspect, zNear, zFar);
}

// template<typename Type>
// auto gs_matrix_calculate_2d_camera_view_and_projection(
//     const gs_vector<Type, 2>& _CameraWorldPosition,
//     const gs_vector<Type, 3>& _CameraWorldUpAxisDirection,
//     const gs_vector<Type, 3>& _CameraWorldFrontAxisDirection,
//     const gs_vector<Type, 2>& _CameraResolution,
//     const float&              _CameraRotationAngle,
//     const Type&               _CameraNearPlanePosition,
//     const Type&               _CameraFarPlanePosition)
// {
//     // compute projection matrix
//     float left   = -_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
//     float right  = +_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
//     float bottom = -_CameraResolution.y * 0.5f + _CameraWorldPosition.y;
//     float top    = +_CameraResolution.y * 0.5f + _CameraWorldPosition.y;

//     // camera orientation
//     gs_vector<Type, 3> cameraLocalFrontAxisDirection = gs_vector_normalize(_CameraWorldFrontAxisDirection);
//     gs_vector<Type, 3> cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, _CameraWorldUpAxisDirection));
//     gs_vector<Type, 3> cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vector_cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

//     gs_matrix<Type, 4, 4> cameraview =
//         gs_matrix_look_at(
//             gs_vector<Type, 3>(0.f, 0.f, 1),
//             gs_vector<Type, 3>(0.f, 0.f, 1) + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection);
    
//     gs_matrix<Type, 4, 4> projection =
//         gs_matrix_ortho(
//             left,
//             right,
//             bottom,
//             top,
//             _CameraNearPlanePosition,
//             _CameraFarPlanePosition) * gs_matrix_rotate(gs_matrix<Type, 4, 4>(1.f), gs_to_radians(_CameraRotationAngle), gs_vector<Type, 3>(0.f, 0.f, 1.f));

//     struct
//     {
//         gs_matrix<Type, 4, 4> cameraview;
//         gs_matrix<Type, 4, 4> projection;
//     } result = {cameraview, projection};

//     return result;
// }

// TODO: implement 3D perspective camera when it's needed
// template<typename Type>
// auto gs_matrix_calculate_perspective_camera_view_and_projection(
//     const gs_vector<Type, 3>& _CameraWorldPosition,
//     const gs_vector<Type, 3>& _CameraWorldUpAxisDirection,
//     const gs_vector<Type, 3>& _CameraWorldFrontAxisDirection,
//     const gs_vector<Type, 2>& _CameraResolution,
//     const gs_vector<Type, 3>& _CameraEulerAngles,
//     const Type&               _CameraNearPlanePosition,
//     const Type&               _CameraFarPlanePosition,
//     const Type&               _FieldOfView = 90,
//     const Type&               _Aspect      = 1,
//     const float&              _Depth       = 10000)
// {
//     // camera rotation angles
//     // gs_mat4f rotateX  = gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(0.f), gs_vec3f(1.f, 0.f, 0.f));
//     // gs_mat4f rotateY  = gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(0.f), gs_vec3f(0.f, 1.f, 0.f));
//     gs_mat4f rotateZ  = gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(0.f), gs_vec3f(0.f, 0.f, 1.f));

//     // camera local attributes
//     gs_vec3f cameraWorldUpAxisDirection    = gs_vec3f(+0.f, +1.f, +0.f);
//     gs_vec3f cameraLocalFrontAxisDirection = gs_vector_normalize(_CameraWorldFrontAxisDirection);
//     gs_vec3f cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, cameraWorldUpAxisDirection));
//     gs_vec3f cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vector_cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

//     // rotate around Z axis
//     cameraLocalFrontAxisDirection = gs_vector_normalize(gs_vec3f(rotateZ * gs_vec4f(cameraLocalFrontAxisDirection, 1.f)));
//     cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vec3f(rotateZ * gs_vec4f(cameraWorldUpAxisDirection, 1.f)));

//     // setup projection matrixes
//     gs_mat4f scaleMatrix = gs_matrix_scale(
//         gs_mat4f(1.f), 
//         gs_vec3f(
//             1.f / std::max<float>(_CameraResolution.x, 1.f), 
//             1.f / std::max<float>(_CameraResolution.y, 1.f), 
//             1.f / _Depth
//         )
//     );

//     gs_vec3f cameraWorldPosition = gs_vec3f(+0.0f, -0.0f, +1.f); // scaleMatrix * gs_vec4f(_CameraWorldPosition, 1.f);

//     gs_mat4f cameraview = gs_matrix_look_at(cameraWorldPosition, cameraWorldPosition + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection) * scaleMatrix;
//     gs_mat4f projection = gs_matrix_perspective(
//         gs_to_radians(_FieldOfView),
//         1.f,
//         _CameraNearPlanePosition / _Depth,
//         _CameraFarPlanePosition / _Depth);

//     struct
//     {
//         gs_matrix<Type, 4, 4> cameraview;
//         gs_matrix<Type, 4, 4> projection;
//     } result = {cameraview, projection};

//     return result;
// }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// [RECT]
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
template<typename Type>
struct gs_2d_ellipse
{
    gs_2d_ellipse(const gs_vector<Type, 2>& _Center, const Type& _Radius) : Center(_Center), Radius(_Radius){}

    gs_vector<Type, 2> Center;
    Type               Radius;

    bool contains(const gs_vector<Type, 2>& _Point) const
    {
        return gs_vector_length(_Point - Center) < Radius;
    }

    gs_2d_ellipse<Type> transform(const gs_matrix<Type, 4, 4>& _Transform)
    {
        return gs_2d_ellipse<Type>(
            _Transform * gs_vector<Type, 4>(Center, 0.f, 1.f),
            Radius
        );
    }
};

template<typename Type>
struct gs_2dbox
{
    gs_2dbox() : Min(gs_vector<Type, 2>(0.f)), Max(gs_vector<Type, 2>(0.f)){}

    template<typename ... Args>
    gs_2dbox(const gs_vector<Type, 2>& _A, const gs_vector<Type, 2>& _B, Args ... _Args)
    {
        Min = gs_vector<Type, 2>(gs_min(_A.x, _B.x, static_cast<gs_vector<Type, 2>>(_Args).x...), gs_min(_A.y, _B.y, static_cast<gs_vector<Type, 2>>(_Args).y...));
        Max = gs_vector<Type, 2>(gs_max(_A.x, _B.x, static_cast<gs_vector<Type, 2>>(_Args).x...), gs_max(_A.y, _B.y, static_cast<gs_vector<Type, 2>>(_Args).y...));
    }

    gs_vector<Type, 2> Min{gs_vector<Type, 2>(0.f)};
    gs_vector<Type, 2> Max{gs_vector<Type, 2>(0.f)};

    gs_vector<Type, 2> size() const
    {
        return gs_vector<Type, 2>(gs_abs((Max - Min).x), gs_abs((Max - Min).y));
    }

    Type width() const
    {
        return size().x;
    }

    Type height() const
    {
        return size().y;
    }

    gs_vector<Type, 2> center() const
    {
        return (Min + Max) * 0.5f;
    }

    bool contains(const gs_vector<Type, 2>& _Point) const
    {
        return _Point.x >= Min.x &&
               _Point.y >= Min.y &&
               _Point.x <= Max.x &&
               _Point.y <= Max.y;
    }

    bool contains(const gs_2dbox<Type>& _Other) const
    {
        return _Other.Min.x > Min.x &&
               _Other.Min.y > Min.y &&
               _Other.Max.x < Max.x &&
               _Other.Max.y < Max.y;
    }

    gs_2dbox<Type> transform(const gs_matrix<Type, 4, 4>& _Transform) const
    {
        return gs_2dbox<Type>(
            _Transform * gs_vector<Type, 4>(Min, 0.f, 1.f),
            _Transform * gs_vector<Type, 4>(Max, 0.f, 1.f));
    }

    bool  overlaps(const gs_2dbox<Type>& _Other) const
    {
        return gs_abs(clip_with(_Other).width() * clip_with(_Other).height())             > gs_epsilon<Type>() * 2 ||
               gs_abs(_Other.clip_with(*this).width() * _Other.clip_with(*this).height()) > gs_epsilon<Type>() * 2;
    }

    gs_2dbox<Type> clip_with(const gs_2dbox<Type>& _Clipbox) const
    {
        return gs_2dbox<Type>(
            gs_clamp(Min, _Clipbox.Min, _Clipbox.Max),
            gs_clamp(Max, _Clipbox.Min, _Clipbox.Max));
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// [BINARY OPERATORS]
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// [VECTORS VS VECTORS]
template<typename Type, int Size>
bool operator!=(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    bool value = false;
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        value |= _A[i] != _B[i];
    return value;
}

template<typename Type, int Size>
bool operator==(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    bool value = true;
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        value &= _A[i] == _B[i];
    return value;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator+(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] + _B[i];
    return _C;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator-(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] - _B[i];
    return _C;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator*(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] * _B[i];
    return _C;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator/(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] / _B[i];
    return _C;
}

// [VECTORS VS SCALARS]
template<typename Type, int Size>
gs_vector<Type, Size> operator+(const gs_vector<Type, Size>& _A, const Type& _B)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] + _B;
    return _C;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator-(const gs_vector<Type, Size>& _A, const Type& _B)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] - _B;
    return _C;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator*(const gs_vector<Type, Size>& _A, const Type& _B)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] * _B;
    return _C;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator/(const gs_vector<Type, Size>& _A, const Type& _B)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] / _B;
    return _C;
}

// [SCALARS VS VECTORS]
template<typename Type, int Size>
gs_vector<Type, Size> operator+(const Type& _B, const gs_vector<Type, Size>& _A)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] + _B;
    return _C;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator-(const Type& _B, const gs_vector<Type, Size>& _A)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] - _B;
    return _C;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator*(const Type& _B, const gs_vector<Type, Size>& _A)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] * _B;
    return _C;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator/(const Type& _B, const gs_vector<Type, Size>& _A)
{
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        _C[i] = _A[i] / _B;
    return _C;
}

// [MATRIX VS MATRIX]
template<typename Type, int Rows, int Columns>
bool operator!=(const gs_matrix<Type, Rows, Columns>& _A, const gs_matrix<Type, Rows, Columns>& _B)
{
    bool output = false;
    for (int i = 0; i < Rows * Columns; ++i)
        output |= _A.Data[i] != _B.Data[i];
    return output;
}

template<typename Type, int Rows, int Columns>
bool operator==(const gs_matrix<Type, Rows, Columns>& _A, const gs_matrix<Type, Rows, Columns>& _B)
{
    bool output = true;
    for (int i = 0; i < Rows * Columns; ++i)
        output &= _A.Data[i] == _B.Data[i];
    return output;
}

template<typename Type, int Rows, int Columns>
gs_matrix<Type, Rows, Columns> operator+(const gs_matrix<Type, Rows, Columns>& _A, const gs_matrix<Type, Rows, Columns>& _B)
{
    gs_matrix<Type, Rows, Columns> result;
    _A.add_mat(_A, _B, result);
    return result;
}

template<typename Type, int Rows, int Columns>
gs_matrix<Type, Rows, Columns> operator-(const gs_matrix<Type, Rows, Columns>& _A, const gs_matrix<Type, Rows, Columns>& _B)
{
    gs_matrix<Type, Rows, Columns> result;
    _A.sub_mat(_A, _B, result);
    return result;
}

template<typename Type, int Rows, int Columns>
gs_matrix<Type, Rows, Columns> operator*(const gs_matrix<Type, Rows, Columns>& _A, const gs_matrix<Type, Rows, Columns>& _B)
{
    gs_matrix<Type, Rows, Columns> result;
    _A.mul_mat(_A, _B, result);
    return result;
}

// [MATRIX VS VECTORS]
template<typename Type, int Rows, int Columns>
gs_vector<Type, Rows> operator*(const gs_matrix<Type, Rows, Columns>& _A, const gs_vector<Type, Rows>& _V)
{
    gs_vector<Type, Rows> result(0);
    _A.mul_vec(_A, _V, result);
    return result;
}

// [COMPLEX VS COMPLEX]
template<typename Type>
gs_complex<Type> operator+(const gs_complex<Type>& _Number1, const gs_complex<Type>& _Number2)
{
    auto _Number3 = gs_complex<Type>::complex_add(_Number1.m_data , _Number2.m_data);
    return gs_complex<Type>(_Number3.REAL, _Number3.IMAG);
}

template<typename Type>
gs_complex<Type> operator-(const gs_complex<Type>& _Number1, const gs_complex<Type>& _Number2)
{
    auto _Number3 = gs_complex<Type>::complex_substract(_Number1.m_data, _Number2.m_data);
    return gs_complex<Type>(_Number3.REAL, _Number3.IMAG);
}

template<typename Type>
gs_complex<Type> operator*(const gs_complex<Type>& _Number1, const gs_complex<Type>& _Number2)
{
    auto _Number3 = gs_complex<Type>::complex_multiply(_Number1.m_data, _Number2.m_data);
    return gs_complex<Type>(_Number3.REAL, _Number3.IMAG);
}

template<typename Type>
gs_complex<Type> operator/(const gs_complex<Type>& _Number1, const gs_complex<Type>& _Number2)
{
    auto _Number3 = gs_complex<Type>::complex_divide(_Number1.m_data, _Number2.m_data);
    return gs_complex<Type>(_Number3.REAL, _Number3.IMAG);
}

template<typename Type>
bool operator>(const gs_complex<Type>& _Number1, const gs_complex<Type>& _Number2)
{
    return gs_complex<Type>::complex_greater(_Number1.m_data , _Number2.m_data);
}

template<typename Type>
bool operator>=(const gs_complex<Type>& _Number1, const gs_complex<Type>& _Number2)
{
    return gs_complex<Type>::complex_greater_or_equal(_Number1.m_data, _Number2.m_data);
}

template<typename Type>
bool operator<(const gs_complex<Type>& _Number1, const gs_complex<Type>& _Number2)
{
    return gs_complex<Type>::complex_lower(_Number1.m_data, _Number2.m_data);
}

template<typename Type>
bool operator<=(const gs_complex<Type>& _Number1, const gs_complex<Type>& _Number2)
{
    return gs_complex<Type>::complex_lower_or_equal(_Number1.m_data, _Number2.m_data);
}

template<typename Type>
bool operator==(const gs_complex<Type>& _Number1, const gs_complex<Type>& _Number2)
{
    return gs_complex<Type>::complex_equal(_Number1.m_data, _Number2.m_data);
}

template<typename Type>
bool operator!=(const gs_complex<Type>& _Number1, const gs_complex<Type>& _Number2)
{
    return !gs_complex<Type>::complex_equal(_Number1.m_data, _Number2.m_data);
}

// [COMPLEX VS SCALAR]
template<typename Type>
gs_complex<Type> operator*(const gs_complex<Type>& _Number, const Type& _Value)
{
    return gs_complex<Type>(_Number.m_data.REAL * _Value , _Number.m_data.IMAG * _Value);
}

template<typename Type>
gs_complex<Type> operator*(const Type& _Value, const gs_complex<Type>& _Number)
{
    return gs_complex<Type>(_Number.m_data.REAL * _Value , _Number.m_data.IMAG * _Value);
}

template<typename Type>
gs_complex<Type> operator/(const gs_complex<Type>& _Number, const Type& _Value)
{
    return gs_complex<Type>(_Number.m_data.REAL / _Value, _Number.m_data.IMAG / _Value);
}

template<typename Type>
gs_complex<Type> operator/(const Type& _Value, const gs_complex<Type>& _Number)
{
    auto _Number3 =
        gs_complex<Type>::complex_divide(
            gs_complex<Type>(_Value, static_cast<Type>(0)).m_data,
            _Number.m_data);

    return gs_complex<Type>(_Number3.REAL, _Number3.IMAG);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename Type, int Rows, int Columns>
void gs_print(const gs_matrix<Type, Rows, Columns>& _Matrix)
{
    printf("[");
    for (int i = 0; i < Rows; i++)
    {
        for (int j = 0; j < Columns; j++)
        {
            if(j < Columns - 1)
                printf("%f,\t", static_cast<double>(_Matrix[j][i]));
            else
                printf("%f", static_cast<double>(_Matrix[j][i]));
        }
        
        if(i < Rows - 1)
            printf("\n");
        else
            printf("]\n");
    }
}

template<typename Type, int Size>
void gs_print(const gs_vector<Type, Size>& _Vector)
{
    printf("[");
    for (int i = 0; i < _Vector.size(); i++)
        printf("%f;\t", static_cast<double>(_Vector[i]));
    printf("]\n");
}

// vectors typedefs
typedef gs_vector<float,  2> gs_vec2f;
typedef gs_vector<float,  3> gs_vec3f;
typedef gs_vector<float,  4> gs_vec4f;

typedef gs_vector<double, 2> gs_vec2d;
typedef gs_vector<double, 3> gs_vec3d;
typedef gs_vector<double, 4> gs_vec4d;

typedef gs_vector<int,    2> gs_vec2i;
typedef gs_vector<int,    3> gs_vec3i;
typedef gs_vector<int,    4> gs_vec4i;

typedef gs_vector<unsigned int, 2> gs_vec2ui;
typedef gs_vector<unsigned int, 3> gs_vec3ui;
typedef gs_vector<unsigned int, 4> gs_vec4ui;

// rectangle typedefs
typedef gs_2dbox<float > gs_2dboxf;
typedef gs_2dbox<double> gs_2dboxd;
typedef gs_2dbox<int   > gs_2dboxi;

typedef gs_2d_ellipse<float> gs_2d_ellipsef;

// matrix typedefs
typedef gs_matrix<float,  2, 2> gs_mat2f;
typedef gs_matrix<float,  3, 3> gs_mat3f;
typedef gs_matrix<float,  4, 4> gs_mat4f;
typedef gs_matrix<double, 2, 2> gs_mat2d;
typedef gs_matrix<double, 3, 3> gs_mat3d;
typedef gs_matrix<double, 4, 4> gs_mat4d;
typedef gs_matrix<int,    2, 2> gs_mat2i;
typedef gs_matrix<int,    3, 3> gs_mat3i;
typedef gs_matrix<int,    4, 4> gs_mat4i;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
// [COLORS]
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
typedef unsigned int gs_color;

// RGBA
gs_color gs_color_rgba(const gs_color& _R, const gs_color& _G, const gs_color& _B, const gs_color& _A);

gs_color gs_color_rgb(const gs_color& _R, const gs_color& _G, const gs_color& _B);

gs_color gs_color_rbg_lerp(gs_color& _SourceColor, gs_color& _TargetColor, float& _Fraction);

gs_color gs_color_rgba_get_r(const gs_color& _Color);

gs_color gs_color_rgba_get_g(const gs_color& _Color);

gs_color gs_color_rgba_get_b(const gs_color& _Color);

gs_color gs_color_rgba_get_a(const gs_color& _Color);

// HSV
gs_color gs_color_hsv(const gs_color& _H, const gs_color& _S, const gs_color& _V);

gs_color gs_color_hsv_get_h(const gs_color& _HSV);

gs_color gs_color_hsv_get_s(const gs_color& _HSV);

gs_color gs_color_hsv_get_v(const gs_color& _HSV);

// HSL
gs_color gs_color_hsl(const gs_color& _H, const gs_color& _S, const gs_color& _L);

gs_color gs_color_hsl_get_h(const gs_color& _HSL);

gs_color gs_color_hsl_get_s(const gs_color& _HSL);

gs_color gs_color_hsl_get_l(const gs_color& _HSL);

// HSV/RGBA
gs_color gs_color_rgb_to_hsv(const gs_color& _RGB);

gs_color gs_color_rgb_to_hsl(const gs_color& _RGB);

gs_color gs_color_hsv_to_rgb(const gs_color& _HSV);

gs_color gs_color_hsl_to_rgb(const gs_color& _HSL);

gs_color gs_color_hsv_to_hsl(const gs_color& _HSV);

gs_color gs_color_hsl_to_hsv(const gs_color& _HSL);

// undef all macro
#undef GS_TO_DEGREES_CONVERSION_MULTIPLYER__
#undef GS_TO_RADIANS_CONVERSION_MULTIPLYER__