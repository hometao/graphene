//=============================================================================

#ifndef GRAPHENE_SURFACE_MESH_IO_PLUGIN_H
#define GRAPHENE_SURFACE_MESH_IO_PLUGIN_H

//=============================================================================

#include <fstream>
#include <iostream>
#include <vector>

#include <graphene/qt/Main_window.h>
#include <graphene/qt/plugins/interfaces/Plugin_interface.h>
#include <graphene/qt/plugins/interfaces/IO_plugin_interface.h>
#include <graphene/surface_mesh/scene_graph/Surface_mesh_node.h>

//=============================================================================

namespace graphene {
namespace qt {

//=============================================================================
	using graphene::scene_graph::Base_node;
	using graphene::scene_graph::Surface_mesh_node;

    
class Surface_mesh_io_plugin : public QObject,
                               public IO_plugin_interface,
                               public Plugin_interface
{
    Q_OBJECT
    Q_INTERFACES(graphene::qt::IO_plugin_interface)
    Q_INTERFACES(graphene::qt::Plugin_interface)
    Q_PLUGIN_METADATA(IID "graphene.Surface_mesh_io_plugin")

public:
	Surface_mesh_node *node;
    Surface_mesh_io_plugin();

    QStringList filters() const;
    QStringList data_types() const;

    bool can_load(QFileInfo fileinfo) const;
    scene_graph::Base_node* load(QFileInfo,int);

    bool can_save(const scene_graph::Base_node*, QFileInfo fileinfo);
    bool save(const scene_graph::Base_node*, QFileInfo fileinfo);
};
    
    
//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_SURFACE_MESH_IO_PLUGIN_H
//=============================================================================
