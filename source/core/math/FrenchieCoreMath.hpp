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
template<typename Type> Type  gs_huge();
template<typename Type> Type  gs_tiny();
template<typename Type> Type  gs_epsilon();

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

// [ALGEBRA]
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
            Data[i] = 0;
    }

    gs_vector(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            Data[i] = _Value;
    }

    gs_vector(const gs_vector<Type, Size>& _Other)
    {
        for (int i = 0; i < Size; i++)
            Data[i] = _Other[i];
    }

    template<int OtherSize>
    gs_vector(const gs_vector<Type, OtherSize>& _Other)
    {
        for (int i = 0; i < gs_min(OtherSize, Size); i++)
            Data[i] = _Other[i];
    }

    template <int OtherSize, typename... Args>
    gs_vector(const gs_vector<Type, OtherSize>& _Other, Args... _Args) 
    {
        int i = 0;

        for (i = 0; i < gs_min(OtherSize, Size); i++)
            Data[i] = _Other[i];

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
        return Data[_Index];
    }

    // const Type[]&
    const Type& operator[](const int& _Index) const
    {
        GS_ASSERT(_Index < Size);
        return Data[_Index];
    }

    // +=
    gs_vector<Type, Size> operator+=(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            Data[i] += _Value;
        return *this;
    }

    gs_vector<Type, Size> operator+=(const gs_vector<Type, Size>& _Value)
    {
        for (int i = 0; i < Size; i++)
            Data[i] += _Value[i];
        return *this;
    }

    // -=
    gs_vector<Type, Size> operator-=(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            Data[i] -= _Value;
        return *this;
    }

    gs_vector<Type, Size> operator-=(const gs_vector<Type, Size>& _Value)
    {
        for (int i = 0; i < Size; i++)
            Data[i] -= _Value[i];
        return *this;
    }

    // *=
    gs_vector<Type, Size> operator*=(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            Data[i] *= _Value;
        return *this;
    }

    gs_vector<Type, Size> operator*=(const gs_vector<Type, Size>& _Value)
    {
        for (int i = 0; i < Size; i++)
            Data[i] *= _Value[i];
        return *this;
    }

    // /=
    gs_vector<Type, Size> operator/=(const Type& _Value)
    {
        for (int i = 0; i < Size; i++)
            Data[i] /= _Value;
        return *this;
    }

    gs_vector<Type, Size> operator/=(const gs_vector<Type, Size>& _Value)
    {
        for (int i = 0; i < Size; i++)
            Data[i] /= _Value[i];
        return *this;
    }

    // =
    gs_vector<Type, Size>& operator=(const gs_vector<Type, Size>& _Other)
    {
        for (int i = 0; i < Size; i++)
            Data[i] = _Other[i];
        return *this;
    }

    template<int OtherSize>
    gs_vector<Type, Size>& operator=(const gs_vector<Type, OtherSize>& _Other)
    {
        for (int i = 0; i < gs_min(Size, OtherSize); i++)
            Data[i] = _Other[i];
        return *this;
    }

private:

    // service methods
    template<typename ... Args>
    void recursive_template_vector_initialization();

    template<typename... Tail>
    void recursive_template_vector_initialization(const int& _Index, const Type& _Head, Tail... _Tail) 
    {
        Data[_Index] = _Head;
        recursive_template_vector_initialization(_Index + 1, static_cast<Type>(_Tail)...);
    }

    void recursive_template_vector_initialization(const int& _Index, const Type& _Head)
    {
        Data[_Index] = _Head;
    }

    void recursive_template_vector_initialization(const int&){}
};

template<typename Type>
struct gs_rect
{
    gs_rect(
        const gs_vector<Type, 2>& _Min = gs_vector<Type, 2>(static_cast<Type>(0)),
        const gs_vector<Type, 2>& _Max = gs_vector<Type, 2>(static_cast<Type>(0))) :
    Min(gs_vector<Type, 2>(gs_min(_Min.x, _Max.x), gs_min(_Min.y, _Max.y))),
    Max(gs_vector<Type, 2>(gs_max(_Min.x, _Max.x), gs_max(_Min.y, _Max.y))){}

    gs_rect(
        const Type& _MinX = static_cast<Type>(0),
        const Type& _MinY = static_cast<Type>(0),
        const Type& _MaxX = static_cast<Type>(0),
        const Type& _MaxY = static_cast<Type>(0)) :
    Min(gs_vector<Type, 2>(gs_min(_MinX, _MaxX), gs_min(_MinY, _MaxY))),
    Max(gs_vector<Type, 2>(gs_max(_MinX, _MaxX), gs_max(_MinY, _MaxY))){}

    gs_vector<Type, 2> Min{gs_vec2f(0.f)};
    gs_vector<Type, 2> Max{gs_vec2f(0.f)};

    gs_vector<Type, 2> get_size() const
    {
        return Max - Min;
    }

    bool contains(const gs_vector<Type, 2>& _Point) const
    {
        return _Point.x >= Min.x &&
               _Point.y >= Min.y &&
               _Point.x <= Max.x &&
               _Point.y <= Max.y;
    }

    bool contains(const gs_rect<Type>& _Other) const
    {
        return _Other.Min.x >= Min.x &&
               _Other.Min.y >= Min.y &&
               _Other.Max.x <= Max.x &&
               _Other.Max.y <= Max.y;
    }

    bool overlaps(const gs_rect<Type>& _Other) const
    {
        const gs_vector<Type, 2> p1 = gs_vector<Type, 2>(_Other.Min.x, _Other.Min.y);
        const gs_vector<Type, 2> p2 = gs_vector<Type, 2>(_Other.Max.x, _Other.Min.y);
        const gs_vector<Type, 2> p3 = gs_vector<Type, 2>(_Other.Max.x, _Other.Max.y);
        const gs_vector<Type, 2> p4 = gs_vector<Type, 2>(_Other.Min.x, _Other.Max.y);
        return contains(p1) || contains(p2) || contains(p3) || contains(p4);
    }
};

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

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// binary operators
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// vectors vs vectors
template<typename Type, int Size>
gs_vector<Type, Size> operator!=(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    bool value = false;
    gs_vector<Type, Size> _C;
    for (int i = 0; i < Size; ++i)
        value |= _A[i] != _B[i];
    return value;
}

template<typename Type, int Size>
gs_vector<Type, Size> operator==(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
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

// vectors vs scalars
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

// scalars vs vectors
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

// matrix vs matrix
template<typename Type, int Rows, int Columns>
bool operator!=(const gs_matrix<Type, Rows, Columns>& _A, const gs_matrix<Type, Rows, Columns>& _B)
{
    bool output = false;
    for (int i = 0; i < Size; ++i)
        output |= _Matrix.Data[i] != Data[i];
    return output;
}

template<typename Type, int Rows, int Columns>
bool operator==(const gs_matrix<Type, Rows, Columns>& _A, const gs_matrix<Type, Rows, Columns>& _B)
{
    bool output = true;
    for (int i = 0; i < Size; ++i)
        output &= _Matrix.Data[i] == Data[i];
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

// matrix vs vector
template<typename Type, int Rows, int Columns>
gs_vector<Type, Rows> operator*(const gs_matrix<Type, Rows, Columns>& _A, const gs_vector<Type, Rows>& _V)
{
    gs_vector<Type, Rows> result(0);
    _A.mul_vec(_A, _V, result);
    return result;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

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

        if(gs_abs(vmax) <= 0) continue;

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


// [GEOMETRY]
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
inline double gs_vectors_dot(const gs_vector<Type, Size>& _A, const gs_vector<Type, Size>& _B)
{
    double dot = 0;
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

    gs_matrix<Type, 4, 4> transform;
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

template<typename T>
inline gs_matrix<T, 4, 4> gs_matrix_ortho(
    const T& left,
    const T& right,
    const T& bottom,
    const T& top,
    const T& zNear,
    const T& zFar)
{
    gs_matrix<T, 4, 4> Result(1);
    Result[0][0] = static_cast<T>(2) / (right - left);
    Result[1][1] = static_cast<T>(2) / (top - bottom);
    Result[2][2] = - static_cast<T>(2) / (zFar - zNear);
    Result[3][0] = - (right + left) / (right - left);
    Result[3][1] = - (top + bottom) / (top - bottom);
    Result[3][2] = - (zFar + zNear) / (zFar - zNear);

    return Result;
}

template<typename Type>
inline gs_matrix<Type, 4, 4> gs_matrix_perspective(
    const Type& _FieldOfView,
    const Type& _Aspect,
    const Type& _Far,
    const Type& _Near)
{
    GS_ASSERT(abs(_Aspect - gs_epsilon<Type>())> static_cast<Type>(0));
    Type const tanHalfFovy = tan(_FieldOfView / static_cast<Type>(2));
    gs_matrix<Type, 4, 4> transform(static_cast<Type>(0));
    transform[0][0] = static_cast<Type>(1) / (_Aspect * tanHalfFovy);
    transform[1][1] = static_cast<Type>(1) / (tanHalfFovy);
    transform[2][2] = - (_Far + _Near) / (_Far - _Near);
    transform[2][3] = - static_cast<Type>(1);
    transform[3][2] = - (static_cast<Type>(2) * _Far * _Near) / (_Far - _Near);
    return transform;
}

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

// rectangle typedefs
typedef gs_rect<float > gs_rectf;
typedef gs_rect<double> gs_rectd;
typedef gs_rect<int   > gs_recti;

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

// undef all macro
#undef GS_TO_DEGREES_CONVERSION_MULTIPLYER__
#undef GS_TO_RADIANS_CONVERSION_MULTIPLYER__