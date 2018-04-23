//=============================================================================

#ifndef ASPECT_RATIO_H
#define ASPECT_RATIO_H


//== INCLUDES =================================================================

#include <graphene/geometry/Vector.h>


//== NAMESPACES ===============================================================

namespace graphene {
namespace geometry {


//== IMPLEMENTATION ==========================================================


/// return aspect ratio of triangle
template <typename Scalar>
Scalar
aspect_ratio(const Vector<Scalar,3> & _v0,
             const Vector<Scalar,3> & _v1,
             const Vector<Scalar,3> & _v2)
{
    Vector<Scalar,3> d0 = _v0 - _v1;
    Vector<Scalar,3> d1 = _v1 - _v2;

    // max squared edge length
    Scalar l2, maxl2 = sqrnorm(d0);
    if ((l2=sqrnorm(d1)) > maxl2)  maxl2 = l2;
    d1 = _v2 - _v0;
    if ((l2=sqrnorm(d1)) > maxl2)  maxl2 = l2;

    // triangle area
    Scalar a2 = sqrnorm(cross(d0,d1));

    // min height is area/max_length
    // aspect ratio = length / height
    //              = length * length / area
    //              = sqrt(maxl2*maxl2/a2)
    return sqrt( (maxl2 *= maxl2) /= a2 );
}


//-----------------------------------------------------------------------------


/// return roundness of triangle: 1=equilateral, 0=colinear
template <typename Scalar>
Scalar
roundness(const Vector<Scalar,3> & _v0,
          const Vector<Scalar,3> & _v1,
          const Vector<Scalar,3> & _v2)
{
    const Scalar FOUR_ROOT3_OVER2 = 3.46410161513775;

    const Vector<Scalar,3> d0 = _v0 - _v1;
    const Vector<Scalar,3> d1 = _v1 - _v2;
    const Vector<Scalar,3> d2 = _v2 - _v0;

    Scalar area = norm(cross(d0,d1));
    area *= FOUR_ROOT3_OVER2;

    Scalar demon = sqrnorm(d0);
    demon += sqrnorm(d1);
    demon += sqrnorm(d2);

    return area /= demon;
}


//=============================================================================
} // namespace geometry
} // namespace graphene
//=============================================================================
#endif
//=============================================================================
