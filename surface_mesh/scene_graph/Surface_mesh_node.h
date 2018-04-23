//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================
#ifndef GRAPHENE_SURFACE_MESH_NODE_H
#define GRAPHENE_SURFACE_MESH_NODE_H
//=============================================================================


#include <graphene/scene_graph/Object_node.h>
#include <graphene/surface_mesh/data_structure/Surface_mesh.h>
#include <graphene/gl/Shader.h>


//=============================================================================


namespace graphene {
namespace scene_graph {


//=============================================================================


/// \addtogroup scene_graph
/// @{


/// A scene graph node for redering surface meshes
class Surface_mesh_node : public Object_node
{

public:

    typedef ::graphene::surface_mesh::Surface_mesh Surface_mesh;

    Surface_mesh_node(Base_node*         _parent = NULL,
                      const std::string& _name   = "Surface_mesh_node");
    virtual ~Surface_mesh_node();

    bool load(const std::string& filename);
    bool save(const std::string& filename) const;

    void draw(gl::GL_state* _gl);

    Bounding_box bbox() const;
    virtual std::string info() const;

    void clear_selections();
    void invert_selections();
    void select_all();
    void select_isolated();
    void delete_selected();
    void grow_selection();
    void set_selection(const std::vector<size_t>& indices);
    void get_selection(std::vector<size_t>& indices);
    void clear_selection(const std::vector<size_t>& indices);
    void select_point(Point p);

    void update_mesh(bool has_features=false);
    void update_colors();
    void update_texcoords();
    void update_features();
    void update_selection();
	void update_segmentation();
    void update_bbox();
    void crease_normals(std::vector<Normal>& vertex_normals);


    bool has_colors()
    {
        return mesh_.get_vertex_property<Color>("v:color") ||
            mesh_.get_face_property<Color>("f:color");
    }

public:

    Surface_mesh mesh_;

    // material
    Vec3f  front_color_;
    Vec3f  back_color_;
    Vec3f  wire_color_;
	Vec3f  ridge_color_;
	Vec3f  ravine_color_;
    Vec4f  material_;
    double crease_angle_;
	bool is_visual;


private:

    void initialize_buffers();
    void delete_buffers();


private:

    // OpenGL buffers
    GLuint vertex_array_object_;
    GLuint vertex_buffer_;
    GLuint normal_buffer_;
    GLuint color_buffer_;
    GLuint tex_coord_buffer_;
    GLuint edge_index_buffer_;
    GLuint feature_index_buffer_;
    GLuint selection_index_buffer_;
    GLuint texture_;


	GLuint line_index_buffer_;
	GLuint lineVert_buffer_;
	GLuint line_ravidx_buffer_;
	GLuint vsa_index_buffer_;

    // buffer sizes
    GLsizei n_vertices_;
    GLsizei n_edges_;
    GLsizei n_triangles_;
    GLsizei n_selected_;
    GLsizei n_feature_;

	GLsizei n_lines_;
	GLsizei n_lineVerts_;
	GLsizei n_rav_lines_;
	GLsizei n_ravVerts_;
	GLsizei n_vsa_edges_;

	// data that we will eventually duplicate
	std::vector<Point>        points;
	std::vector<Normal>       normals;
};


//=============================================================================
/// @}
//=============================================================================
} // namespace scnene_graph
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_SURFACE_MESH_NODE_H
//=============================================================================
