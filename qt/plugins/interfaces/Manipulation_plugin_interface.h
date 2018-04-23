//=============================================================================
// Copyright (C) 2013 by Graphics & Geometry Group, Bielefeld University
//=============================================================================


#ifndef GRAPHENE_MANIPULATION_PLUGIN_INTERFACE_H
#define GRAPHENE_MANIPULATION_PLUGIN_INTERFACE_H


//== INCLUDES =================================================================


#include <graphene/scene_graph/Base_node.h>

#include <list>
#include <string>


//== NAMESPACES ===============================================================


namespace graphene {
namespace qt {


//== CLASS DEFINITION =========================================================


/// Interface for plugins providing means to manipulate objects.
///
/// Plugins that provide manipulation modes have to implement this interface.
class Manipulation_plugin_interface
{
public:
    virtual ~Manipulation_plugin_interface() {};

    virtual void update_manipulation_modes(scene_graph::Base_node* node) = 0;

protected:
    std::list<std::string> manipulation_modes_;
};


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
Q_DECLARE_INTERFACE(graphene::qt::Manipulation_plugin_interface,
                    "de.uni-bielefeld.graphics.graphene.Manipulation_plugin_interface/1.0")
//=============================================================================
#endif // GRAPHENE_MANIPULATION_PLUGIN_INTERFACE_H
//=============================================================================

