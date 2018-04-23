//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================
#ifndef GRAPHENE_SCENE_GRAPH_H
#define GRAPHENE_SCENE_GRAPH_H
//=============================================================================


#include <graphene/geometry/Bounding_box.h>
#include <graphene/gl/GL_state.h>


//=============================================================================


namespace graphene {
namespace scene_graph {


//=============================================================================


class Operation;
class Object_node;
class Base_node;


//=============================================================================


/// \addtogroup scene_graph scene_graph
/// @{

/// A simple scene graph for data managment and rendering
class Scene_graph
{

public:

    typedef graphene::geometry::Bounding_box Bounding_box;


public:

    Scene_graph();
    ~Scene_graph();

    void         draw(gl::GL_state* gl);
    Bounding_box bbox() const;

    Object_node* selected_node() const;
    std::vector<Object_node*> objects() const;


private:

    void traverse(Base_node* _node, Operation& _op, gl::GL_state* _gl=0) const;


public:

    Base_node* root_;
};


//=============================================================================
/// @}
//=============================================================================
} // namespace scene_graph
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_SCENE_GRAPH_H
//=============================================================================
