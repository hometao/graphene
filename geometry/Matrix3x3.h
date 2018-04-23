//=============================================================================

#ifndef MAT3_H
#define MAT3_H


//== INCLUDES =================================================================

#include <graphene/geometry/Vector.h>
#include <graphene/geometry/Matrix4x4.h>
#include <math.h>
#include <iostream>


//== NAMESPACE ================================================================

namespace graphene {


//== CLASS DEFINITION =========================================================


/** Simple 3x3 matrix.
 */

template <class Scalar>
class Mat3
{
public:

    /// constructor
    Mat3() {}

    Mat3(Scalar s)
    {
        int i,j;
        for (i=0; i<3; ++i) {
            for (j=0; j<3; ++j) {
                data_[j*3+i]= s;
            }
        }
    }

    Mat3(bool identity) :
        Mat3(Scalar(0))
    {
        if (identity) {
            (*this)(0,0) = (*this)(1,1) = (*this)(2,2) = (*this)(3,3) = 1.0;
        }
    }

    Mat3(const Mat4<Scalar>& m)
    {
        int i,j;
        for (i=0; i<3; ++i)
            for (j=0; j<3; ++j)
                (*this)(i,j) = m(i,j);
    }

    /// destructor
    ~Mat3() {}

    /// access entry at row i and column j
    Scalar& operator()(unsigned int i, unsigned int j)
    {
        return data_[j*3+i];
    }

    /// const-access entry at row i and column j
    const Scalar& operator()(unsigned int i, unsigned int j) const
    {
        return data_[j*3+i];
    }

    /// return zero matrix
    static Mat3<Scalar> zero();

    /// return identity matrix
    static Mat3<Scalar> identity();

    /// this = this * m
    Mat3& operator*=(const Mat3& m)
    {
        return (*this = *this * m);
    };

    /// this = this + m
    Mat3& operator+=(const Mat3& m)
    {
        int i,j;
        for (i=0; i<3; ++i) for (j=0; j<3; ++j)  data_[j*3+i] += m.data_[j*3+i];
        return *this;
    };

    /// this = this - m
    Mat3& operator-=(const Mat3& m)
    {
        int i,j;
        for (i=0; i<3; ++i) for (j=0; j<3; ++j)  data_[j*3+i] -= m.data_[j*3+i];
        return *this;
    };

    /// this = s * this
    Mat3& operator*=(const Scalar s)
    {
        int i,j;
        for (i=0; i<3; ++i) for (j=0; j<3; ++j)  data_[j*3+i] *= s;
        return *this;
    };

    /// this = s / this
    Mat3& operator/=(const Scalar s)
    {
        int i,j; Scalar is(1.0/s);
        for (i=0; i<3; ++i) for (j=0; j<3; ++j)  data_[j*3+i] *= is;
        return *this;
    };


    /// const access to array
    const Scalar* data() const { return data_; }

    /// acces to array
    Scalar* data() { return data_; }

private:

    Scalar data_[9];
};


//-----------------------------------------------------------------------------


typedef Mat3<float>  Mat3f;
typedef Mat3<double> Mat3d;


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat3<Scalar>
Mat3<Scalar>::identity()
{
    Mat3<Scalar> m;

    for (int j=0; j<3; ++j)
        for (int i=0; i<3; ++i)
            m(i,j) = 0.0;

    m(0,0) = m(1,1) = m(2,2) = 1.0;

    return m;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat3<Scalar>
Mat3<Scalar>::zero()
{
    Mat3<Scalar> m;

    for (int j=0; j<3; ++j)
        for (int i=0; i<3; ++i)
            m(i,j) = 0.0;

    return m;
}


//-----------------------------------------------------------------------------

template <typename Scalar>
Mat3<Scalar>
inverse(const Mat3<Scalar> &m)
{
    float det = (- m(0,0)*m(1,1)*m(2,2)
                 + m(0,0)*m(1,2)*m(2,1)
                 + m(1,0)*m(0,1)*m(2,2)
                 - m(1,0)*m(0,2)*m(2,1)
                 - m(2,0)*m(0,1)*m(1,2)
                 + m(2,0)*m(0,2)*m(1,1));

    Mat3<Scalar> inv;
    inv(0,0) = (m(1,2)*m(2,1) - m(1,1)*m(2,2)) / det;
    inv(0,1) = (m(0,1)*m(2,2) - m(0,2)*m(2,1)) / det;
    inv(0,2) = (m(0,2)*m(1,1) - m(0,1)*m(1,2)) / det;
    inv(1,0) = (m(1,0)*m(2,2) - m(1,2)*m(2,0)) / det;
    inv(1,1) = (m(0,2)*m(2,0) - m(0,0)*m(2,2)) / det;
    inv(1,2) = (m(0,0)*m(1,2) - m(0,2)*m(1,0)) / det;
    inv(2,0) = (m(1,1)*m(2,0) - m(1,0)*m(2,1)) / det;
    inv(2,1) = (m(0,0)*m(2,1) - m(0,1)*m(2,0)) / det;
    inv(2,2) = (m(0,1)*m(1,0) - m(0,0)*m(1,1)) / det;

    return inv;
}

//-----------------------------------------------------------------------------

template <typename Scalar>
const Mat3<Scalar>
outer_product(const Vector<Scalar,3>& a, const Vector<Scalar,3>& b)
{
    Mat3<Scalar> m;

    for (int j=0; j<3; ++j)
        for (int i=0; i<3; ++i)
            m(i,j) = a[i]*b[j];

    return m;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat3<Scalar>
transpose(const Mat3<Scalar>& m)
{
    Mat3<Scalar> result;

    for (int j=0; j<3; ++j)
        for (int i=0; i<3; ++i)
            result(i,j) = m(j,i);

    return result;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
Mat3<Scalar>
operator*(const Mat3<Scalar>& m0, const Mat3<Scalar>& m1)
{
    Mat3<Scalar> m;

    for (int i=0; i<3; ++i)
    {
        for (int j=0; j<3; ++j)
        {
            m(i,j) = 0.0f;
            for (int k=0; k<3; ++k)
            {
                m(i,j) += m0(i,k) * m1(k,j);
            }
        }
    }

    return m;
}


//-----------------------------------------------------------------------------


/// output matrix to ostream os
template <typename Scalar>
std::ostream&
operator<<(std::ostream& os, const Mat3<Scalar>& m)
{
  for(int i=0; i<3; i++)
  {
    for(int j=0; j<3; j++)
      os << m(i,j) << " ";
    os << "\n";
  }
  return os;
}


//-----------------------------------------------------------------------------


/// read the space-separated components of a vector from a stream */
template <typename Scalar>
std::istream&
operator>>(std::istream& is, Mat3<Scalar>& m)
{
  for(int i=0; i<3; i++)
    for(int j=0; j<3; j++)
      is >> m(i,j);
  return is;
}


//-----------------------------------------------------------------------------


template <typename Scalar>
bool
symmetric_eigendecomposition(const Mat3<Scalar>& m,
                             Scalar& eval1,
                             Scalar& eval2,
                             Scalar& eval3,
                             Vector<Scalar,3>& evec1,
                             Vector<Scalar,3>& evec2,
                             Vector<Scalar,3>& evec3)
{
    unsigned int   i, j;
    Scalar         theta, t, c, s;
    Mat3<Scalar>   V = Mat3<Scalar>::identity();
    Mat3<Scalar>   R;
    Mat3<Scalar>   A=m;
    const Scalar   eps = 1e-10;//0.000001;


    int iterations = 100;
    while (iterations--)
    {
        // find largest off-diagonal elem
        if (fabs(A(0,1)) < fabs(A(0,2)))
        {
            if (fabs(A(0,2)) < fabs(A(1,2)))
            {
                i = 1, j = 2;
            }
            else
            {
                i = 0, j = 2;
            }
        }
        else
        {
            if (fabs(A(0,1)) < fabs(A(1,2)))
            {
                i = 1, j = 2;
            }
            else
            {
                i = 0, j = 1;
            }
        }


        // converged?
        if (fabs(A(i,j)) < eps) break;


        // compute Jacobi-Rotation
        theta = 0.5 * (A(j,j) - A(i,i)) / A(i,j);
        t = 1.0 / ( fabs( theta ) + sqrt( 1.0 + theta*theta ) );
        if (theta < 0.0) t = -t;

        c = 1.0 / sqrt(1.0 + t*t);
        s = t*c;

        R = Mat3<Scalar>::identity();
        R(i,i) = R(j,j) = c;
        R(i,j) = s;
        R(j,i) = -s;

        A = transpose(R) * A * R;
        V *= R;
    }


    if (iterations > 0)
    {

        // sort and return
        int sorted[3];
        Scalar d[3]={A(0,0), A(1,1), A(2,2)};

        if (d[0] > d[1])
        {
            if (d[1] > d[2])
            {
                sorted[0] = 0, sorted[1] = 1, sorted[2] = 2;
            }
            else
            {
                if (d[0] > d[2])
                {
                    sorted[0] = 0, sorted[1] = 2, sorted[2] = 1;
                }
                else
                {
                    sorted[0] = 2, sorted[1] = 0, sorted[2] = 1;
                }
            }
        }
        else
        {
            if (d[0] > d[2])
            {
                sorted[0] = 1, sorted[1] = 0, sorted[2] = 2;
            }
            else
            {
                if (d[1] > d[2])
                {
                    sorted[0] = 1, sorted[1] = 2, sorted[2] = 0;
                }
                else
                {
                    sorted[0] = 2, sorted[1] = 1, sorted[2] = 0;
                }
            }
        }

        eval1 = d[sorted[0]];
        eval2 = d[sorted[1]];
        eval3 = d[sorted[2]];

        evec1 = Vector<Scalar,3>(V(0,sorted[0]), V(1,sorted[0]), V(2,sorted[0]));
        evec2 = Vector<Scalar,3>(V(0,sorted[1]), V(1,sorted[1]), V(2,sorted[1]));
        evec3 = normalize(cross(evec1, evec2));

        return true;
    }


    return false;
}


//=============================================================================
} // namespace graphene
//=============================================================================
#endif
//=============================================================================
