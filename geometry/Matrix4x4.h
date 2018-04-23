//=============================================================================

#ifndef MAT4_H
#define MAT4_H


//== INCLUDES =================================================================

#include <graphene/geometry/Vector.h>
#include <math.h>
#ifdef _WIN32
#define M_PI 3.14159265
#endif
#include <iostream>


//== NAMESPACE ================================================================

namespace graphene {


//== CLASS DEFINITION =========================================================


/** Simple 4x4 matrix.
 */

template <class Scalar>
class Mat4
{
public:

	/// constructor
	Mat4() {}

    /// construct from 4 column vectors
    Mat4(Vector<Scalar,4> c0, Vector<Scalar,4> c1, Vector<Scalar,4> c2, Vector<Scalar,4> c3)
    {
        (*this)(0,0) = c0[0]; (*this)(0,1) = c1[0]; (*this)(0,2) = c2[0]; (*this)(0,3) = c3[0];
        (*this)(1,0) = c0[1]; (*this)(1,1) = c1[1]; (*this)(1,2) = c2[1]; (*this)(1,3) = c3[1];
        (*this)(2,0) = c0[2]; (*this)(2,1) = c1[2]; (*this)(2,2) = c2[2]; (*this)(2,3) = c3[2];
        (*this)(3,0) = c0[3]; (*this)(3,1) = c1[3]; (*this)(3,2) = c2[3]; (*this)(3,3) = c3[3];
    }

    /// construct from 16 (row-wise) entries
    Mat4(Scalar m00, Scalar m01, Scalar m02, Scalar m03,
         Scalar m10, Scalar m11, Scalar m12, Scalar m13,
         Scalar m20, Scalar m21, Scalar m22, Scalar m23,
         Scalar m30, Scalar m31, Scalar m32, Scalar m33)
    {
        (*this)(0,0) = m00; (*this)(0,1) = m01; (*this)(0,2) = m02; (*this)(0,3) = m03;
        (*this)(1,0) = m10; (*this)(1,1) = m11; (*this)(1,2) = m12; (*this)(1,3) = m13;
        (*this)(2,0) = m20; (*this)(2,1) = m21; (*this)(2,2) = m22; (*this)(2,3) = m23;
        (*this)(3,0) = m30; (*this)(3,1) = m31; (*this)(3,2) = m32; (*this)(3,3) = m33;
    }

    /// construct from scalar. sets all matrix entries to s.
    Mat4(Scalar s)
    {
        int i;
        for (i=0; i<16; ++i)
        {
            data_[i] = s;
        }
    }

    /// cast to matrix of other scalar type
    template <typename T> explicit operator Mat4<T>()
    {
        Mat4<T> m;
        for (int i=0; i<4; ++i)
            for (int j=0; j<4; ++j)
                m(i,j) = static_cast<T>((*this)(i,j));
        return m;
    }

	/// destructor
	~Mat4() {}

	/// access entry at row i and column j
	Scalar& operator()(unsigned int i, unsigned int j)
    {
		return data_[(j<<2) + i];
	}

    /// const-access entry at row i and column j
	const Scalar& operator()(unsigned int i, unsigned int j) const
    {
		return data_[(j<<2) + i];
	}

    /// this = s / this
    Mat4& operator/=(const Scalar s)
    {
        int i,j; Scalar is(1.0/s);
        for (i=0; i<4; ++i) for (j=0; j<4; ++j)  (*this)(i,j) *= is;
        return *this;
    }

    /// this = s * this
    Mat4& operator*=(const Scalar s)
    {
        int i,j;
        for (i=0; i<4; ++i) for (j=0; j<4; ++j)  (*this)(i,j) *= s;
        return *this;
    }

    /// const-access as scalar array
	const Scalar* data() const { return data_; }

    /// access as scalar array
    Scalar* data()  { return data_; }

    /// return zero matrix
    static Mat4<Scalar> zero();

    /// return identity matrix
    static Mat4<Scalar> identity();

    static Mat4<Scalar> frustum(Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f);

    static Mat4<Scalar> perspective(Scalar fovy, Scalar aspect, Scalar near, Scalar far);

    static Mat4<Scalar> ortho(Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar zNear, Scalar zFar);

    static Mat4<Scalar> look_at(const Vector<Scalar ,3> &eye, const Vector<Scalar ,3> &center, const Vector<Scalar ,3> &up);

    static Mat4<Scalar> translate(const Vector<Scalar ,3> &t);

    static Mat4<Scalar> rotate(const Vector<Scalar ,3> &axis, Scalar angle);


private:

	Scalar data_[16];
};


//-----------------------------------------------------------------------------


typedef Mat4<float>  Mat4f;
typedef Mat4<double> Mat4d;


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::identity()
{
    Mat4<Scalar> m;

    for (int j=0; j<4; ++j)
        for (int i=0; i<4; ++i)
            m(i,j) = 0.0;

    m(0,0) = m(1,1) = m(2,2) = m(3,3) = 1.0;

    return m;
}



//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::zero()
{
    Mat4<Scalar> m;

    for (int j=0; j<4; ++j)
        for (int i=0; i<4; ++i)
            m(i,j) = 0.0;

    return m;
}

//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::frustum(Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f)
{
    Mat4<Scalar> m(Scalar(0));

    m(0,0) = (n+n) / (r-l);
    m(0,2) = (r+l) / (r-l);
    m(1,1) = (n+n) / (t-b);
    m(1,2) = (t+b) / (t-b);
    m(2,2) = -(f+n) / (f-n);
    m(2,3) = -f * (n+n) / (f-n);
    m(3,2) = -1.0f;

    return m;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::perspective(Scalar fovy, Scalar aspect, Scalar near, Scalar far)
{
    Scalar t = near * tan( fovy * M_PI / 360.0 );
    Scalar b = -t;
    Scalar l = b * aspect;
    Scalar r = t * aspect;

    return Mat4<Scalar>::frustum(l, r, b, t, near, far);
}


//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::ortho(Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar zNear, Scalar zFar)
{
    Mat4<Scalar> m(0.0);

    m(0,0) = Scalar(2) / (right - left);
    m(1,1) = Scalar(2) / (top - bottom);
    m(2,2) = - Scalar(2) / (zFar - zNear);
    m(0,3) = - (right + left) / (right - left);
    m(1,3) = - (top + bottom) / (top - bottom);
    m(2,3) = - (zFar + zNear) / (zFar - zNear);
    m(3,3) = Scalar(1);

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::look_at(const Vector<Scalar ,3>& eye, const Vector<Scalar ,3>& center, const Vector<Scalar ,3>& up)
{

    Vector<Scalar,3> z = normalize(eye-center);
    Vector<Scalar,3> x = normalize(cross(up, z));
    Vector<Scalar,3> y = normalize(cross(z, x));

    Mat4<Scalar> m;
    m(0,0)=x[0]; m(0,1)=x[1]; m(0,2)=x[2]; m(0,3)=-dot(x,eye);
    m(1,0)=y[0]; m(1,1)=y[1]; m(1,2)=y[2]; m(1,3)=-dot(y,eye);
    m(2,0)=z[0]; m(2,1)=z[1]; m(2,2)=z[2]; m(2,3)=-dot(z,eye);
    m(3,0)=0.0;  m(3,1)=0.0;  m(3,2)=0.0;  m(3,3)=1.0;

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::translate(const Vector<Scalar ,3>& t)
{
    Mat4<Scalar> m(Scalar(0));
    m(0,0) = m(1,1) = m(2,2) = m(3,3) = 1.0f;
    m(0,3) = t[0];
    m(1,3) = t[1];
    m(2,3) = t[2];

    return m;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
Mat4<Scalar>
Mat4<Scalar>::rotate(const Vector<Scalar ,3>& axis, Scalar angle)
{
    Mat4<Scalar> m(Scalar(0));
    Scalar a = angle * (M_PI/180.0f);
    Scalar c = cosf(a);
    Scalar s = sinf(a);
    Scalar one_m_c = Scalar(1) - c;
    Vector<Scalar, 3> ax = normalize(axis);

    m(0,0) = ax[0]*ax[0] * one_m_c + c;
    m(0,1) = ax[0]*ax[1] * one_m_c - ax[2] * s;
    m(0,2) = ax[0]*ax[2] * one_m_c + ax[1] * s;

    m(1,0) = ax[1]*ax[0] * one_m_c + ax[2] * s;
    m(1,1) = ax[1]*ax[1] * one_m_c + c;
    m(1,2) = ax[1]*ax[2] * one_m_c - ax[0] * s;

    m(2,0) = ax[2]*ax[0] * one_m_c - ax[1] * s;
    m(2,1) = ax[2]*ax[1] * one_m_c + ax[0] * s;
    m(2,2) = ax[2]*ax[2] * one_m_c + c;

    m(3,3) = 1.0f;

    return m;
}

//-----------------------------------------------------------------------------


/// output matrix to ostream os
template <typename Scalar>
std::ostream&
operator<<(std::ostream& os, const Mat4<Scalar>& m)
{
  for(int i=0; i<4; i++)
  {
    for(int j=0; j<4; j++)
      os << m(i,j) << " ";
    os << "\n";
  }
  return os;
}


//-----------------------------------------------------------------------------


/// read the space-separated components of a vector from a stream */
template <typename Scalar>
std::istream&
operator>>(std::istream& is, Mat4<Scalar>& m)
{
  for(int i=0; i<4; i++)
    for(int j=0; j<4; j++)
      is >> m(i,j);
  return is;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Vector<Scalar,3>
affine_transform(const Mat4<Scalar>& m, const Vector<Scalar,3>& v)
{
    const Scalar x = m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2] + m(0,3);
    const Scalar y = m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2] + m(1,3);
    const Scalar z = m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2] + m(2,3);
    return Vector<Scalar,3>(x,y,z);
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Vector<Scalar,3>
linear_transform(const Mat4<Scalar>& m, const Vector<Scalar,3>& v)
{
    const Scalar x = m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2];
    const Scalar y = m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2];
    const Scalar z = m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2];
    return Vector<Scalar,3>(x,y,z);
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
operator*(const Mat4<Scalar>& m0, const Mat4<Scalar>& m1)
{
    Mat4<Scalar> m;

    for (int i=0; i<4; ++i)
    {
        for (int j=0; j<4; ++j)
        {
            m(i,j) = Scalar(0);
            for (int k=0; k<4; ++k)
                m(i,j) += m0(i,k) * m1(k,j);
        }
    }

    return m;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Vector<Scalar,4>
operator*(const Mat4<Scalar>& m, const Vector<Scalar,4>& v)
{
    const Scalar x = m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2] + m(0,3)*v[3];
    const Scalar y = m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2] + m(1,3)*v[3];
    const Scalar z = m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2] + m(2,3)*v[3];
    const Scalar w = m(3,0)*v[0] + m(3,1)*v[1] + m(3,2)*v[2] + m(3,3)*v[3];

    return Vector<Scalar,4> (x,y,z,w);
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
inverse(const Mat4<Scalar>& m)
{

    Scalar Coef00 = m(2,2) * m(3,3) - m(2,3) * m(3,2);
    Scalar Coef02 = m(2,1) * m(3,3) - m(2,3) * m(3,1);
    Scalar Coef03 = m(2,1) * m(3,2) - m(2,2) * m(3,1);

    Scalar Coef04 = m(1,2) * m(3,3) - m(1,3) * m(3,2);
    Scalar Coef06 = m(1,1) * m(3,3) - m(1,3) * m(3,1);
    Scalar Coef07 = m(1,1) * m(3,2) - m(1,2) * m(3,1);

    Scalar Coef08 = m(1,2) * m(2,3) - m(1,3) * m(2,2);
    Scalar Coef10 = m(1,1) * m(2,3) - m(1,3) * m(2,1);
    Scalar Coef11 = m(1,1) * m(2,2) - m(1,2) * m(2,1);

    Scalar Coef12 = m(0,2) * m(3,3) - m(0,3) * m(3,2);
    Scalar Coef14 = m(0,1) * m(3,3) - m(0,3) * m(3,1);
    Scalar Coef15 = m(0,1) * m(3,2) - m(0,2) * m(3,1);

    Scalar Coef16 = m(0,2) * m(2,3) - m(0,3) * m(2,2);
    Scalar Coef18 = m(0,1) * m(2,3) - m(0,3) * m(2,1);
    Scalar Coef19 = m(0,1) * m(2,2) - m(0,2) * m(2,1);

    Scalar Coef20 = m(0,2) * m(1,3) - m(0,3) * m(1,2);
    Scalar Coef22 = m(0,1) * m(1,3) - m(0,3) * m(1,1);
    Scalar Coef23 = m(0,1) * m(1,2) - m(0,2) * m(1,1);

    Vector<Scalar,4> const SignA(+1, -1, +1, -1);
    Vector<Scalar,4> const SignB(-1, +1, -1, +1);

    Vector<Scalar,4> Fac0(Coef00, Coef00, Coef02, Coef03);
    Vector<Scalar,4> Fac1(Coef04, Coef04, Coef06, Coef07);
    Vector<Scalar,4> Fac2(Coef08, Coef08, Coef10, Coef11);
    Vector<Scalar,4> Fac3(Coef12, Coef12, Coef14, Coef15);
    Vector<Scalar,4> Fac4(Coef16, Coef16, Coef18, Coef19);
    Vector<Scalar,4> Fac5(Coef20, Coef20, Coef22, Coef23);

    Vector<Scalar,4> Vec0(m(0,1), m(0,0), m(0,0), m(0,0));
    Vector<Scalar,4> Vec1(m(1,1), m(1,0), m(1,0), m(1,0));
    Vector<Scalar,4> Vec2(m(2,1), m(2,0), m(2,0), m(2,0));
    Vector<Scalar,4> Vec3(m(3,1), m(3,0), m(3,0), m(3,0));

    Vector<Scalar,4> Inv0 = SignA * (Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
    Vector<Scalar,4> Inv1 = SignB * (Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
    Vector<Scalar,4> Inv2 = SignA * (Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
    Vector<Scalar,4> Inv3 = SignB * (Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

    Mat4<Scalar> Inverse(Inv0, Inv1, Inv2, Inv3);

    Vector<Scalar,4> Row0(Inverse(0,0), Inverse(1,0), Inverse(2,0), Inverse(3,0));
    Vector<Scalar,4> Col0(m(0,0), m(0,1), m(0,2), m(0,3));

    Scalar Determinant = dot(Col0, Row0);

    Inverse /= Determinant;

    return Inverse;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat4<Scalar>
transpose(const Mat4<Scalar>& m)
{
    Mat4<Scalar> result;

    for (int j=0; j<4; ++j)
        for (int i=0; i<4; ++i)
            result(i,j) = m(j,i);

    return result;
}


//=============================================================================
} // namespace graphene
//=============================================================================
#endif
//=============================================================================

