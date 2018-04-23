//== INCLUDES =================================================================

#include "IO.h"
#include <stdio.h>


//== NAMESPACES ===============================================================


namespace graphene {
namespace surface_mesh {


//== IMPLEMENTATION ===========================================================


// helper function
template <typename T> size_t read(FILE* in, T& t)
{
    return fread((char*)&t, 1, sizeof(t), in);    
}


//-----------------------------------------------------------------------------


bool read_poly(Surface_mesh& mesh, const std::string& filename)
{
    unsigned int n_items;

    // open file (in binary mode)
    FILE* in = fopen(filename.c_str(), "rb");
    if (!in) return false;


    // clear mesh
    mesh.clear();


    // how many elements?
    unsigned int nv, ne, nh, nf, nl;
    read(in, nv);
    read(in, ne);
    read(in, nf);
    nh = 2*ne;


    // resize containers
    mesh.vprops_.resize(nv);
    mesh.hprops_.resize(nh);
    mesh.eprops_.resize(ne);
    mesh.fprops_.resize(nf);
	mesh.lprops_.resize(nl);


    // get properties
    Surface_mesh::Vertex_property<Surface_mesh::Vertex_connectivity>      vconn = mesh.vertex_property<Surface_mesh::Vertex_connectivity>("v:connectivity");
    Surface_mesh::Halfedge_property<Surface_mesh::Halfedge_connectivity>  hconn = mesh.halfedge_property<Surface_mesh::Halfedge_connectivity>("h:connectivity");
    Surface_mesh::Face_property<Surface_mesh::Face_connectivity>          fconn = mesh.face_property<Surface_mesh::Face_connectivity>("f:connectivity");
    Surface_mesh::Vertex_property<Point>                                  point = mesh.vertex_property<Point>("v:point");

    // read properties from file
    n_items = fread((char*)vconn.data(), sizeof(Surface_mesh::Vertex_connectivity),   nv, in);
    n_items = fread((char*)hconn.data(), sizeof(Surface_mesh::Halfedge_connectivity), nh, in);
    n_items = fread((char*)fconn.data(), sizeof(Surface_mesh::Face_connectivity),     nf, in);
    n_items = fread((char*)point.data(), sizeof(Point),                               nv, in);

    fclose(in);
    return true;
}


//=============================================================================
} // namespace surface_mesh
} // namespace graphene
//=============================================================================
