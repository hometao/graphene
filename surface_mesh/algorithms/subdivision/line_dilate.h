//=============================================================================

#ifndef LINE_DILATE_H
#define LINE_DILATE_H


//== INCLUDES =================================================================

#include <graphene/surface_mesh/data_structure/Surface_mesh.h>


//== NAMESPACE ================================================================

namespace graphene {
namespace surface_mesh {


//=============================================================================


/// \addtogroup 
/// @{

/// Perform one step line dilate on Surface_mesh \a mesh.
    void line_dilate(Surface_mesh& mesh);

/// @}


//=============================================================================
} // namespace surface_mesh
} // namespace graphene
//=============================================================================
#endif
//=============================================================================
