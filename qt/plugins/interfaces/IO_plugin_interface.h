//=============================================================================
// Copyright (C) 2013 by Graphics & Geometry Group, Bielefeld University
//=============================================================================


#ifndef GRAPHENE_IO_PLUGIN_INTERFACE_H
#define GRAPHENE_IO_PLUGIN_INTERFACE_H


//== INCLUDES =================================================================

#include <graphene/scene_graph/Base_node.h>

#include <QFileInfo>
#include <QString>
#include <QStringList>



//== NAMESPACES ===============================================================


namespace graphene {
namespace qt {


//== CLASS DEFINITION =========================================================


/// Interface for plugins involving file IO.
///
/// Plugins that offer support for reading and writing particular file
/// types have to implement this interface.
class IO_plugin_interface
{
public:
    // Destructor.
    virtual ~IO_plugin_interface() {};

    /// Return the list of file name filters supported by this plugin.
    virtual QStringList filters() const = 0;

    /// Return the list of supported mesh types.
    virtual QStringList data_types() const = 0;

    /// Indicate if the plugin is able to load a certain file.
    virtual bool can_load(QFileInfo fileinfo) const = 0;

    /// Load a file.
    virtual scene_graph::Base_node* load(QFileInfo fileinfo,int model=0) = 0;

    /// Indicate if the plugin is able to save a certain file.
    virtual bool can_save(const scene_graph::Base_node*, QFileInfo fileinfo) = 0;

    /// Save a file.
    virtual bool save(const scene_graph::Base_node*, QFileInfo fileinfo) = 0;
};


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
Q_DECLARE_INTERFACE(graphene::qt::IO_plugin_interface,
                    "de.uni-bielefeld.graphics.graphene.IO_plugin_interface/1.0")
//=============================================================================
#endif // GRAPHENE_IO_PLUGIN_INTERFACE_H
