#ifndef GRAPHENE_ADAPT_SUBDIVISION_H
#define GRAPHENE_ADAPT_SUBDIVISION_H

#include <graphene/surface_mesh/data_structure/Surface_mesh.h>

namespace graphene {
namespace surface_mesh {

/// adapt mid subdivision 
/// @{

/// Perform one step of adapt midpoint subdivision on Surface_mesh \a mesh.
void adapt_subdivision(Surface_mesh& mesh);

/// @}

//=============================================================================
} // namespace surface_mesh
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_ADAPT_SUBDIVISION_H
//=============================================================================