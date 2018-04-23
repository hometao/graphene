//== INCLUDES =================================================================

#include <graphene/surface_mesh/algorithms/subdivision/loop_subdivision.h>


//== NAMESPACE ================================================================

namespace graphene {
namespace surface_mesh {


//== IMPLEMENTATION ==========================================================


void loop_subdivision(Surface_mesh& mesh)
{    
    if (!mesh.is_triangle_mesh())
    {
        return;
    }

    // reserve memory
    int nv = mesh.n_vertices();
    int ne = mesh.n_edges();
    int nf = mesh.n_faces();
    mesh.reserve(nv+ne, 2*ne+3*nf, 4*nf);


    // get properties
    auto points = mesh.vertex_property<Point>("v:point");
    auto vpoint = mesh.add_vertex_property<Point>("loop:vpoint");
    auto epoint = mesh.add_edge_property<Point>("loop:epoint");
    auto vfeature = mesh.get_vertex_property<bool>("v:feature");
    auto efeature = mesh.get_edge_property<bool>("e:feature");


    // compute vertex positions
    for (auto v: mesh.vertices())
    {
        // isolated vertex?
        if (mesh.is_isolated(v))
        {
            vpoint[v] = points[v];
        }

        // boundary vertex?
        else if (mesh.is_boundary(v))
        {
            Surface_mesh::Halfedge h1 = mesh.halfedge(v);
            Surface_mesh::Halfedge h0 = mesh.prev_halfedge(h1);

            Point p = points[v];
            p *= 6.0;
            p += points[mesh.to_vertex(h1)];
            p += points[mesh.from_vertex(h0)];
            p *= 0.125;

            vpoint[v] = p;
        }

        // interior feature vertex?
        else if (vfeature && vfeature[v])
        {
            Point p = points[v];
            p *= 6.0;
            int count(0);

            for (auto h: mesh.halfedges(v))
            {
                if (efeature[mesh.edge(h)])
                {
                    p += points[mesh.to_vertex(h)];
                    ++count;
                }
            }

            if (count == 2) // vertex is on feature edge
            {
                p *= 0.125;
                vpoint[v] = p;
            }
            else // keep fixed
            {
                vpoint[v] = points[v];
            }
        }

        // interior vertex
        else
        {
            Point   p(0,0,0);
            Scalar  k(0);

            for (auto vv: mesh.vertices(v))
            {
                p += points[vv];
                ++k;
            }
            p /= k;

            Scalar  beta = (0.625 - pow(0.375 + 0.25*cos(2.0*M_PI/k), 2.0));

            vpoint[v] = points[v]*(Scalar)(1.0-beta) + beta*p;
        }
    }


    // compute edge positions
    for (auto e: mesh.edges())
    {
        // boundary or feature edge?
        if (mesh.is_boundary(e) || (efeature && efeature[e]))
        {
            epoint[e] = (points[mesh.vertex(e,0)] + points[mesh.vertex(e,1)]) * Scalar(0.5);
        }

        // interior edge
        else
        {
            Surface_mesh::Halfedge h0 = mesh.halfedge(e, 0);
            Surface_mesh::Halfedge h1 = mesh.halfedge(e, 1);
            Point p = points[mesh.to_vertex(h0)];
            p += points[mesh.to_vertex(h1)];
            p *= 3.0;
            p += points[mesh.to_vertex(mesh.next_halfedge(h0))];
            p += points[mesh.to_vertex(mesh.next_halfedge(h1))];
            p *= 0.125;
            epoint[e] = p;
        }
    }


    // set new vertex positions
    for (auto v: mesh.vertices())
    {
        points[v] = vpoint[v];
    }


    // insert new vertices on edges
    for (auto e: mesh.edges())
    {
        // feature edge?
        if (efeature && efeature[e])
        {
            Surface_mesh::Halfedge h  = mesh.insert_vertex(e, epoint[e]);
            Surface_mesh::Vertex   v  = mesh.to_vertex(h);
            Surface_mesh::Edge     e0 = mesh.edge(h);
            Surface_mesh::Edge     e1 = mesh.edge(mesh.next_halfedge(h));
            vfeature[v]  = true;
            efeature[e0] = true;
            efeature[e1] = true;
        }

        // normal edge
        else
        {
            mesh.insert_vertex(e, epoint[e]);
        }
    }


    // split faces
    Surface_mesh::Halfedge  h;
    for (auto f: mesh.faces())
    {
        h = mesh.halfedge(f);
        mesh.insert_edge(h, mesh.next_halfedge(mesh.next_halfedge(h)));
        h = mesh.next_halfedge(h);
        mesh.insert_edge(h, mesh.next_halfedge(mesh.next_halfedge(h)));
        h = mesh.next_halfedge(h);
        mesh.insert_edge(h, mesh.next_halfedge(mesh.next_halfedge(h)));
    }


    // clean-up properties
    mesh.remove_vertex_property(vpoint);
    mesh.remove_edge_property(epoint);
}


//=============================================================================
} // namespace surface_mesh
} // namespace graphene
//=============================================================================

