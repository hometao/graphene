#include <graphene/scene_graph/Base_node.h>
#include <graphene/scene_graph/Camera_node.h>
#include <graphene/scene_graph/Light_node.h>
#include <graphene/scene_graph/Material_node.h>
#include <graphene/surface_mesh/scene_graph/Surface_mesh_node.h>
#include <graphene/scene_graph/Transformation_node.h>
#include <graphene/scene_graph/Background_node.h>
#include <graphene/scene_graph/Point_set_node.h>

#include <graphene/qt/plugins/povray_plugin/Helper_functions.h>

#include <map>
#include <sstream>


// ---------------------------------------------------------------------------


// Transformations of scene graph nodes into declaration in for the .pov file
// of Povray.

using namespace graphene::scene_graph;

namespace graphene
{


// ---------------------------------------------------------------------------


std::string
transform(
    Transformation_node* _node,
    std::map<Base_node*, std::string>& _keyword_map)
{
    _keyword_map[_node] = "transform";

    std::stringstream ss;
    ss << comment_line(_node);
    ss << "#declare D" << _node << " = transform {\n";
    ss << "     matrix " << Povray_matrix(_node->mat_) << '\n'
       << "}\n";

    return ss.str();
}


// ---------------------------------------------------------------------------


std::string
transform(
    Background_node* _node,
    std::map<Base_node*,std::string>& _keyword_map)
{
    _keyword_map[_node] = "";

    std::stringstream ss;
    ss << comment_line(_node);
    ss << "background {"
       << "color rgb " << Povray_vector(_node->color_) << " "
       << "}\n";

    return ss.str();
}


// ---------------------------------------------------------------------------


std::string
transform(
    Camera_node* _node,
    std::map<Base_node*,std::string>& _keyword_map)
{
    // prevent using this node in the scene graph hierarchy (of unions)
    _keyword_map[_node] = "";

    // calculate the height and width of the viewing plane
    //
    // near plane height
    float height_near =
        2.0*_node->near_*tan(_node->fovy_ * 0.5 * (M_PI / 180.0));
    // near plane width
    float width_near = _node->aspect_ * height_near;

    // vectors in left handed coordinate system of povray
    Vec3f location(0,0,0);
    Vec3f up(0,height_near,0);
    Vec3f right(width_near,0,0);
    Vec3f direction(0,0,_node->near_);
    Vec3f look_at(0,0,1);

    // create Povray element for a camera
    std::stringstream ss;
    ss << comment_line(_node);
    ss << "camera {"
       << "location "  << Povray_vector(location)  << " "
       << "up "        << Povray_vector(up)        << " "
       << "right "     << Povray_vector(right)     << " "
       << "direction " << Povray_vector(direction) << " "
       << "look_at "   << Povray_vector(look_at)   << " "
       << "}\n";

    return ss.str();
}


// ---------------------------------------------------------------------------


std::string
transform(Light_node* _node,std::map<Base_node*,std::string>& _keyword_map)
{
    _keyword_map[_node] = "light_source";

    std::stringstream ss;
    ss << comment_line(_node);
    ss << "#declare D" << _node << "= light_source { "
       << Povray_vector(rcs2lcs(_node->location_)) << " "
       << "color rgb " << Povray_vector(_node->color_) << " "
       << "}\n";

    return ss.str();

}


// ---------------------------------------------------------------------------


std::string
transform(
    Material_node* _node,
    std::map<Base_node*,std::string>& _keyword_map)
{
    _keyword_map[_node] = "texture";

    float diffuse = (_node->front_diffuse_.data()[0] +
                     _node->front_diffuse_.data()[1] +
                     _node->front_diffuse_.data()[2]) / 3.0;

    // create pigment color of 10% of ambient_material color and
    // 60% diffuse_material color
    Vec4f pigment_color(0.1*_node->front_ambient_+0.6*_node->front_diffuse_);

    std::stringstream ss;

    std::string indent = "    ";

    ss << comment_line(_node);
    ss << "#declare D" << _node << " = texture {\n"
       << indent << "finish { "
       << "ambient rgb "    << Povray_vector(_node->front_ambient_)  << " "
       << "diffuse "        << diffuse            << " "
       << "specular "       << _node->specular_   << " "
       << "roughness "      << _node->roughness_  << " "
       << "phong "          << _node->phong_      << " "
       << "phong_size "     << _node->front_shininess_ << " "
       << "}\n"
       << indent << "pigment { "
       << "rgb " << Povray_vector(pigment_color) << " }" << std::endl
       << "}\n";

    return ss.str();
}


// ---------------------------------------------------------------------------


// concatenates strings like
// prefix + vertex_property_vector + suffix
// for every vertex in the mesh
template<typename S>
std::string
write_mesh_vectors(const Surface_mesh& _mesh, const S& vecs, const std::string& prefix, const std::string& suffix)
{

    std::stringstream ss;


    Surface_mesh::Vertex_iterator it  = _mesh.vertices_begin();
    Surface_mesh::Vertex_iterator end = _mesh.vertices_end();

    while (it != end)
    {
        Point p = vecs[*it];
        ss.precision(7);
        ss << prefix << Povray_vector(rcs2lcs(p)) << suffix;
        ++it;
    }

    return ss.str();

}


// ---------------------------------------------------------------------------


// writes the mesh indices for every face
std::string
write_mesh_indices(const Surface_mesh& _mesh, const std::string& indent)
{


    std::stringstream ss;

    Surface_mesh::Face_iterator fit = _mesh.faces_begin();

    while (fit != _mesh.faces_end())
    {
        Surface_mesh::Vertex_around_face_circulator fvit;
        fvit = _mesh.vertices(*fit);

        int idx0 = (*fvit).idx();
        int idx1 = (*++fvit).idx();
        int idx2 = (*++fvit).idx();

        ss << indent << "<" <<  idx0 << "," << idx1  << "," << idx2 << ">," << '\n';

        ++fit;
    }

    return ss.str();

}


// ---------------------------------------------------------------------------


// creates for every edge in the mesh a string like this
//
// prefix + edge_start_point + "," + edge_end_point + suffix
//
// and concatenates it.
std::string
write_mesh_edges(const Surface_mesh& _mesh, const std::string& prefix, const std::string& suffix)
{


    std::stringstream ss;

    Surface_mesh::Vertex_property<Point> points;
    points = _mesh.get_vertex_property<Point>("v:point");

    Surface_mesh::Edge_iterator e_it, e_end(_mesh.edges_end());

    for (e_it = _mesh.edges_begin(); e_it != e_end; ++e_it)
    {
        Point a = points[_mesh.to_vertex(_mesh.halfedge(*e_it,0))];
        Point b = points[_mesh.to_vertex(_mesh.halfedge(*e_it,1))];
        ss << prefix
           << Povray_vector(rcs2lcs(a)) << ", "
           << Povray_vector(rcs2lcs(b)) << ", "
           << suffix;
    }

    return ss.str();
}


// ---------------------------------------------------------------------------


// creates a Povray wireframe out of a surface mesh
std::string mesh_wireframe(Surface_mesh_node* _node,const std::string& indent)
{
    const Surface_mesh& mesh = _node->mesh_;

    std::stringstream ss;

    Surface_mesh::Vertex_property<Point> points = mesh.get_vertex_property<Point>("v:point");

    ss << "union {" << '\n'
       << indent << "#local R = " << _node->radius_ << ";\n"
       << write_mesh_edges(mesh,indent + "cylinder {","R open }\n")
       << write_mesh_vectors(mesh, points, indent + "sphere { ", " 1.5*R }\n")
       << indent << "texture { pigment { color " << Povray_vector(_node->wireframe_color_) << "}}\n"
       << "}\n";

    return ss.str();

}


// ---------------------------------------------------------------------------


// Computes a smoothed normal at vertex _vh for face _fh.
// Uses all neighbour face normals that differ less than angle from the central
// face normal.
Point
smooth_normal(const Surface_mesh& mesh_,Surface_mesh::Vertex _vh, Surface_mesh::Face _fh, Scalar angle)
{

    typedef Surface_mesh Mesh;

    Mesh::Vertex_around_vertex_circulator v2_it = mesh_.vertices(_vh);

    Point nn(0,0,0);


    // CHECK: can this be false?
    if (v2_it)
    {
        const Mesh::Face_property<Point>& normals = mesh_.get_face_property<Point>("f:normal");
        const Surface_mesh::Vertex_property<Point>& points = mesh_.get_vertex_property<Point>("v:point");

        const Point nf = normals[_fh];
        const Scalar ca = angle;
        const Scalar c  = cos( ca * M_PI / 180.0 );

        Mesh::Vertex_around_vertex_circulator v1_it(v2_it); ++v1_it;
        const Point&     p0 = points[_vh];
        Point d1, d2;
        Point n;
        Scalar cosine, angle;
        Mesh::Face fh;


        // for all vertices arount _vh
        for (; v1_it != mesh_.vertices(_vh); ++v2_it, ++v1_it)
        {
            if ((fh = mesh_.face(mesh_.halfedge(*v1_it))).is_valid())
            {
                n = normals[fh];

                if (dot(n,nf) > c)
                {
                    // compute angle between the two corresponding faces
                    const Point& p1 = points[*v1_it];
                    const Point& p2 = points[*v2_it];

                    ((d1 = p1) -= p0).normalize();
                    ((d2 = p2) -= p0).normalize();

                    // CHECK: Is it really necessary to use the
                    // CHECK: length of d1 and d2 (both are 1)?
                    // cosine = dot(d1,d2) / sqrt(dot(d1,d1) * dot(d2,d2));
                    cosine = dot(d1,d2);
                    //if      (cosine < -1.0f)
                    //cosine = -1.0f;
                    //else if (cosine >  1.0f)
                    //cosine =  1.0f;
                    angle = acos(cosine);

                    // weight normal with angle
                    n *= angle;

                    // use this modified normal to construct part
                    // of the new normal at _vh for face _fh
                    nn += n;
                }
            }
        }

        // if no modification takes place, just use _fh's normal
        // else use the new computed normal
        if (nn == Point(0,0,0))
            return nf;
        else
            return nn.normalize();
    }


    return nn;
}


// ---------------------------------------------------------------------------


// creates smoothed normals in Povray format for all vertices with the help
// of the conditional angle.
// prefix and suffix are appended correspondenly to this normals.
std::string mesh_smooth_normal(const Surface_mesh& mesh,Scalar angle,const std::string& prefix, const std::string& suffix)
{
    Surface_mesh::Face_iterator f_it;
    Surface_mesh::Face_iterator f_end(mesh.faces_end());

    Surface_mesh::Vertex_around_face_circulator fv_it;
    Surface_mesh::Vertex_around_face_circulator fv_end;

    std::stringstream ss;

    for (f_it = mesh.faces_begin(); f_it != f_end; ++f_it)
    {
        {
            fv_it = mesh.vertices(*f_it);
            fv_end = fv_it;
            do
            {
                Point n = smooth_normal(mesh,*fv_it, *f_it, angle);
                ss << prefix << Povray_vector(rcs2lcs(n)) << suffix;
                ++fv_it;
            } while (fv_it != fv_end);
        }
    }

    return ss.str();
}


// ---------------------------------------------------------------------------


// creates indices for smooth normals in Povray format for all faces
std::string mesh_smooth_normal_indices(const Surface_mesh& mesh,const std::string& prefix, const std::string& suffix)
{

    //


    Surface_mesh::Face_iterator f_it;
    Surface_mesh::Face_iterator f_end(mesh.faces_end());

    std::stringstream ss;
    int i = 0;
    for (f_it = mesh.faces_begin(); f_it != f_end; ++f_it)
    {
        //if (mesh.status(f_it).tagged())
        {
            int i0 = i++;
            int i1 = i++;
            int i2 = i++;

            ss << prefix << Povray_vector(Vec3i(i0, i1, i2)) << suffix;
        }
    }

    return ss.str();

}


// ---------------------------------------------------------------------------


// translates a surface mesh of _node into a mesh2-object
std::string
write_mesh2(Surface_mesh_node* _node, const std::string& indent)
{

    // transforms a surface mesh into a mesh2

    const Surface_mesh& mesh = _node->mesh_;

    Surface_mesh::Vertex_property<Point> points = mesh.get_vertex_property<Point>("v:point");
    Surface_mesh::Vertex_property<Normal> normals = _node->vertex_normals_;

    std::string draw_mode = _node->get_draw_mode();

    std::stringstream ss;

    ss << "mesh2 {\n";

    // mesh2 vertices
    ss << indent << "vertex_vectors {" << '\n';
    ss << indent << indent  << mesh.n_vertices() << ",\n";
    ss << write_mesh_vectors(mesh, points, indent + indent, ",\n");
    ss << indent << "}\n";

    // no adaptive shading, if angle == 0.
    bool adaptive_shading = _node->adaptive_shading_angle_ > 0.0;;

    // mesh2 normals
    if (draw_mode == "Solid Smooth")
    {
        ss << indent << "normal_vectors {\n";
        ss << indent << indent << mesh.n_vertices() << ",\n";
        ss << write_mesh_vectors(mesh, normals, indent + indent, ",\n");
        ss << indent << "}\n";
    }
    else if (draw_mode == "Solid Flat" && adaptive_shading)
    {
        ss << indent << "normal_vectors {\n";
        ss << indent << indent << 3 * mesh.faces_size() << ",\n";
        ss << mesh_smooth_normal(mesh,_node->adaptive_shading_angle_, indent + indent,",\n");
        ss << indent << "}\n";
    }

    // mesh2 faces
    ss << indent << "face_indices {\n"
       << indent << indent << mesh.n_faces() << ",\n"
       << write_mesh_indices(mesh,indent + indent)
       << indent << "}\n";

    if (draw_mode == "Solid Flat" && adaptive_shading)
    {
        ss << indent << "normal_indices {\n";
        ss << indent << indent  << mesh.faces_size() << ",\n";
        ss << mesh_smooth_normal_indices(mesh,indent + indent,",\n");
        //ss << write_mesh_indices(mesh,indent + indent);
        ss << indent << "}\n";
    }

    ss << "}\n";

    return ss.str();

}


// ---------------------------------------------------------------------------


std::string transform( Surface_mesh_node* _node, std::map<Base_node*,std::string>& _keyword_map)
{
    const Surface_mesh& mesh = _node->mesh_;

    std::stringstream ss;

    // add comment for this node
    ss << comment_line(_node);

    // get draw mode
    std::string draw_mode = _node->get_draw_mode();

    // do nothing if mesh has no vertices or not supported draw_mode
    if (mesh.n_vertices() == 0 || draw_mode == "Vertex Color")
    {
        _keyword_map[_node] = "";
        ss << "// Surface_mesh removed\n";
        return ss.str();
    }


    // use mesh as an object
    _keyword_map[_node] = "object";


    // set indentation depth
    std::string indent = "    ";


    // declare new makro for this node
    ss << "#declare D" << _node << " =\n";


    // draw this node as wireframe?
    if (draw_mode == "Wireframe")
    {
        ss << mesh_wireframe(_node,indent);
        return ss.str();
    }


    if (draw_mode == "Points")
    {
        Surface_mesh::Vertex_property<Point> points = mesh.get_vertex_property<Point>("v:point");

        ss << "union {\n"
           << indent << "#local R = " << _node->radius_ << ";\n"
           << write_mesh_vectors(mesh, points, indent + "sphere { ", " R }\n")
           << "}\n";

        return ss.str();
    }


    if (draw_mode == "Hidden Line")
    {
        ss << "union {\n";
        ss << write_mesh2(_node,indent);
        ss << mesh_wireframe(_node,indent);
        ss << "}\n";

        return ss.str();
    }


    if (draw_mode == "Solid Smooth" || draw_mode == "Solid Flat")
    {
        ss << write_mesh2(_node,indent);

        return ss.str();
    }

    return "";

}


// ---------------------------------------------------------------------------


std::string
transform(Point_set_node* _node,std::map<Base_node*,std::string>& _keyword_map)
{

    _keyword_map[_node] = "object";

    std::stringstream ss;

    ss << comment_line(_node)
       << "#declare D" << _node << " = union { \n"
       << "     #local R = " << _node->radius_ << ";\n";

    Point_set:: Point_iterator pit = _node->point_set_.points_begin();
    Point_set:: Point_iterator pend = _node->point_set_.points_end();

    while (pit != pend)
    {
        ss << "     sphere { " << Povray_vector(rcs2lcs(*(pit++))) << " R }\n";
    }
    ss << "}\n";

    return ss.str();
}


// ---------------------------------------------------------------------------


// apply right transform to _node and return result.
const std::string convert_node_to_povray_format(
        Base_node* _node,
        std::map<Base_node*,std::string>& _keyword_map)
{
    if (dynamic_cast<Transformation_node*>(_node))
        return transform((Transformation_node*) _node, _keyword_map);

    if (dynamic_cast<Background_node*>(_node))
        return transform((Background_node*) _node, _keyword_map);

    if (dynamic_cast<Camera_node*>(_node))
        return transform((Camera_node*) _node, _keyword_map);

    if (dynamic_cast<Light_node*>(_node))
        return transform((Light_node*) _node, _keyword_map);

    if (dynamic_cast<Material_node*>(_node))
        return transform((Material_node*) _node, _keyword_map);

    if (dynamic_cast<Surface_mesh_node*>(_node))
        return transform((Surface_mesh_node*) _node, _keyword_map);

    if (dynamic_cast<Point_set_node*>(_node))
        return transform((Point_set_node*) _node, _keyword_map);

    // Missing transformation for _node
    return "";
}


// ---------------------------------------------------------------------------


// returns a string, with whom the declaration of _node is
// applicable in the hierarchy of unions
const std::string create_parent_directive(
        Base_node* _node,
        std::string _keyword)
{
    // Is _node not allowed in an union? (camera, background, ...)
    // This nodes don't need a directive-declaration, they are
    // globaly applicable. For this type of nodes _keyword
    // will be ""
    if (_keyword == "")
        return "";

    // creates the directive, applicable in the hierarchy
    std::stringstream ss;
    ss << comment_line(_node);
    ss << _keyword << " { D" << _node << " }" << std::endl;

    return ss.str();
}


// ---------------------------------------------------------------------------


}
