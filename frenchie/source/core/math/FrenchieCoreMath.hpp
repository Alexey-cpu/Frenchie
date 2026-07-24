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

/*! \defgroup <Core> (Core)
 *  @brief The module contains core utility functions and classes.
    @{
*/

/*! @} */

/*! \defgroup <Math> (Math)
*   @ingroup Core
 *  @brief The module contains core linear algebra utility functions and classes for 2D/3D graphics.
    @{
*/

//------------------------------------------------------------------------------------------------------------------------------------------------
// [UTILITY]
//------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief Function for the maximum value of a given type retrieval
 * 
 * @return returns maximum possible value of an appropriate type
 */
template<typename Type> Type gs_huge();

/**
 * @brief Function for the minimum value of a given type retrieval
 * 
 * @return returns minimum possible value of an appropriate type
 */
template<typename Type> Type gs_tiny();

/**
 * @brief Function for the epsilon value of a given type retrieval
 *
 * @return returns an epsilon value of an appropriate type
 */
template<typename Type> Type gs_epsilon();

/**
 * @brief Clamp function
 * 
 * @param _Value input value
 * @param _Min input value minimum
 * @param _Max input value maximum
 * @return returns value clamped between minimum and maximum
* \f[
*       value = \begin{cases} min, \text{if value <= min} \\ max, \text{if value >= max} \end{cases}
* \f]
 */
template<typename Type>
inline Type gs_clamp(const Type& _Value, const Type& _Min, const Type& _Max)
{
    if(_Value < _Min) return _Min;
    if(_Value > _Max) return _Max;
    return _Value;
}

/**
 * @brief Integer numbner to closest even number round function
 * 
 * @param _Value input value 
 * @return returns input value rounded to closest event number
 */
inline int gs_round_to_even(const int & _Value)
{
    return ((((_Value) + 1) / 2) * 2);
}

/**
 * @brief Radians to degrees convertion function
 * 
 * @param _Angle input angle in radians 
 * @return returns input angle in degrees 
 */
template<typename Type>
inline Type gs_to_degrees(const Type& _Angle)
{
    return _Angle * (Type)GS_TO_DEGREES_CONVERSION_MULTIPLYER__;
}

/**
 * @brief Degrees to radians convertion function
 * 
 * @param _Angle input angle in degrees 
 * @return returns input angle in radians 
 */
template<typename Type>
inline Type gs_to_radians(const Type& _Angle)
{
    return _Angle * (Type)GS_TO_RADIANS_CONVERSION_MULTIPLYER__;
}

/**
 * @brief This function clamps angle between -2PI and +2PI
 * @param _Angle input angle
 * @return retruns input angle clamped between [-2PI, +2PI]
 */
template<typename Type>
Type gs_normalize_angle(const Type& _Angle)
{
    Type angle = _Angle;
    while (angle < 0   ) angle += PI2;
    while (angle >= PI2) angle -= PI2;
    return angle;
};

/**
 * @brief Absolute value function
 * 
 * @param _Value input value
 * @return returns input number absolute value
 */
template<typename Type>
inline Type gs_abs(const Type& _Value)
{
    return _Value < 0 ? -_Value : +_Value;
}

/**
 * @brief Array index circular clamp function
 * @param _Index input index
 * @param _Size  array size
 * @return clamps input index between 0 and _Size
 */
inline int gs_array_index_clamp(const int& _Index, const int& _Size)
{
    int index = _Index;
    while (index < 0     ) index += gs_abs(_Size);
    while (index >= _Size) index -= gs_abs(_Size);
    return index;
}

/**
 * @brief Number sign extraction function
 * 
 * @param _Value input number
 * @return returns input number sign as follows:
* \f[
*       sign = \begin{cases} -1, \text{if value < 0} \\ +1, \text{if value > 0} \\ 0, \text{if value = 0} \end{cases}
* \f]
 */
template<typename Type>
inline Type gs_sign(Type _Value)
{
    if(_Value == static_cast<Type>(0))
        return 0;

    return _Value > 0 ? static_cast<Type>(1) : -static_cast<Type>(1);
}

template<typename Type>
inline Type gs_max(const Type& _A, const Type& _B)
{
    return _A > _B ? _A : _B;
}

/**
 * @brief Maximum number function
 *  
 * @param _A - first number
 * @param _B - second number
 * @param _Args - other optional numbers
 * @return returns maximum number out of several numbers:
* \f[
*       max = max(a, b, ... n)
* \f]
* requiers at least two input numbers.
 */
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

/**
 * @brief Minimum number function
 * 
 * @param _A - first number
 * @param _B - second number
 * @param _Args - other optional numbers
 * @return returns minimum number out of several numbers:
* \f[
*       min = min(a, b, ... n)
* \f]
* requiers at least two input numbers.
 */
template<typename Type, typename ... Args>
inline Type gs_min(const Type& _A, const Type& _B, Args... _Args)
{
    return gs_min(gs_min(_A, _B), _Args...);
}

/**
 * @brief Swaps two values
 *  
 * @param _A first value 
 * @param _B second value
 */
template<typename Type>
inline void gs_swap(Type& _A, Type& _B)
{
    Type _C = _A;
    _A = _B;
    _B = _C;
}

/**
 * @brief Pseudo random number generation function
 * 
 * @param _Min minimum value
 * @param _Max maximum value
 * @param _Seed seed
 * @return returns pseudo-random number in range [_Min, _Max] using _Seed.
 * The function uses a simple 64 bit linear feedback shift register for pseudo random numbers generation.
 */

template<typename Type> Type gs_pseudo_random(
    const Type& _Min  = gs_tiny<Type>(),
    const Type& _Max  = gs_huge<Type>(),
    const Type& _Seed = gs_huge<Type>());

//------------------------------------------------------------------------------------------------------------------------------------------------
// [COMPLEX]
//------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * @class gs_complex
 * @brief Represents complex number
 */
template<typename Type = float>
struct gs_complex
{
public:

    /**
     * @brief Constructs new gs_complex<T> object
     */
    gs_complex()
    {
        m_data.REAL = 0;
        m_data.IMAG = 0;
    }

    /**
     * @brief Constructs new gs_complex<T> object
     * @param _Value value that initializes real part of a complex number
     * @brief Constructs new gs_complex<T> object and initializes it's real part by _Value
     */
    gs_complex(Type _Value)
    {
        m_data.REAL = _Value;
        m_data.IMAG = 0;
    }

    /*!
     *  @brief Constructs new gs_complex<T> object
     *  @param _Real input real part of a complex number
     *  @param _Imag input imaginary of a complex number
     *  @brief Constructs new gs_complex<T> object and initializes it's real and imaginary parts by _Value
    */
    gs_complex(Type _Real, Type _Imag)
    {
        m_data.REAL = _Real;
        m_data.IMAG = _Imag;
    }

    /*!
     *  @brief Initializing constructor
     *  @param _Number input 2D fixed size array where _Number[0] stores real part and _Number[1] stores imaginary part
     *  @brief Constructs new gs_complex<T> object and initializes it's real part by _Number[0] and imaginary part by _Number[1]
    */
    gs_complex(Type _Number[2])
    {
        m_data.REAL = _Number[0];
        m_data.IMAG = _Number[1];
    }

    // +=
    inline gs_complex<Type> operator+=(const gs_complex<Type>& _Value)
    {
        complex_plain _c3 = complex_add(this->m_data, _Value.m_data);
        this->m_data.REAL = _c3.REAL;
        this->m_data.IMAG = _c3.IMAG;
        return *this;
    }

    // -=
    inline gs_complex<Type> operator-=(const gs_complex<Type>& _Value)
    {
        complex_plain _c3 = complex_substract(this->m_data, _Value.m_data);
        this->m_data.REAL = _c3.REAL;
        this->m_data.IMAG = _c3.IMAG;
        return *this;
    }

    // *=
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

    // /=
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

    /*!
     *  @brief Zero complex number generation function
     *  @returns Returns complex number with zerowed real and imaginary parts
    */
    static gs_complex<Type> zero()
    {
        return gs_complex<Type>(static_cast<Type>(0), static_cast<Type>(0));
    }

    /*!
     *  @brief Unit complex number generation function
     *  @returns Returns unit complex number
    */
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

/*!
*  @brief Complex number real part extraction function
*  @param _Number input complex number
*  @return returns real part of complex number
*/
template<typename Type> Type
gs_realf(const gs_complex<Type>& _Number)
{
    return _Number.m_data.REAL;
}

/*!
*  @brief Real number real part extraction function
*  @param [_Number] input real number
*  @return returns real part of real number
*/
template<typename Type> Type
gs_realf(const Type& _Number)
{
    return _Number;
}

/*!
*  @brief Complex number imaginary part extraction function
*  @param [_Number] input complex number
*  @return imaginary part of complex number
*/
template<typename Type> Type
gs_imagf(const gs_complex<Type>& _Number)
{
    return _Number.m_data.IMAG;
}

/**
 * @brief Real number imaginary part extraction function
 * 
 * @param _Number input real number
 * @return always returns zero
 */

template<typename Type> Type
gs_imagf(const Type& _Number)
{
    (void)_Number;
    return 0.0;
}

/**
 * @brief Complex number modulus computation function
 * 
 * @param _Number input complex number
 * @return the function returns input complex number modulus:
* \f[
*       abs(z) = \sqrt{ real\left( z \right)^2 + imag\left( z \right)^2 }
* \f]
*/
template<typename Type>
inline Type gs_cabsf(const gs_complex<Type>& _Number)
{
    return gs_complex<Type>::complex_abs(_Number.m_data);
}

/**
 * @brief Complex number angle computation function
 * 
 * @param _Number input complex number
 * @return Type the function returns input complex number angle:
* \f[
*       arg(z) = atan2\left( real\left( z \right) , imag\left( z \right) \right)
* \f]
 */

template<typename Type>
Type gs_cargf(const gs_complex<Type>& _Number)
{
    return gs_complex<Type>::complex_argument(_Number.m_data);
}

/**
 * @brief Complex number square root computation function
 * 
 * @param _Number input complex number
 * @return the function returns input complex number square root: 
* \f[
*       \sqrt{z} = \sqrt{ abs(z) } * cos \left( \frac{ arg(z) }{ 2 } \right) + j*\sqrt{ abs(z) } * sin \left( \frac{ arg(z) }{ 2 } \right)
* \f]
 */
template<typename Type> gs_complex<Type>
gs_csqrtf(const gs_complex<Type>& _Number)
{
    Type abs = gs_cabsf(_Number);
    Type arg = gs_cargf(_Number);
    return gs_complex<Type>(cos(arg * 0.5), sin(arg * 0.5)) * sqrt(abs);
}

/**
 * @brief Complex number power calculation function
 * 
 * @param _Number input complex number
 * @param _Power input power to which complex number is raised
 * @return the function returns input complex number power: 
* \f[
*       z^{n} = abs(z)^{n} * \left( cos\left( arg(z) * n \right) + j * sin\left(\right) arg(z) * n \right)
* \f]
 */
template<typename Type> gs_complex<Type>
gs_cpowf(const gs_complex<Type>& _Number, const Type& _Power)
{
    Type abs = gs_cabsf(_Number);
    Type arg = gs_cargf(_Number);
    return gs_complex<Type>(cos(arg * _Power), sin(arg * _Power)) * pow(abs, _Power);
}

/**
 * @brief Complex number conjugation functon
 * 
 * @param [_Number]  input complex number
 * @return * complex conjugated number:
* \f[
*       conj(real(z) + j * imag(z)) = real(z) - j * imag(z)
* \f]
 */
template<typename Type> gs_complex<Type>
gs_conjf(const gs_complex<Type>& _Number)
{
    return gs_complex<Type>(gs_realf(_Number), -gs_imagf(_Number));
}

/**
 * @brief Complex number normalization function
 * 
 * @param [_Number] input complex number
 * @return  the function returns normalized input complex number:
* \f[
*       norm(z) = \frac{ real(z) + j * imag(z) }{ abs( z ) }
* \f]
 */
template<typename Type> gs_complex<Type>
gs_cnormf(const gs_complex<Type>& _Number)
{
    Type abs = gs_cabsf(_Number);
    abs = (Type)(abs < 1e-12 ? (Type)1.0 : abs);
    return gs_complex<Type>(gs_realf(_Number) / abs, gs_imagf(_Number) / abs);
}

/*!
* @brief Rotation vector generation function
* @param _Argument input unit vector angle
* @return the function returns unit vector represented by a complex number with argument _Argument:
* \f[
*       rot(arg) = cos(arg) + j*sin(arg)
* \f]
*/
template<typename Type> gs_complex<Type>
gs_crotf(const float& _Argument)
{
    return gs_complex<Type>(cos(_Argument), sin(_Argument));
}

/*!
* @brief Complex hyperbollic sinus function
* @param _Number input complex number
* @return the function returns complex number hyperbollic sine:
* \f[
*       snih(z) = \frac{ e^{z} - e^{-z} }{ 2 }
* \f]
*/
template<typename Type> gs_complex<Type>
gs_sinhf(const gs_complex<Type>& _Number)
{
    Type re = (exp(gs_realf(_Number)) * cos(gs_imagf(_Number)) - exp(-gs_realf(_Number)) * cos(-gs_imagf(_Number))) * static_cast<Type>(0.5);
    Type im = (exp(gs_realf(_Number)) * sin(gs_imagf(_Number)) - exp(-gs_realf(_Number)) * sin(-gs_imagf(_Number))) * static_cast<Type>(0.5);
    return gs_complex<Type>(re , im);
}

/*!
* @brief Complex hyperbollic cosine function
* @param _Number input complex number
* @return the function returns complex number hyperbollic cosine:
* \f[
*       cosh( z ) = \frac{ e^{z} + e^{-z} }{ 2 }
* \f]
*/
template<typename Type> gs_complex<Type>
gs_coshf(const gs_complex<Type>& _Number)
{
    float re = (exp(gs_realf(_Number)) * cos(gs_imagf(_Number)) + exp(-gs_realf(_Number)) * cos(-gs_imagf(_Number))) * static_cast<Type>(0.5);
    float im = (exp(gs_realf(_Number)) * sin(gs_imagf(_Number)) + exp(-gs_realf(_Number)) * sin(-gs_imagf(_Number))) * static_cast<Type>(0.5);
    return gs_complex<Type>(re , im);
}

/*!
* @brief Complex hyperbollic tangent function
* @param _Number input complex number
* @return the function returns 32-bit complex number hyperbollic tangent:
* \f[
*       tanh(z) = \frac{ snih(z) }{ cosh(z) }
* \f]
*/
template<typename Type> gs_complex<Type>
gs_tanhf(const gs_complex<Type>& _Number)
{
    return gs_sinhf(_Number) / gs_coshf(_Number);
}

/*!
* @brief Complex hyperbollic catan function
* @param _Number input complex number
* @return the function returns 32-bit complex number hyperbollic catan:
* \f[
*       catanh(z) = \frac{ cosh(z) }{ snih(z) }
* \f]
*/
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

/*!
* @class gs_vector
* @tparam [Type] type of vecttor element
* @tparam [Size] size of a vector
* @brief Represents static vector
*/
template<typename Type, int Size>
struct gs_vector final : public gs_vector_data<Type, Size>
{
    typedef Type value_type;

    /*!
     *  @brief Default constructor
     *  @brief Initializes an empty vector
    */
    gs_vector()
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] = 0;
    }

    /*!
     *  @brief Initializing constructor
     *  @param [_Value] - vector value
     *  @brief Initializes every entry of a vector by a value _Value
    */
    gs_vector(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] = _Value;
    }

    /*!
     *  @brief Initializing constructor
     *  @param [_Other] - other vector
     *  @brief Sets values of this vector by the values of the _Other vector
    */
    gs_vector(const gs_vector<Type, Size>& _Other)
    {
        for (int i = 0; i < Size; i++)
            this->Data[i] = _Other[i];
    }

    /*!
     *  @brief Initializing constructor
     *  @param [_Other] - other vector
     *  @brief Sets values of this vector by the values of the _Other vector.
     * If _Other vector size is not equal to this vector size minimum amount of values between this vector
     * and _Other vector are initialized in this vector
    */
    template<int OtherSize>
    gs_vector(const gs_vector<Type, OtherSize>& _Other)
    {
        for (int i = 0; i < gs_min(OtherSize, Size); i++)
            this->Data[i] = _Other[i];
    }

    /*!
     *  @brief Initializing constructor
     *  @param [_Other] - other vector
     *  @param [_Args ] - input values
     *  @brief Sets values of this vector by the values of the _Other vector and if
     * not all values are initialized then remaining values are initialized by values from _Args
    */
    template <int OtherSize, typename... Args>
    gs_vector(const gs_vector<Type, OtherSize>& _Other, Args... _Args) 
    {
        int i = 0;

        for (i = 0; i < gs_min(OtherSize, Size); i++)
            this->Data[i] = _Other[i];

        if(i < Size)
            recursive_template_vector_initialization(static_cast<int>(i), static_cast<Type>(_Args)...);
    }

    /*!
     *  @brief Initializing constructor
     *  @param [_Args ] - input values
     *  @brief Sets values of this vector by the values from _Args
    */
    template <typename... Args>
    gs_vector(Args... _Args) 
    {
        GS_ASSERT(sizeof...(Args) <= Size);
        recursive_template_vector_initialization(static_cast<int>(0), static_cast<Type>(_Args)...);
    }

    /*!
     *  @brief Vector size retrieval function
     *  @return returns this vector size
    */
    const int size() const
    {
        return Size;
    }

    // &[]
    /*!
     *  @brief Vector element retrieval operator
     *  @param [_Index ] - vector element index
     *  @return returns this vector element if _Index is less than this vector size, asserts otherwise
    */
    Type& operator[](const int& _Index)
    {
        GS_ASSERT(_Index < Size);
        return this->Data[_Index];
    }

    // const Type[]&
    /*!
     *  @brief Vector element retrieval operator
     *  @param [_Index ] - vector element index
     *  @return returns this vector element if _Index is less than this vector size, asserts otherwise
    */
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
inline Type gs_sum_of_squares(Type _A)
{
    return _A * _A;
}

/*!
* @brief Squares summ computation function
* @param _A first input value
* @param _Args other optional input values
* @return returns the summ of squares of input values:
* \f[
*       sum = a^{2} + b^{2} + ... args^{2}
* \f]
*/
template<typename Type, typename ... Args>
inline Type gs_sum_of_squares(Type _A, Args... _Args)
{
    return gs_sum_of_squares(_A) + gs_sum_of_squares(_Args ...);
}

/*!
* @brief Vector squares summ computation function
* @param _Vector input vector
* @return returns the summ of squares of _Vector elements:
* \f[
*       sum = \sum_{i=0}^{n} V_i^{2}
* \f]
*/
template<typename Type, int Size>
inline Type gs_sum_of_squares(const gs_vector<Type, Size>& _Vector)
{
    Type sumOfSquares = 0;
    for (int i = 0; i < _Vector.size(); ++i)
        sumOfSquares += _Vector[i] * _Vector[i];
    return sumOfSquares;
}

/*!
* @brief Vector length computation function
* @param _A input vector [0] value
* @param _B input vector [1] value
* @param _Args input vector other values
* @return returns vector length:
* \f[
*       length = \sqrt{ a^{2} + b^{2} + ... args^{2} }
* \f]
*/
template<typename Type, typename ... Args>
inline Type gs_vector_length(Type _A, Type _B, Args... _Args)
{
    Type sumOfSquares = gs_sum_of_squares(_A, _B, _Args ...);
    return (Type)(sumOfSquares > 0 ? sqrt(sumOfSquares) : 0);
}

/*!
* @brief Vector length computation function
* @param _Vector input vector
* @return returns vector length:
* \f[
*       length = \sqrt{ \sum_{i=0}^{n} V_i^{2} }
* \f]
*/
template<typename Type, int Size>
inline Type gs_vector_length(const gs_vector<Type, Size>& _Vector)
{
    Type sumOfSquares = gs_sum_of_squares<Type, Size>(_Vector);
    return sumOfSquares > 0 ? (Type)sqrt(sumOfSquares) : (Type)0;
}

/*!
* @brief Vector normalization function
* @param _Vector input vector
* @return returns normalized _Vector:
* \f[
*       normalized = \frac{vector}{\sqrt{ \sum_{i=0}^{n} V_i^{2} }}
* \f]
*/
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

/*!
* @brief Vector argument computation function
* @param _Vector input vector
* @return returns vector argument computed from it's X and Y coordinates:
* \f[
*       arg(z) = atan2\left(x, y \right)
* \f]
*/
template<typename Type, int Size>
inline Type gs_vector_argument(const gs_vector<Type, Size>& _Vector)
{
    gs_vector<Type, Size> normalized = gs_vector_normalize(_Vector);
    return (Type)atan2(normalized.y, normalized.x);
}

/*!
* @brief Vectors dot product computation function
* @param _A first input vector
* @param _B second input vector
* @return returns vectors _A and _B dot product:
* \f[
*       result = a[0] * b[0] + a[1] * b[1] + ... a[n] * b[n]
* \f]
*/
template<typename Type, int Size>
inline Type gs_vectors_dot(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    Type dot = 0;
    for (int i = 0; i < Size; i++)
        dot += _A[i] * _B[i];
    return dot;
}

/*!
* @brief 2D Vectors cross product computation function
* @param _A first 2D input vector
* @param _B second 2D input vector
* @return returns _A and _B 2D vectors cross product equal  to the area of the parallelogram formed by two input vectors:
* \f[
*       result = a.x * b.y - a.y * b.x
* \f]
*/
template<typename Type>
inline Type gs_vector_cross(const gs_vector<Type, 2>& _A, const gs_vector<Type, 2> _B)
{
    const Type Ax = _A[0];
    const Type Ay = _A[1];
    const Type Bx = _B[0];
    const Type By = _B[1];
    return Ax * By - Ay * Bx;
}

/*!
* @brief 2D Vectors cross product computation function
* @param _A first 3D input vector
* @param _B second 3D input vector
* @return returns _A and _B 3D vectors cross product equal to the vector that is
* perpendicular to multiplied vectors and which length is equal to the area of the parallelogram formed by two input vectors:
* \f[
*    \begin{cases} 
*       result.x = a.y * b.z - b.y * a.z \\
*       result.y = a.z * b.x - b.z * a.x \\
*       result.z = a.x * b.y - b.x * a.y \\
*    \end{cases}
* \f]
*/
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

/*!
* @brief Vectors clamp function
* @tparam Type input vector element type
* @tparam Size input vector size
* @param _Value input vector
* @param _Min input vector coordiantes minimum values
* @param _Max input vector coordiantes maximum values
* @return returns _Value vector which coordinates are clamped between _Min and _Max
*/
template<typename Type, int Size>
inline gs_vector<Type, Size> gs_clamp(const gs_vector<Type, Size>& _Value, const gs_vector<Type, Size>& _Min, const gs_vector<Type, Size>& _Max)
{
    gs_vector<Type, Size> Vector;
    for (int i = 0; i < Size; ++i)
        Vector[i] = gs_clamp(_Value[i], _Min[i], _Max[i]);
    return Vector;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// [MATRIX]
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
/*!
* @class gs_matrix
* @tparam Type type of matrix elements
* @tparam Rows matrix rows count
* @tparam Columns size of a vector
* @brief Represents static matrix object in column-wise order and with compile time Rows and Columns count.
*/
template<typename Type, int Rows, int Columns>
struct gs_matrix final
{
    typedef Type value_type;

    /**
     * @brief Construct a new gs_matrix<Type, Rows, Columns> object
     * @param _Value the value that initializes diagonal elements 
     */
    gs_matrix(const Type& _Value = static_cast<Type>(0))
    {
        for (int i = 0; i < Columns; ++i)
            Data[i * Columns + i] = _Value;
    }

    /**
     * @brief Makes a copy of gs_matrix<Type, Rows, Columns> object
     * @param _Matrix object to copy
     */
    gs_matrix(const gs_matrix<Type, Rows, Columns>& _Matrix)
    {
        for (int i = 0; i < Size; ++i)
            Data[i] = _Matrix.Data[i]; 
    }

    /**
     * @brief The function to get this matrix rows number
     * @return returns this matrix rows number
     */
    int rows() const
    {
        return Rows;
    }

    /**
     * @brief The function to get this matrix columns number
     * @return returns this matrix columns number
     */
    int columns() const
    {
        return Columns;
    }

    /**
     * @brief The function to get matrix column vector
     * @param _Column column index
     * @return returns this matrix column vector. If _Column is greater than matrix columns count function asserts.
     */
    Type* operator[](const int& _Column)
    {
        GS_ASSERT(_Column < Columns);
        return &Data[_Column * Rows];
    }

    /**
     * @brief The function to get matrix column vector
     * @param _Column column index
     * @return returns this matrix column vector. If _Column is greater than matrix columns count function asserts.
     */
    const Type* operator[](const int& _Column) const
    {
        GS_ASSERT(_Column < Columns);
        return &Data[_Column * Rows];
    }

    // +=
    gs_matrix<Type, Rows, Columns> operator+=(const gs_matrix<Type, Rows, Columns>& _Matrix)
    {
        gs_matrix<Type, Rows, Columns> result;
        add_mat(*this, _Matrix, result);
        
        for (int i = 0; i < Size; i++)
            Data[i] = result.Data[i];
        
        return *this;
    }

    // -=
    gs_matrix<Type, Rows, Columns> operator-=(const gs_matrix<Type, Rows, Columns>& _Matrix)
    {
        gs_matrix<Type, Rows, Columns> result;
        sub_mat(*this, _Matrix, result);

        for (int i = 0; i < Size; i++)
            Data[i] = result.Data[i];

        return *this;
    }

    // *=
    template<int Dimention>
    gs_matrix<Type, Rows, Dimention> operator*=(const gs_matrix<Type, Columns, Dimention>& _Matrix)
    {
        gs_matrix<Type, Rows, Dimention> result;
        mul_mat(*this, _Matrix, result);

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
    bool equals(const gs_matrix<Type, Rows, Columns>& _B) const
    {
        for (int i = 0; i < Size; i++)
        {
            if(Data[i] != _B.Data[i])
                return false;
        }

        return true;
    }

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

/**
 * @brief Matrix transposition function
 * @param _Matrix input matrix 
 * @return returns transposed _Matrix
 */
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

/**
 * @brief Square matrix factorization function
 * @param _Matrix input matrix 
 * @return returns a struct containing matrix factors and rows inverse permutation vector
 * @details Function factors input square matrix using right looking LU factorization with column-wise pivoting.
 */
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


/**
 * @brief Square matrix determinant computation function
 * @param _Matrix input matrix 
 * @return returns matrix determinant
 */
template<typename Type, int Size>
Type gs_matrix_determinant_square(const gs_matrix<Type, Size, Size>& _Matrix)
{
    Type det = (Type)1;
    auto factorization = gs_matrix_factor_square(_Matrix);
    for (int j = 0; j < factorization.Matrix.columns(); j++)
        det *= factorization.Matrix[j][j];
    return det;
}

/**
 * @brief Linear square equation system solve function
 * @param _Matrix input matrix 
 * @param _RightHandSide right-hand side vectors matrix
 * @return returns a matrix where each column is a solution of equation system with coefficient matrix _Matrix.
 */
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

/**
 * @brief Square matrix invertion function
 * @param _Matrix input matrix 
 * @return returns inverted matrix _Matrix
 */
template<typename Type, int Size>
gs_matrix<Type, Size, Size> gs_matrix_invert_square(const gs_matrix<Type, Size, Size>& _Matrix)
{
    gs_matrix<Type, Size, Size> eye(0);
    for (int i = 0; i < Size; i++)
        eye[i][i] = 1.0;

    return gs_matrix_solve_square(_Matrix, eye);
}

/**
 * @brief Scale transform 3D matrix generation function
 * @param _Matrix input matrix
 * @param _Transform scale 3D vector
 * @return returns scale transform 3D matrix
 */
template<typename Type>
inline gs_matrix<Type, 4, 4> gs_matrix_scale(const gs_matrix<Type, 4, 4>& _Matrix, const gs_vector<Type, 3>& _Transform)
{
    gs_matrix<Type, 4, 4> transform(1);
    transform[0][0] = _Transform[0];
    transform[1][1] = _Transform[1];
    transform[2][2] = _Transform[2];
    return _Matrix * transform;
}

/**
 * @brief Translation transform 3D matrix generation function
 * @param _Matrix input matrix
 * @param _Transform translation 3D vector
 * @return returns translation transform 3D matrix
 */
template<typename Type>
inline gs_matrix<Type, 4, 4> gs_matrix_translate(const gs_matrix<Type, 4, 4>& _Matrix, const gs_vector<Type, 3>& _Transform)
{
    gs_matrix<Type, 4, 4> transform(1);
    transform[3][0] = _Transform[0];
    transform[3][1] = _Transform[1];
    transform[3][2] = _Transform[2];
    return _Matrix * transform;
}

/**
 * @brief Rotation transform 3D matrix generation function
 * @param _Matrix input matrix
 * @param _Angle rotation angle
 * @param _Axis axis around which we need to rotate
 * @return returns rotation transform 3D matrix
 */
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

/**
 * @brief Transform translation 3D matrix translation vector extraction function
 * @param _Matrix input matrix
 * @return returns translation 3D vector out-of input Transform translation 3D matrix
 */
template<typename Type>
inline gs_vector<Type, 3> gs_matrix_retrieve_transform_translation_vector(const gs_matrix<Type, 4, 4>& _Matrix)
{
    return {_Matrix[3][0], _Matrix[3][1], _Matrix[3][2]};
}

/**
 * @brief Calculates orthogonal projection matrix
 * 
 * @param _Left horizontal clipping plane start X coordinate
 * @param _Right horizontal clipping plane end X coordinate
 * @param _Bottom vertical clipping plane start Y coordinate
 * @param _Top vertical clipping plane end Y coordinate
 * @param _ZNear near clipping plane Z coordinate
 * @param _ZFar far clipping plane Z coordinate
 * @param _RightHand defines if we are in right-hand coordinate system
 * @param _NegativeOne defines if Z-clipping plane range is [-1; +1] or [0:+1]
 * @return returns orthogonal projection matrix:
* \f[
*    \begin{cases} 
*       \text{if _RightHand = true}, then
*       \begin{cases}
*           \text{if _NegativeOne = true}, then
*              \begin{pmatrix}
*                 \frac{2}{right - left} & 0 & 0 & -\frac{right + left}{right - left} \\
*                 0 & \frac{2}{top - bottom} & 0 & -\frac{top + bottom}{top - bottom} \\
*                 0 & 0 & -\frac{2}{far - near} & -\frac{far + near}{far - near} \\
*                 0 & 0 & 0 & 1
*              \end{pmatrix} 
*            \\
*           \text{if _NegativeOne = false}, then
*              \begin{pmatrix}
*                 \frac{2}{right - left} & 0 & 0 & -\frac{right + left}{right - left} \\
*                 0 & \frac{2}{top - bottom} & 0 & -\frac{top + bottom}{top - bottom} \\
*                 0 & 0 & -\frac{1}{far - near} & -\frac{near}{far - near} \\
*                 0 & 0 & 0 & 1
*              \end{pmatrix}
*       \end{cases}
*       \\
*       \text{if _RightHand = false}, then
*       \begin{cases}
*           \text{if _NegativeOne = true}, then
*              \begin{pmatrix}
*                 \frac{2}{right - left} & 0 & 0 & -\frac{right + left}{right - left} \\
*                 0 & \frac{2}{top-bottom} & 0 & -\frac{top + bottom}{top - bottom} \\
*                 0 & 0 & \frac{2}{far-near} & -\frac{far + near}{far - near} \\
*                 0 & 0 & 0 & 1
*              \end{pmatrix} 
*            \\
*           \text{if _NegativeOne = false}, then
*              \begin{pmatrix}
*                 \frac{2}{right - left} & 0 & 0 & -\frac{right + left}{right - left} \\
*                 0 & \frac{2}{top-bottom} & 0 & -\frac{top + bottom}{top - bottom} \\
*                 0 & 0 & \frac{1}{far-near} & -\frac{near}{far - near} \\
*                 0 & 0 & 0 & 1
*              \end{pmatrix}
*       \end{cases}
*    \end{cases}
* \f]
 */
template<typename T>
inline gs_matrix<T, 4, 4> gs_matrix_ortho(
    const T&    _Left,
    const T&    _Right,
    const T&    _Bottom,
    const T&    _Top,
    const T&    _ZNear,
    const T&    _ZFar,
    const bool& _RightHand   = true,
    const bool& _NegativeOne = true)
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
    if(_RightHand)
    {
        return _NegativeOne ?
            gs_matrix_ortho_rh_no(_Left, _Right, _Bottom, _Top, _ZNear, _ZFar) :
                gs_matrix_ortho_rh_zo(_Left, _Right, _Bottom, _Top, _ZNear, _ZFar);
    }

    // left hand
    return _NegativeOne ?
        gs_matrix_ortho_lh_no(_Left, _Right, _Bottom, _Top, _ZNear, _ZFar) :
            gs_matrix_ortho_lh_zo(_Left, _Right, _Bottom, _Top, _ZNear, _ZFar);
}

//------------------------------------------------------------------
// look at
//------------------------------------------------------------------
/**
 * @brief Camera look at matrix computation function
 * 
 * @param _Eye The 3D position of the camera in world coordinates.
 * @param _Center The 3D position the camera is looking at. This defines the direction of the gaze.
 * @param _Up The world’s upward direction.
 * @param _RightHand defines if we are in right-hand coordinate system
 * @return gs_matrix<Type, 4, 4>:
* \f[
*
*       \begin{cases}
*       f = \frac{center - eye}{abs(center - eye)} \\
*       s = \frac{cross(f, up)}{abs(cross(f, up))}   \\
*       u = cross(s, f) \\
*           \text{if _RightHand = true}, then
*              \begin{pmatrix}
*                 s.x & s.y & s.z & -dot(s, eye) \\
*                 u.x & u.y & u.z & -dot(u, eye) \\
*                 -f.x & -f.y & -f.z & dot(f, eye) \\
*                 0 & 0 & 0 & 1
*              \end{pmatrix} 
*            \\
*           \text{if _RightHand = false}, then
*              \begin{pmatrix}
*                 s.x & s.y & s.z & -dot(s, eye) \\
*                 u.x & u.y & u.z & -dot(u, eye) \\
*                 f.x & f.y & f.z & -dot(f, eye) \\
*                 0 & 0 & 0 & 1
*              \end{pmatrix}
*       \end{cases}
* \f]
 */
template<typename Type>
inline gs_matrix<Type, 4, 4> gs_matrix_look_at(
    const gs_vector<Type, 3>& _Eye,
    const gs_vector<Type, 3>& _Center,
    const gs_vector<Type, 3>& _Up,
    const bool&               _RightHand = true)
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

    return _RightHand ? gs_matrix_look_at_rh(_Eye, _Center, _Up) : gs_matrix_look_at_lh(_Eye, _Center, _Up);
}

//------------------------------------------------------------------
// prespective
//------------------------------------------------------------------
/**
 * @brief Calculates pespective projection matrix
 * 
 * @param _FieldOfView field of view angle in radians
 * @param _Aspect screen aspect (width / height)
 * @param _ZNear near clipping plane Z coordinate
 * @param _ZFar far clipping plane Z coordinate
 * @param _RightHand defines if we are calculating projection in right-hand coordinate system
 * @param _NegativeOne defines if we are in right-hand coordinate system
 * @return returns pespective projection matrix:
* \f[
*    \begin{cases} 
*       \text{if _RightHand = true}, then
*       \begin{cases}
*           \text{if _NegativeOne = true}, then
*              \begin{pmatrix}
*                 \frac{1}{aspect * tan\left( \frac{2}{\phi} \right)} & 0 & 0 & 0 \\
*                 0 & \frac{1}{tan\left( \frac{2}{\phi} \right)} & 0 & 0 \\
*                 0 & 0 & -\frac{far + near}{far - near} & -\frac{2 * far * near}{far - near} \\
*                 0 & 0 & -1 & 0
*              \end{pmatrix} 
*            \\
*           \text{if _NegativeOne = false}, then
*              \begin{pmatrix}
*                 \frac{1}{aspect * tan\left( \frac{2}{\phi} \right)} & 0 & 0 & 0 \\
*                 0 & \frac{1}{tan\left( \frac{2}{\phi} \right)} & 0 & 0 \\
*                 0 & 0 & -\frac{far}{far - near} & -\frac{far * near}{far - near} \\
*                 0 & 0 & -1 & 0
*              \end{pmatrix}
*       \end{cases}
*       \\
*       \text{if _RightHand = false}, then
*       \begin{cases}
*           \text{if _NegativeOne = true}, then
*              \begin{pmatrix}
*                 \frac{1}{aspect * tan\left( \frac{2}{\phi} \right)} & 0 & 0 & 0 \\
*                 0 & \frac{1}{tan\left( \frac{2}{\phi} \right)} & 0 & 0 \\
*                 0 & 0 & \frac{far + near}{far - near} & -\frac{2 * far * near}{far - near} \\
*                 0 & 0 & 1 & 0
*              \end{pmatrix} 
*            \\
*           \text{if _NegativeOne = false}, then
*              \begin{pmatrix}
*                 \frac{1}{aspect * tan\left( \frac{2}{\phi} \right)} & 0 & 0 & 0 \\
*                 0 & \frac{1}{tan\left( \frac{2}{\phi} \right)} & 0 & 0 \\
*                 0 & 0 & \frac{far}{far - near} & -\frac{far * near}{far - near} \\
*                 0 & 0 & 1 & 0
*              \end{pmatrix}
*       \end{cases}
*    \end{cases}
* \f]
 */
template<typename Type>
inline gs_matrix<Type, 4, 4> gs_matrix_perspective(
    const Type& _FieldOfView,
    const Type& _Aspect,
    const Type& _ZNear,
    const Type& _ZFar,
    const bool& _RightHand   = true,
    const bool& _NegativeOne = true)
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
    if(_RightHand)
    {
        return _NegativeOne ?
            gs_matrix_perspective_rh_no(_FieldOfView, _Aspect, _ZNear, _ZFar) :
                gs_matrix_perspective_rh_zo(_FieldOfView, _Aspect, _ZNear, _ZFar);
    }

    // left hand
    return _NegativeOne ?
        gs_matrix_perspective_lh_no(_FieldOfView, _Aspect, _ZNear, _ZFar) :
            gs_matrix_perspective_lh_zo(_FieldOfView, _Aspect, _ZNear, _ZFar);
}

/*!
* @brief Function to check if 2D point is inside 2D polygon
* @param _Polygon array of 2D polygon points
* @param _Size number of points in 2D polygon
* @param _Point point to check
* @return returns true if the _Point is inside polygon, otherwise returns false
*/
template<typename Type>
int gs_2D_point_in_polygon(const gs_vector<Type, 2> _Polygon[], const int _Size, const gs_vector<Type, 2>& _Point)
{
    int i, j, c = 0;

    for (i = 0, j = _Size-1; i < _Size; j = i++) 
    {
        if(((_Polygon[i].y > _Point.y) != (_Polygon[j].y > _Point.y)) &&
            (_Point.x < (_Polygon[j].x - _Polygon[i].x) * (_Point.y - _Polygon[i].y) / (_Polygon[j].y-_Polygon[i].y) + _Polygon[i].x)) c = !c;
    }

    return c;
}

/*!
* @brief Function that computes 2D polygon signed area using Shoelace Formula
* @param _Polygon array of 2D polygon points
* @param _Size number of points in 2D polygon
* @return returns 2D polygon signed area
*/
template<typename Type>
Type gs_2D_polygon_signed_area(const gs_vector<Type, 2> _Polygon[], const int& _Size)
{
    Type sum = 0;
    for (int i = 0; i < _Size; ++i)
        sum += _Polygon[i].x * _Polygon[(i + 1) % _Size].y - _Polygon[(i + 1) % _Size].x * _Polygon[i].y;
    return sum / (Type)2;
};

/*!
* @brief Function that checks on which side from 2D line lies 2D point
* @param _Source 2D line source point
* @param _Target 2D line target point
* @param _Point 2D point to chekc
* @return returns -1 if point if on the left and +1 if point on the right from line and 0 if point lies on the line
*/
template<typename Type>
Type gs_2D_where_point_lies(const gs_vector<Type, 2>& _Source, const gs_vector<Type, 2>& _Target, const gs_vector<Type, 2>& _Point)
{
    return (_Point.x - _Source.x) * (_Target.y - _Source.y) - (_Point.y - _Source.y) * (_Target.x - _Source.x);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// [2D BOX]
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief  Represents 2D box
 */
template<typename Type>
struct gs_2d_box
{
    /**
     * @brief Construct a new gs_2dbox<gs_2dbox> object
     */
    gs_2d_box() : Min(gs_vector<Type, 2>(0.f)), Max(gs_vector<Type, 2>(0.f)){}

    /**
     * @brief Constructs a new gs_2dbox<gs_2dbox> object
     * 
     * @param _Points points array 
     * @param _Count points array  size
     * @details takes a range of points, the point Min(X, Y) coordinates are the top left and Max(X,Y) are the bottom right
     */
    gs_2d_box(const gs_vector<Type, 2> _Points[], const int& _Count)
    {
        Min = _Points[0];
        Max = _Points[0];

        for (int i = 0; i < _Count; i++)
        {
            Min = gs_vector<Type, 2>(gs_min(Min.x, _Points[i].x), gs_min(Min.y, _Points[i].y));
            Max = gs_vector<Type, 2>(gs_max(Max.x, _Points[i].x), gs_max(Max.y, _Points[i].y));
        }
    }

    /**
     * @brief Construct a new gs_2dbox<gs_2dbox> object
     * 
     * @tparam Args 
     * @param _A first point 
     * @param _B second point
     * @param _Args other points
     * @details takes a range of points, the point Min(X, Y) coordinates are the top left and Max(X,Y) are the bottom right
     */
    template<typename ... Args>
    gs_2d_box(const gs_vector<Type, 2>& _A, const gs_vector<Type, 2>& _B, Args ... _Args)
    {
        Min = gs_vector<Type, 2>(gs_min(_A.x, _B.x, static_cast<gs_vector<Type, 2>>(_Args).x...), gs_min(_A.y, _B.y, static_cast<gs_vector<Type, 2>>(_Args).y...));
        Max = gs_vector<Type, 2>(gs_max(_A.x, _B.x, static_cast<gs_vector<Type, 2>>(_Args).x...), gs_max(_A.y, _B.y, static_cast<gs_vector<Type, 2>>(_Args).y...));
    }

    /**
     * @brief 2D box size
     * @return returns 2D box size
     */
    gs_vector<Type, 2> size() const
    {
        return gs_vector<Type, 2>(gs_abs((Max - Min).x), gs_abs((Max - Min).y));
    }

    /**
     * @brief 2D box width
     * @return returns 2D box width
     */
    Type width() const
    {
        return size().x;
    }

    /**
     * @brief 2D box height
     * @return returns 2D box height
     */
    Type height() const
    {
        return size().y;
    }

    /**
     * @brief 2D area
     * @return returns 2D box area
     */
    Type area() const
    {
        return width() * height();
    }

    /**
     * @brief 2D box center
     * @return returns 2D box center
     */
    gs_vector<Type, 2> center() const
    {
        return (Min + Max) * 0.5f;
    }

    /**
     * @brief Detects if the point is within 2D box
     * @param _Point input point 
     * @return returns true if point is inside 2D box
     */
    bool contains(const gs_vector<Type, 2>& _Point) const
    {
        return _Point.x >= Min.x &&
               _Point.y >= Min.y &&
               _Point.x <= Max.x &&
               _Point.y <= Max.y;
    }

    /**
     * @brief Detects if another 2D box is within this 2D box
     * @param _Other another 2D box
     * @return returns true if another 2D box is within this 2D box
     */
    bool contains(const gs_2d_box<Type>& _Other) const
    {
        return _Other.Min.x > Min.x &&
               _Other.Min.y > Min.y &&
               _Other.Max.x < Max.x &&
               _Other.Max.y < Max.y;
    }

    /**
     * @brief Detects if this 2D box overlaps another 2D box
     * @param _Other another 2D box
     * @return returns true if this 2D box overlaps another 2D box
     */
    bool overlaps(const gs_2d_box<Type>& _Other) const
    {
        return gs_abs(clip_with(_Other).width() * clip_with(_Other).height())             > gs_epsilon<Type>() * 2 ||
               gs_abs(_Other.clip_with(*this).width() * _Other.clip_with(*this).height()) > gs_epsilon<Type>() * 2;
    }

    /**
     * @brief Checks if this 2D box intersects 2D line
     * @param _P1 2D line source point
     * @param _P2 2D line target point
     * @return returns true if this 2D box overlaps another 2D box
     */
    bool intersects(const gs_vector<Type, 2>& _P1, const gs_vector<Type, 2>& _P2) const
    {
        return 
               // check that this 2D box contains the line
               ( contains(_P1) && contains(_P2) )                                                                                                                                                                                       ||
               
               // check that the line goes through this 2D box
               ( gs_sign(gs_2D_where_point_lies(gs_vector<Type, 2>(Min.x, Min.y), gs_vector<Type, 2>(Max.x, Min.y), _P1)) != gs_sign(gs_2D_where_point_lies(gs_vector<Type, 2>(Min.x, Min.y), gs_vector<Type, 2>(Max.x, Min.y), _P2)) ) ||
               ( gs_sign(gs_2D_where_point_lies(gs_vector<Type, 2>(Max.x, Min.y), gs_vector<Type, 2>(Max.x, Max.y), _P1)) != gs_sign(gs_2D_where_point_lies(gs_vector<Type, 2>(Max.x, Min.y), gs_vector<Type, 2>(Max.x, Max.y), _P2)) ) ||
               ( gs_sign(gs_2D_where_point_lies(gs_vector<Type, 2>(Max.x, Max.y), gs_vector<Type, 2>(Min.x, Max.y), _P1)) != gs_sign(gs_2D_where_point_lies(gs_vector<Type, 2>(Max.x, Max.y), gs_vector<Type, 2>(Min.x, Max.y), _P2)) ) ||
               ( gs_sign(gs_2D_where_point_lies(gs_vector<Type, 2>(Min.x, Max.y), gs_vector<Type, 2>(Min.x, Min.y), _P1)) != gs_sign(gs_2D_where_point_lies(gs_vector<Type, 2>(Min.x, Max.y), gs_vector<Type, 2>(Min.x, Min.y), _P2)) );
    }

    /**
     * @brief Applies transform to this 2D box
     * @param _Transform applied tranform matrix 
     * @return returns transformed 2D box
     */
    gs_2d_box<Type> transform(const gs_matrix<Type, 4, 4>& _Transform) const
    {
        return gs_2d_box<Type>(
            _Transform * gs_vector<Type, 4>(Min, 0.f, 1.f),
            _Transform * gs_vector<Type, 4>(Max, 0.f, 1.f));
    }

    /**
     * @brief Clips this 2D box with input clipping box
     * @param _Clipbox input clipping box
     * @return returns this 2D box clipped by input clipping box
     */
    gs_2d_box<Type> clip_with(const gs_2d_box<Type>& _Clipbox) const
    {
        return gs_2d_box<Type>(
            gs_clamp(Min, _Clipbox.Min, _Clipbox.Max),
            gs_clamp(Max, _Clipbox.Min, _Clipbox.Max));
    }

    gs_vector<Type, 2> Min{gs_vector<Type, 2>(0.f)}; ///< top left
    gs_vector<Type, 2> Max{gs_vector<Type, 2>(0.f)}; ///< bottom roght
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// [2D ELLIPSE]
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * @brief represents 2D ellipse
 */
template<typename Type>
struct gs_2d_ellipse
{
    /**
     * @brief Construct a new gs_2d_ellipse<T> object
     * @param _Center ellipse center
     * @param _Radius ellipse radius
     */
    gs_2d_ellipse(
        const gs_vector<Type, 2>& _Center,
        const Type&               _Radius) : Center(_Center), MinorRadius(_Radius), MajorRadius(_Radius){}

    /**
     * @brief Construct a new gs_2d_ellipse<T> object
     * @param _Center ellipse center
     * @param _MinorRadius ellipse minor radius
     * @param _MajorRadius ellipse major radius
     */
    gs_2d_ellipse(
        const gs_vector<Type, 2>& _Center,
        const Type&               _MinorRadius,
        const Type&               _MajorRadius) : Center(_Center), MinorRadius(_MinorRadius), MajorRadius(_MajorRadius){}

    /**
     * @brief Detects if a point is inside ellipse
     * @param _Point point of interest 
     * @return returns true if the point is inside ellipse
     */
    bool contains(const gs_vector<Type, 2>& _Point) const
    {        
        Type dx = (Center.x - _Point.x);
        Type dy = (Center.y - _Point.y);
        Type dd = dx * dx / MinorRadius / MinorRadius + dy * dy / MajorRadius / MajorRadius;

        return dd < 1.f || gs_abs(dd - 1.f) < gs_tiny<Type>() * (Type)2;
    }

    /**
     * @brief Returns this 2D ellipse bounding box
     * @return returns this 2D ellipse bounding box
     */
    gs_2d_box<Type> bounding_box() const
    {
        return gs_2d_box<Type>(
            Center - gs_vector<Type, 2>(MinorRadius, MajorRadius),
            Center + gs_vector<Type, 2>(MinorRadius, MajorRadius));
    }

    /**
     * @brief Returns transformed ellipse
     * @param _Transform transform matrix 
     * @return returns transformed ellipse
     */
    gs_2d_ellipse<Type> transform(const gs_matrix<Type, 4, 4>& _Transform)
    {
        return gs_2d_ellipse<Type>(_Transform * gs_vector<Type, 4>(Center, 0.f, 1.f), MinorRadius, MajorRadius);
    }

    gs_vector<Type, 2> Center;
    Type               MinorRadius;
    Type               MajorRadius;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// [2D TRIANGLE]
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief represents 2D triangle
 */
template<typename Type>
struct gs_2d_triangle
{
    /**
     * @brief Construct a new gs_2d_triangle<T> object
     * @param _P1 2D triangle first point
     * @param _P2 2D triangle second point
     * @param _P3 2D triangle third point
     */
    gs_2d_triangle(
        const gs_vector<Type, 2>& _P1 = gs_vector<Type, 2>((Type)0, (Type)0),
        const gs_vector<Type, 2>& _P2 = gs_vector<Type, 2>((Type)0, (Type)0),
        const gs_vector<Type, 2>& _P3 = gs_vector<Type, 2>((Type)0, (Type)0)) : P1(_P1), P2(_P2), P3(_P3){}

    /**
     * @brief Constructs a new gs_2d_triangle<T> bounding triangle object around a given array of points
     * @param _Points 2D triangle first point
     * @param _Count  2D triangle second point
     */
    gs_2d_triangle(const gs_vector<Type, 2> _Points[], const int& _Count)
    {
        // points bounding box
        gs_2d_box<Type> boundingBox = gs_2d_box<Type>(_Points, _Count);
        
        // points circum circle radius
        Type circumCircleRadius = 0.f;
        for (int i = 0; i < _Count; i++)
            circumCircleRadius = gs_max(circumCircleRadius, gs_vector_length(_Points[i] - boundingBox.center()));

        // points bounding triangle
        Type boundingTriangleSide   = 2.f * sqrtf(3) * circumCircleRadius;
        Type boundingTriangleHeight = 3.f * circumCircleRadius;

        P1 = boundingBox.center() + gs_vector<Type, 2>(0.f, boundingTriangleHeight - circumCircleRadius);
        P2 = boundingBox.center() + gs_vector<Type, 2>(0.f, -circumCircleRadius) + gs_vector<Type, 2>(+boundingTriangleSide * 0.5f, 0.f);
        P3 = boundingBox.center() + gs_vector<Type, 2>(0.f, -circumCircleRadius) + gs_vector<Type, 2>(-boundingTriangleSide * 0.5f, 0.f);
    }

    /**
     * @brief returns tirangle area
     * @return returns tirangle area 
     */
    Type area() const
    {
        Type a = gs_vector_length(P2 - P1);
        Type b = gs_vector_length(P3 - P2);
        Type c = gs_vector_length(P1 - P3);
        Type s = (a + b + c) / (Type)2;
        Type p = s * (s - a) * (s - b) * (s - c);
        return p > (Type)0 ? sqrt(p) : (Type)0;
    }

    /**
     * @brief checks if this 2D triangle contains 2D point inside
     * @return returns tirangle area 
     */
    bool contains(const gs_vector<Type, 2>& _Point) const
    {
        gs_vector<Type, 2> points[3] = {P1, P2, P3};
        return gs_2D_point_in_polygon(points, 3, _Point);
    }

    /**
     * @brief returns this 2D thisangle circum circle
     * @return returns this 2D thisangle circum circle
     */
    gs_2d_ellipse<Type> circum_circle() const
    {
        // are the 3 pts of the tri
        gs_vector<Type, 3> a = P1;
        gs_vector<Type, 3> b = P2;
        gs_vector<Type, 3> c = P3;

        gs_vector<Type, 3> ac = c - a ;
        gs_vector<Type, 3> ab = b - a ;
        gs_vector<Type, 3> abXac = gs_vector_cross(ab, ac);

        // this is the vector from a TO the circumsphere center    
        gs_vector<Type, 3> toCircumsphereCenter = (gs_vector_cross(abXac, ab) * gs_sum_of_squares(ac) + gs_vector_cross(ac, abXac ) * gs_sum_of_squares(ab)) / (gs_sum_of_squares(abXac) * (Type)2);
        float              circumsphereRadius   = gs_vector_length(toCircumsphereCenter);

        // The 3 space coords of the circumsphere center then:
        gs_vector<Type, 3> ccs = a + toCircumsphereCenter ; // now this is the actual 3space location
        return gs_2d_ellipse<Type>(ccs, circumsphereRadius);
    };

    /**
     * @brief Returns this 2D triangle bounding box
     * @return returns this 2D triangle bounding box
     */
    gs_2d_box<Type> bounding_box() const
    {
        return gs_2d_box<Type>(P1, P2, P3);
    }

    gs_vector<Type, 2> P1{gs_vector<Type, 2>((Type)0, (Type)0)};
    gs_vector<Type, 2> P2{gs_vector<Type, 2>((Type)0, (Type)0)};
    gs_vector<Type, 2> P3{gs_vector<Type, 2>((Type)0, (Type)0)};
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// [2D LINE]
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief represents 2D line
 */
template<typename Type>
struct gs_2d_line
{
    /**
     * @brief Construct a new gs_2d_line<T> object
     * @param _P1 source 2D line point
     * @param _P2 target 2D line point
     */
    gs_2d_line(
        const gs_vector<Type, 2>& _P1 = gs_vector<Type, 2>((Type)0, (Type)0),
        const gs_vector<Type, 2>& _P2 = gs_vector<Type, 2>((Type)0, (Type)0)) : P1(_P1), P2(_P2){}

    /**
     * @brief Returns this 2D line length
     * @return returns this 2D ine length
     */
    Type length() const
    {
        return gs_vector_length(P2 - P1);
    }

    /**
     * @brief Returns this 2D line direction
     * @return returns this 2D ine direction
     */
    gs_vector<Type, 2> direction() const
    {
        return gs_vector_normalize(P2 - P1);
    }

    gs_vector<Type, 2> perpendicular() const
    {
        return gs_vector_normalize(gs_vector_cross(gs_vector<Type, 3>(direction()), gs_vector<Type, 3>((Type)0, (Type)0, (Type)1)));
    }

    /**
     * @brief Returns this 2D line bounding box
     * @return returns this 2D triangle bounding box
     */
    gs_2d_box<Type> bounding_box() const
    {
        return gs_2d_box<Type>(P1, P2);
    }

    gs_vector<Type, 2> P1{gs_vector<Type, 2>((Type)0, (Type)0)};
    gs_vector<Type, 2> P2{gs_vector<Type, 2>((Type)0, (Type)0)};
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// [BINARY OPERATORS]
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// [VECTORS VS VECTORS]
template<typename Type, int Size>
bool operator!=(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    for (int i = 0; i < Size; ++i)
    {
        if(_A[i] != _B[i])
            return true;
    }

    return false;
}

template<typename Type, int Size>
bool operator==(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    for (int i = 0; i < Size; ++i)
    {
        if(_A[i] != _B[i])
            return false;
    }

    return true;
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
    return !_A.equals(_B);
}

template<typename Type, int Rows, int Columns>
bool operator==(const gs_matrix<Type, Rows, Columns>& _A, const gs_matrix<Type, Rows, Columns>& _B)
{
    return _A.equals(_B);
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

/**
 * @brief Matrix print function
 * @param _Matrix input matrix 
 */
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

/**
 * @brief Vector print function
 * @param _Vector input vector 
 */
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
typedef gs_2d_box<float > gs_2d_boxf;
typedef gs_2d_box<double> gs_2d_boxd;

// 2D ellipse typedefs
typedef gs_2d_ellipse<float > gs_2d_ellipsef;
typedef gs_2d_ellipse<double> gs_2d_ellipsed;

// 2D triangle typedefs
typedef gs_2d_triangle<float > gs_2d_trianglef;
typedef gs_2d_triangle<double> gs_2d_triangled;

// 2D line typedefs
typedef gs_2d_line<float > gs_2d_linef;
typedef gs_2d_line<double> gs_2d_lined;

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
int gs_color_32bit_invert(gs_color _Color);

/**
 * @brief 8 bit RGBA color construction function
 * 
 * @param _R red component
 * @param _G green component
 * @param _B blue component
 * @param _A alpha component
 * @return returns 8 bit encoded RGBA color
 */
gs_color gs_color_rgba(const gs_color& _R, const gs_color& _G, const gs_color& _B, const gs_color& _A);

/**
 * @brief 8 bit RGB color construction function
 * 
 * @param _R red component
 * @param _G green component
 * @param _B blue component
 * @return returns 8 bit encoded RGB color, alpha component is set 255
 */
gs_color gs_color_rgb(const gs_color& _R, const gs_color& _G, const gs_color& _B);

/**
 * @brief 8 bit RGBA color interpolation function
 * 
 * @param _SourceColor source color
 * @param _TargetColor target color
 * @param _Fraction fraction ranged [0; 1]
 * @return returns 8 bit encoded RGBA color interpolated between _SourceColor and _TargetColor
 */
gs_color gs_color_rbg_lerp(gs_color& _SourceColor, gs_color& _TargetColor, float& _Fraction);

/**
 * @brief 8 bit RGBA color red component extraction function
 * 
 * @param _Color input color
 * @return extracts red component out of input 8 bit RGBA color
 */
gs_color gs_color_rgba_get_r(const gs_color& _Color);

/**
 * @brief 8 bit RGBA color green component extraction function
 * 
 * @param _Color input color
 * @return extracts green component out of input 8 bit RGBA color
 */
gs_color gs_color_rgba_get_g(const gs_color& _Color);

/**
 * @brief 8 bit RGBA color blue component extraction function
 * 
 * @param _Color input color
 * @return extracts blue component out of input 8 bit RGBA color
 */
gs_color gs_color_rgba_get_b(const gs_color& _Color);

/**
 * @brief 8 bit RGBA color alpha component extraction function
 * 
 * @param _Color input color
 * @return extracts alpha component out of input 8 bit RGBA color
 */
gs_color gs_color_rgba_get_a(const gs_color& _Color);

// HSV

/**
 * @brief 8 bit HSV color construction function
 * @param _H hue component
 * @param _S saturation component
 * @param _V brightness component
 * @return returns 8 bit encoded HSV color
 */
gs_color gs_color_hsv(const gs_color& _H, const gs_color& _S, const gs_color& _V);

/**
 * @brief 8 bit HSV color hue component extraction function
 * @param _HSV input HSV color
 * @return extracts hue component out of input 8 bit HSV color
 */
gs_color gs_color_hsv_get_h(const gs_color& _HSV);

/**
 * @brief 8 bit HSV color saturation component extraction function 
 * @param _HSV input HSV color
 * @return extracts saturation component out of input 8 bit HSV color
 */
gs_color gs_color_hsv_get_s(const gs_color& _HSV);

/**
 * @brief 8 bit HSV color brightness component extraction function
 * @param _HSV input HSV color
 * @return extracts brightness component out of input 8 bit HSV color
 */
gs_color gs_color_hsv_get_v(const gs_color& _HSV);

// HSL

/**
 * @brief 8 bit HSL color construction function
 * @param _H hue component
 * @param _S saturation component
 * @param _L brightness component
 * @return returns 8 bit encoded HSL color
 */
gs_color gs_color_hsl(const gs_color& _H, const gs_color& _S, const gs_color& _L);

/**
 * @brief 8 bit HSL color hue component extraction function
 * @param _HSL input HSL color
 * @return extracts hue component out of input 8 bit HSL color
 */
gs_color gs_color_hsl_get_h(const gs_color& _HSL);

/**
 * @brief 8 bit HSL color saturation component extraction function
 * @param _HSL input HSL color
 * @return extracts saturation component out of input 8 bit HSL color
 */
gs_color gs_color_hsl_get_s(const gs_color& _HSL);

/**
 * @brief 8 bit HSL color brightness component extraction function
 * @param _HSL input HSL color
 * @return extracts brightness component out of input 8 bit HSL color
 */
gs_color gs_color_hsl_get_l(const gs_color& _HSL);

// HSV/RGBA

/**
 * @brief Converts RGB to HSV
 * @param _RGB input 8 bit RGB color 
 * @return returns HSV 8 bit color
 */
gs_color gs_color_rgb_to_hsv(const gs_color& _RGB);

/**
 * @brief Converts RGB to HSL
 * @param _RGB input 8 bit RGB color 
 * @return returns HSL 8 bit color
 */
gs_color gs_color_rgb_to_hsl(const gs_color& _RGB);

/**
 * @brief Converts HSV to RGB
 * @param _HSV input 8 bit HSV color 
 * @return returns RGB 8 bit color
 */
gs_color gs_color_hsv_to_rgb(const gs_color& _HSV);

/**
 * @brief Converts HSL to RGB
 * @param _HSL input 8 bit HSL color 
 * @return returns RGB 8 bit color
 */
gs_color gs_color_hsl_to_rgb(const gs_color& _HSL);

/**
 * @brief Converts HSV to HSL
 * @param _HSV input 8 bit HSV color 
 * @return returns HSL 8 bit color
 */
gs_color gs_color_hsv_to_hsl(const gs_color& _HSV);

/**
 * @brief Converts HSL to HSV
 * @param _HSL input 8 bit HSL color 
 * @return returns HSV 8 bit color
 */
gs_color gs_color_hsl_to_hsv(const gs_color& _HSL);


/*! @} */

// undef all macro
#undef GS_TO_DEGREES_CONVERSION_MULTIPLYER__
#undef GS_TO_RADIANS_CONVERSION_MULTIPLYER__