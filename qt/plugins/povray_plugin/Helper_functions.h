#ifndef GRAPHENE_HELPER_FUNCTIONS_H
#define GRAPHENE_HELPER_FUNCTIONS_H

#include <sstream>
#include <vector>
#include <string>

#include <graphene/geometry/Vector.h>
#include <graphene/scene_graph/Base_node.h>


// ---------------------------------------------------------------------------


using graphene::scene_graph::Base_node;

namespace graphene
{


// parses any type to string
template<typename T>
std::string to_string(T _val)
{
    std::stringstream ss;
    ss << _val;
    return ss.str();
}


// ---------------------------------------------------------------------------


// creates a comment description, used in Povray .pov files, for _node
std::string
comment_line(Base_node* _node)
{
    return "// " + _node->name() + " " + _node->fileinfo() + " D" + to_string(_node) + '\n';
}


// ---------------------------------------------------------------------------


// Change handedness of vector from right handed to left handed
// by flipping the sign of the z coordinate
Vec3f rcs2lcs(Vec3f _vec)
{
    _vec[2] *= -1;

    return _vec;
}


// ---------------------------------------------------------------------------


// converts a Vec3f into a string, useable as Povray 3d-vector
const std::string Povray_vector(const Vec3f& _vec)
{
    std::stringstream ss;
    ss << "<" << _vec[0] << "," << _vec[1] << "," << _vec[2] << ">";

    return ss.str();
}


// ---------------------------------------------------------------------------


// converts a Vec4f into a string, useable as Povray 3d-vector
const std::string Povray_vector(const Vec4f& _vec)
{
    return Povray_vector(Vec3f(_vec[0],_vec[1],_vec[2]));
}


// ---------------------------------------------------------------------------


// Converts a matrix for a right-handed coordinate system (rcs) into
// a matrix for a left-handed coordinate system (lcs).
const std::string
Povray_matrix(const double _m[16])
{

// Converts a matrix for a right-handed coordinate system (rcs) into
// a matrix for a left-handed coordinate system (lcs).
//
// Howto:
//
// To transform a vector v_r in rcs to v_l in lcs, the sign of one
// coordinate has to be flipped. Here we flip the sign of the
// z-coordinate with the help of transformation matrix T_z:
//
//       (1  0  0)
// T_z = (0  1  0) , T_z^(-1) = T_z
//       (0  0 -1)
//
// v_l = T_z * v_r
// v_r = T_z * v_l
//
// Construct vector out of affine transformation in rcs:
// For v_r' = A_r * v_r + b_r:
//
// Transform v_r' in rcs to v_l' in lcs:
// v_l' = T_z * v_r'
//      = T_z * (A_r * v_r + b_r)
//      = T_z * (A_r * (T_z * v_l) + b_r)
//      = (T_z * A_r * T_z) * v_l + T_z * b_r
//      = A_l * v_l + b_l
//
// Because of that, a matrix A_r in rcs transforms to
// A_l = T_z * A_r * T_z in lcs.
//
// An affine transformation M changes from rcs to lcs in this way:
//
// M_r = (A_r | b_r)
//       (0   | 1  )
//
// M_r = (m00 m01 m02 m03) => M_l =( m00  m01 -m02  m03)
//       (m10 m11 m12 m13)         ( m10  m11 -m12  m13)
//       (m20 m21 m22 m23)         (-m20 -m21  m22 -m23)
//       (0   0   0   1  )         ( 0    0    0    1  )
//
// Povray does not use M_l but M_l^t for transformations, also
// removing the redunant last row/column (0 0 0 1):
//
// M_l_povray = (+m00 +m10 -m20)
//              (+m01 +m11 -m21)
//              (-m02 -m12 +m22)
//              (+m03 +m13 -m23)
//
// For the column ordered matrix _m[16]
// M_l_povray is given by:
//
// M_l_povray = (+_m0  +_m1  -_m2 )
//              (+_m4  +_m5  -_m6 )
//              (-_m8  -_m9  +_m10)
//              (+_m12 +_m13 -_m14)

    std::stringstream ss;
    ss << "<"
       <<  _m[0] <<  "," <<  _m[1] <<  "," << -_m[2] << ", "
       <<  _m[4] <<  "," <<  _m[5] <<  "," << -_m[6] << ", "
       << -_m[8] <<  "," << -_m[9] <<  "," <<  _m[10] << ", "
       <<  _m[12] << "," <<  _m[13] << "," << -_m[14] << ">";

    return ss.str();
}

}

#endif
