//=============================================================================

#ifndef REMESHER_H
#define REMESHER_H


//== INCLUDES =================================================================

#include <graphene/surface_mesh/data_structure/Surface_mesh.h>
#include <graphene/surface_mesh/algorithms/surface_mesh_tools/Triangle_kD_tree.h>


//== NAMESPACES ===============================================================

namespace graphene {
namespace surface_mesh {


//== CLASS DEFINITION =========================================================


void uniform_remeshing(Surface_mesh& mesh,
                       Scalar edge_length,
                       unsigned int iterations=10,
                       bool use_projection=true);

void adaptive_remeshing(Surface_mesh& mesh,
                        Scalar min_edge_length,
                        Scalar max_edge_length,
                        Scalar approx_error,
                        unsigned int iterations=10,
                        bool use_projection=true);


//-----------------------------------------------------------------------------


class Remesher
{
public:

    // constructor
    Remesher(Surface_mesh& mesh);

    // destructor
    ~Remesher();

    // uniform remeshing with target edge length
    void remesh(Scalar edge_length,
                unsigned int iterations=10,
                bool use_projection=true);

    // adaptive remeshing with min/max edge length and approximation error
    void remesh(Scalar min_edge_length,
                Scalar max_edge_length,
                Scalar approx_error,
                unsigned int iterations=10,
                bool use_projection=true);



private:

    void preprocessing();
    void postprocessing();

    void split_long_edges();
    void collapse_short_edges();
    void flip_edges();
    void tangential_smoothing(unsigned int iterations);
    void remove_caps();

    void project_to_reference(Surface_mesh::Vertex v);
    Triangle_kD_tree::Nearest_neighbor  closest_face(Surface_mesh::Vertex v);

    bool is_too_long  (Surface_mesh::Vertex v0, Surface_mesh::Vertex v1) const
    {
        return distance(points_[v0], points_[v1]) > 4.0/3.0 * std::min(vsizing_[v0], vsizing_[v1]);
    }
    bool is_too_short (Surface_mesh::Vertex v0, Surface_mesh::Vertex v1) const
    {
        return distance(points_[v0], points_[v1]) < 4.0/5.0 * std::min(vsizing_[v0], vsizing_[v1]);
    }


private:

    Surface_mesh&  mesh_;
    Surface_mesh*  refmesh_;

    bool use_projection_;
    Triangle_kD_tree*  kD_tree_;

    bool uniform_;
    Scalar target_edge_length_;
    Scalar min_edge_length_;
    Scalar max_edge_length_;
    Scalar approx_error_;

    Surface_mesh::Vertex_property<Point>  points_;
    Surface_mesh::Vertex_property<Point>  vnormal_;
    Surface_mesh::Vertex_property<bool>   vfeature_;
    Surface_mesh::Edge_property<bool>     efeature_;
    Surface_mesh::Vertex_property<bool>   vlocked_;
    Surface_mesh::Edge_property<bool>     elocked_;
    Surface_mesh::Vertex_property<Scalar> vsizing_;

    Surface_mesh::Vertex_property<Point>  refpoints_;
    Surface_mesh::Vertex_property<Point>  refnormals_;
    Surface_mesh::Vertex_property<Scalar> refsizing_;
};


//=============================================================================
} // namespace surface_mesh
} // namespace graphene
//=============================================================================
#endif
//=============================================================================

