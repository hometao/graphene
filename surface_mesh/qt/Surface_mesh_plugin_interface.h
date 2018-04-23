//=============================================================================
// Copyright (C) 2013 by Graphics & Geometry Group, Bielefeld University
//=============================================================================


#ifndef GRAPHENE_SURFACE_MESH_PLUGIN_INTERFACE_H
#define GRAPHENE_SURFACE_MESH_PLUGIN_INTERFACE_H


//=============================================================================


#include <graphene/qt/Main_window.h>
#include <graphene/qt/plugins/interfaces/Plugin_interface.h>
#include <graphene/surface_mesh/scene_graph/Surface_mesh_node.h>


//=============================================================================


namespace graphene {
namespace qt {


//=============================================================================


class Surface_mesh_plugin_interface : public Plugin_interface
{
public:

    typedef graphene::scene_graph::Surface_mesh_node Surface_mesh_node;
    typedef graphene::surface_mesh::Surface_mesh Surface_mesh;


public:

    /// Destructor
    virtual ~Surface_mesh_plugin_interface() {};

    /// Return the currently selected surface_mesh.
    virtual Surface_mesh* selected_mesh()
    {
        Surface_mesh_node* node = dynamic_cast<Surface_mesh_node*>(main_window_->scene_graph_->selected_node());

        if (node)
        {
            return &node->mesh_;
        }
        else
        {
            std::cerr << "Failed to retrieve selected surface_mesh!" << std::endl;
        }

        return NULL;
    };

    /// Return the currently selected Surface_mesh_node.
    virtual Surface_mesh_node* selected_node()
    {
        Surface_mesh_node* node =  dynamic_cast<Surface_mesh_node*>(main_window_->scene_graph_->selected_node());

        if (node)
        {
            return node;
        }
        else
        {
            std::cerr << "Failed to retrieve selected Surface_mesh_node!" << std::endl;
        }

        return NULL;
    };
};


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
Q_DECLARE_INTERFACE(graphene::qt::Surface_mesh_plugin_interface,
                    "de.uni-bielefeld.graphics.graphene.Surface_mesh_plugin_interface/1.0")
//=============================================================================
#endif // GRAPHENE_SURFACE_MESH_PLUGIN_INTERFACE_H
//=============================================================================

