/*
 * AffineMatrix3.h
 * 
 * This file is part of the "GaussianLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GS_AFFINE_MATRIX3_H__
#define __GS_AFFINE_MATRIX3_H__


#include "Real.h"
#include "Assert.h"
#include "Macros.h"
#include "Matrix.h"
#include "Tags.h"
#include "AffineMatrix.h"

#include <cmath>
#include <cstring>
#include <algorithm>


namespace Gs
{


#ifdef GS_ROW_MAJOR_STORAGE
#   define __GS_FOREACH_ROW_COL__(r, c)                                     \
        for (std::size_t r = 0; r < AffineMatrix3T<T>::rowsSparse; ++r)     \
        for (std::size_t c = 0; c < AffineMatrix3T<T>::columnsSparse; ++c)
#else
#   define __GS_FOREACH_ROW_COL__(r, c)                                     \
        for (std::size_t c = 0; c < AffineMatrix3T<T>::columnsSparse; ++c)  \
        for (std::size_t r = 0; r < AffineMatrix3T<T>::rowsSparse; ++r)
#endif

/**
This is an affine 3x3 matrix for affine transformations,
i.e. it can only contain translations, scaling, rotations and shearing.
It only stores a 2x3 matrix where the 3rd row is always implicitly (0, 0, 1).
\tparam T Specifies the data type of the matrix components.
This should be a primitive data type such as float, double, int etc.
\remarks The macro GS_ROW_MAJOR_STORAGE can be defined, to use row-major storage layout.
By default column-major storage layout is used.
The macro GS_ROW_VECTORS can be defined, to use row vectors. By default column vectors are used.
Here is an example, how an affine 3x3 matrix is laid-out with column- and row vectors:
\code
// Affine 3x3 matrix with column vectors:
// / x1 y1 z1 \
// | x2 y2 z2 |
// \  0  0  1 /

// Affine 3x3 matrix with row vectors:
// / x1 x2 0 \
// | y1 y2 0 |
// \ z1 z2 1 /

// In both cases, (z1, z2, 1) stores the position in the affine transformation.
\endcode
*/
template <typename T>
class AffineMatrix3T
{
    
    public:
        
        static const std::size_t rows           = 3;
        static const std::size_t columns        = 3;
        static const std::size_t elements       = AffineMatrix3T<T>::rows*AffineMatrix3T<T>::columns;

        #ifdef GS_ROW_VECTORS
        static const std::size_t rowsSparse     = 3;
        static const std::size_t columnsSparse  = 2;
        #else
        static const std::size_t rowsSparse     = 2;
        static const std::size_t columnsSparse  = 3;
        #endif

        static const std::size_t elementsSparse = AffineMatrix3T<T>::rowsSparse*AffineMatrix3T<T>::columnsSparse;

        using ThisType = AffineMatrix3T<T>;
        using TransposedType = Matrix<T, AffineMatrix3T<T>::rows, AffineMatrix3T<T>::columns>;

        class Initializer
        {
            
            public:
                
                Initializer(ThisType& matrix) :
                    matrix_ ( matrix ),
                    element_( 0      )
                {
                }

                Initializer& operator , (const T& nextValue)
                {
                    matrix_(element_ / AffineMatrix3T<T>::columnsSparse, element_ % AffineMatrix3T<T>::columnsSparse) = nextValue;
                    ++element_;
                    return *this;
                }

            private:

                ThisType&   matrix_;
                std::size_t element_;

        };

        AffineMatrix3T()
        {
            #ifndef GS_ENABLE_AUTO_INIT
            Reset();
            #endif
        }

        AffineMatrix3T(const ThisType& rhs)
        {
            *this = rhs;
        }

        #ifdef GS_ROW_VECTORS

        AffineMatrix3T(
            const T& m11, const T& m12,
            const T& m21, const T& m22,
            const T& m31, const T& m32)
        {
            (*this)(0, 0) = m11; (*this)(0, 1) = m12;
            (*this)(1, 0) = m21; (*this)(1, 1) = m22;
            (*this)(2, 0) = m31; (*this)(2, 1) = m32;
        }

        #else

        AffineMatrix3T(
            const T& m11, const T& m12, const T& m13,
            const T& m21, const T& m22, const T& m23)
        {
            (*this)(0, 0) = m11; (*this)(0, 1) = m12; (*this)(0, 2) = m13;
            (*this)(1, 0) = m21; (*this)(1, 1) = m22; (*this)(1, 2) = m23;
        }

        #endif

        AffineMatrix3T(UninitializeTag)
        {
            // do nothing
        }

        /**
        \brief Returns a reference to the element at the specified entry.
        \param[in] row Specifies the row index. This must be in the range [0, 1], or [0, 2] if GS_ROW_VECTORS is defined.
        \param[in] col Specifies the column index. This must be in the range [0, 2], or [0, 1] if GS_ROW_VECTORS is defined.
        */
        T& operator () (std::size_t row, std::size_t col)
        {
            GS_ASSERT(row < AffineMatrix3T<T>::rowsSparse);
            GS_ASSERT(col < AffineMatrix3T<T>::columnsSparse);
            #ifdef GS_ROW_MAJOR_STORAGE
            return m_[row*AffineMatrix3T<T>::columnsSparse + col];
            #else
            return m_[col*AffineMatrix3T<T>::rowsSparse + row];
            #endif
        }

        /**
        \brief Returns a constant reference to the element at the specified entry.
        \param[in] row Specifies the row index. This must be in the range [0, 1], or [0, 2] if GS_ROW_VECTORS is defined.
        \param[in] col Specifies the column index. This must be in the range [0, 2], or [0, 1] if GS_ROW_VECTORS is defined.
        */
        const T& operator () (std::size_t row, std::size_t col) const
        {
            GS_ASSERT(row < AffineMatrix3T<T>::rowsSparse);
            GS_ASSERT(col < AffineMatrix3T<T>::columnsSparse);
            #ifdef GS_ROW_MAJOR_STORAGE
            return m_[row*AffineMatrix3T<T>::columnsSparse + col];
            #else
            return m_[col*AffineMatrix3T<T>::rowsSparse + row];
            #endif
        }

        T& operator [] (std::size_t element)
        {
            GS_ASSERT(element < AffineMatrix3T<T>::elementsSparse);
            return m_[element];
        }

        const T& operator [] (std::size_t element) const
        {
            GS_ASSERT(element < AffineMatrix3T<T>::elementsSparse);
            return m_[element];
        }

        ThisType& operator *= (const ThisType& rhs)
        {
            *this = (*this * rhs);
            return *this;
        }

        ThisType& operator = (const ThisType& rhs)
        {
            for (std::size_t i = 0; i < ThisType::elementsSparse; ++i)
                m_[i] = rhs.m_[i];
            return *this;
        }

        #ifdef GS_ROW_VECTORS

        T& At(std::size_t col, std::size_t row)
        {
            return (*this)(row, col);
        }

        const T& At(std::size_t col, std::size_t row) const
        {
            return (*this)(row, col);
        }

        #else

        T& At(std::size_t row, std::size_t col)
        {
            return (*this)(row, col);
        }

        const T& At(std::size_t row, std::size_t col) const
        {
            return (*this)(row, col);
        }

        #endif

        void Reset()
        {
            for (std::size_t i = 0; i < ThisType::elementsSparse; ++i)
                m_[i] = T(0);
        }

        void LoadIdentity()
        {
            __GS_FOREACH_ROW_COL__(r, c)
            {
                (*this)(r, c) = (r == c ? T(1) : T(0));
            }
        }

        static ThisType Identity()
        {
            ThisType result;
            result.LoadIdentity();
            return result;
        }

        TransposedType Transposed() const
        {
            TransposedType result;

            __GS_FOREACH_ROW_COL__(r, c)
            {
                result(c, r) = (*this)(r, c);
            }

            for (std::size_t i = 0; i < ThisType::columnsSparse; ++i)
                result(i, ThisType::rowsSparse);

            return result;
        }

        T Determinant() const
        {
            return Gs::Determinant(*this);
        }

        //! Returns the trace of this matrix: M(0, 0) + M(1, 1) + 1.
        T Trace() const
        {
            return (*this)(0, 0) + (*this)(1, 1) + T(1);
        }

        AffineMatrix3T<T> Inverse() const
        {
            AffineMatrix3T<T> inv{ *this };
            inv.MakeInverse();
            return inv;
        }

        bool MakeInverse()
        {
            AffineMatrix3T<T> in{ *this };
            return Gs::Inverse(*this, in);
        }

        //! Returns a pointer to the first element of this matrix.
        T* Ptr()
        {
            return &(m_[0]);
        }

        //! Returns a constant pointer to the first element of this matrix.
        const T* Ptr() const
        {
            return &(m_[0]);
        }

        /* --- Extended functions for affine transformations --- */

        void SetPosition(const Vector2T<T>& position)
        {
            At(0, 2) = position.x;
            At(1, 2) = position.y;
        }

        Vector2T<T> GetPosition() const
        {
            return Vector2T<T>(At(0, 2), At(1, 2));
        }

    private:
        
        T m_[ThisType::elementsSparse];

};

#undef __GS_FOREACH_ROW_COL__


/* --- Global Operators --- */

template <typename T>
AffineMatrix3T<T> operator * (const AffineMatrix3T<T>& lhs, const AffineMatrix3T<T>& rhs)
{
    return Details::MulAffineMatrices(lhs, rhs);
}

template <typename T, typename I>
typename AffineMatrix3T<T>::Initializer operator << (AffineMatrix3T<T>& matrix, const I& firstValue)
{
    typename AffineMatrix3T<T>::Initializer initializer(matrix);
    initializer , static_cast<T>(firstValue);
    return initializer;
}


/* --- Type Alias --- */

using AffineMatrix3     = AffineMatrix3T<Real>;
using AffineMatrix3f    = AffineMatrix3T<float>;
using AffineMatrix3d    = AffineMatrix3T<double>;
using AffineMatrix3i    = AffineMatrix3T<int>;


} // /namespace Gs


#endif



// ================================================================================