//=============================================================================

#ifndef OPERATIONS_H
#define OPERATIONS_H

//=============================================================================

#include <graphene/scene_graph/Base_node.h>
#include <graphene/scene_graph/Object_node.h>
#include <graphene/gl/GL_state.h>

//=============================================================================

namespace graphene {
namespace scene_graph {

//=============================================================================


class Operation
{
public:
    virtual ~Operation() {};
    virtual void apply(Base_node* _node, gl::GL_state* _gl) = 0;
};


//-----------------------------------------------------------------------------


class Draw_operation : public Operation
{
public:
    ~Draw_operation() {};

    void apply(Base_node* _node, gl::GL_state* _gl)
    {
        _node->draw(_gl);
    };
};


//-----------------------------------------------------------------------------


class Compute_bounding_box_operation : public Operation
{
public:
    ~Compute_bounding_box_operation() {};

    void apply(Base_node* _node, gl::GL_state* _gl)
    {
        bbox_ += _node->bbox();
    };

    geometry::Bounding_box bbox_;
};


//-----------------------------------------------------------------------------


class Find_target_operation : public Operation
{
public:
    Find_target_operation() { target_ = NULL; };
    ~Find_target_operation() {};

    void apply(Base_node* node, gl::GL_state* _gl)
    {
        if (node->is_target())
        {
            Object_node* on = dynamic_cast<Object_node*>(node);

            if (on)
            {
                target_ = on;
            }
        }
    };

    Object_node* target_;
};


//-----------------------------------------------------------------------------


class Collect_objects_operation : public Operation
{
public:

    Collect_objects_operation() {}

    void apply(Base_node* node, gl::GL_state* _gl)
    {
        Object_node* on = dynamic_cast<Object_node*>(node);

        if (on)
        {
            object_nodes_.push_back(on);
        }
    };

    std::vector<Object_node*> object_nodes_;
};


//=============================================================================
} // namespace scene_graph
} // namespace graphene
//=============================================================================
#endif // OPERATIONS_H
//=============================================================================
