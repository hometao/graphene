//=============================================================================

#ifndef GRAPHENE_TYPES_H
#define GRAPHENE_TYPES_H


//== INCLUDES =================================================================

#include <graphene/geometry/Vector.h>


//=============================================================================
namespace graphene {
//=============================================================================

    
/// Scalar type
typedef float              Scalar;
//typedef double              Scalar;

/// Point type
typedef Vector<Scalar, 3>   Point;
/// Normal type
typedef Vector<Scalar,3>   Normal;

/// Curvature direction
typedef Vector<Scalar, 3>   Direction;

/// Color type
typedef Vector<Scalar,3>   Color;

/// Texture coordinate type
typedef Vector<Scalar,3>   Texture_coordinate;

    
//=============================================================================
}
//=============================================================================
#endif
//============================================================================
