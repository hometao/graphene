//=============================================================================

#include <graphene/scene_graph/Scene_graph.h>
#include <graphene/scene_graph/Operations.h>
#include <graphene/scene_graph/Object_node.h>

#include <algorithm>

//=============================================================================

namespace graphene {
namespace scene_graph {

//=============================================================================


Scene_graph::
Scene_graph()
{
    root_ = new Base_node(NULL, "Root Node");
}


//-----------------------------------------------------------------------------


Scene_graph::
~Scene_graph()
{
    delete root_;
}


//-----------------------------------------------------------------------------


void
Scene_graph::
traverse(Base_node* _node, Operation& _op, gl::GL_state* _gl) const
{
    _node->enter(_gl);

    _op.apply(_node, _gl);

    for (Base_node* child : _node->children())
    {
        traverse(child, _op, _gl);
    }

    _node->leave(_gl);
}


//-----------------------------------------------------------------------------


void
Scene_graph::
draw(gl::GL_state* gl)
{
    Draw_operation op;
    traverse(root_, op, gl);
}


//-----------------------------------------------------------------------------


geometry::Bounding_box
Scene_graph::
bbox() const
{
    Compute_bounding_box_operation op;
    traverse(root_, op);
    return op.bbox_;
}


//-----------------------------------------------------------------------------


Object_node*
Scene_graph::
selected_node() const
{
    Find_target_operation op;
    traverse(root_, op);
    return op.target_;
}


//-----------------------------------------------------------------------------


std::vector<Object_node*>
Scene_graph::
objects() const
{    
    Collect_objects_operation op;
    traverse(root_, op);
    return op.object_nodes_;
}


//=============================================================================
} // namespace scene_graph
} // namespace graphene
//=============================================================================
