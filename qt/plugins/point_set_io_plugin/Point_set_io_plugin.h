//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================
#ifndef GRAPHENE_POINT_SET_IO_PLUGIN_H
#define GRAPHENE_POINT_SET_IO_PLUGIN_H
//=============================================================================


#include <graphene/qt/Main_window.h>
#include <graphene/qt/plugins/interfaces/Plugin_interface.h>
#include <graphene/qt/plugins/interfaces/IO_plugin_interface.h>

#include <fstream>
#include <iostream>
#include <vector>


//=============================================================================


namespace graphene {
namespace qt {


//=============================================================================


/// \addtogroup qt
/// @{


/// The plugin for handling point set I/O
class Point_set_io_plugin : public QObject,
                   public Plugin_interface,
                   public IO_plugin_interface
{
    Q_OBJECT
    Q_INTERFACES(graphene::qt::IO_plugin_interface)
    Q_INTERFACES(graphene::qt::Plugin_interface)
    Q_PLUGIN_METADATA(IID "graphene.Point_set_io_plugin")


public:

    Point_set_io_plugin();

    QStringList filters() const;
    QStringList data_types() const;

    bool can_load(QFileInfo fileinfo) const;
    graphene::scene_graph::Base_node* load(QFileInfo fileinfo);

    bool can_save(const graphene::scene_graph::Base_node*, QFileInfo fileinfo);
    bool save(const graphene::scene_graph::Base_node*, QFileInfo fileinfo);


};


//=============================================================================
///@}
//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_POINT_SET_IO_PLUGIN_H
//=============================================================================
