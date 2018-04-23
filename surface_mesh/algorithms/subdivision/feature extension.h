//=============================================================================

#ifndef FEATURE_EXTENSION_H
#define FEATURE_EXTENSION_H


//== INCLUDES =================================================================

#include <graphene/surface_mesh/data_structure/Surface_mesh.h>


//== NAMESPACE ================================================================

namespace graphene {
namespace surface_mesh {


//=============================================================================


/// \addtogroup 
/// @{
	struct Candidate
	{
		Surface_mesh::Vertex v;
		Scalar Fn;
		Scalar k_max;
		Direction dir;
		Scalar cosine;
	};

	struct  Endpoint
	{
		Surface_mesh::Vertex v;
		Scalar Fn;
		Scalar pro;
		Scalar cosine;
	};
/// Perform one step line dilate on Surface_mesh \a mesh.
void feature_extension(Surface_mesh& mesh);

Scalar max_maximum_curvatures(const std::vector<Candidate>& candidate_cache);
Scalar min_maximum_curvatures(const std::vector<Candidate>& candidate_cache);
Surface_mesh::FeatureVertex  find_shortest_distance_point(Surface_mesh& mesh, Surface_mesh::FeatureLine l, Point v);

/// @}


//=============================================================================
} // namespace surface_mesh
} // namespace graphene
//=============================================================================
#endif
//=============================================================================
