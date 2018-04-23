//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================


#include <graphene/qt/plugins/point_set_io_plugin/Point_set_io_plugin.h>
#include <graphene/scene_graph/Point_set_node.h>
#include <graphene/macros.h>


//=============================================================================


namespace graphene {
namespace qt {


using graphene::scene_graph::Base_node;
using graphene::scene_graph::Point_set_node;


//=============================================================================


Point_set_io_plugin::
Point_set_io_plugin()
{
  name_ = "Point_set_io_plugin";
}


//-----------------------------------------------------------------------------


QStringList
Point_set_io_plugin::
filters() const
{
    return QStringList() << "XYZ files (*.xyz)";
}


//-----------------------------------------------------------------------------


QStringList
Point_set_io_plugin::
data_types() const
{
    return QStringList() << "Point Set with Normals";
}


//-----------------------------------------------------------------------------


bool
Point_set_io_plugin::
can_load(QFileInfo fileinfo) const
{
    if (fileinfo.completeSuffix() == "xyz")
    {
        return true;
    }

    return false;
}


//-----------------------------------------------------------------------------


Base_node*
Point_set_io_plugin::
load(QFileInfo fileinfo)
{
    Point_set_node* ps_node = new Point_set_node(main_window_->scene_graph_->root_);
    ps_node->load(fileinfo.filePath().toStdString().data());
    ps_node->set_target(true);

    LOG(Log_info) << "Loaded " << fileinfo.filePath().toStdString().data() << ".\n";

    return (Base_node*)ps_node;
}


//-----------------------------------------------------------------------------


bool
Point_set_io_plugin::
can_save(const Base_node* node, QFileInfo fileinfo)
{
    return (dynamic_cast<const Point_set_node*>(node)
            && (fileinfo.completeSuffix() == "xyz"));
}


//-----------------------------------------------------------------------------


bool
Point_set_io_plugin::
save(const Base_node* node, QFileInfo fileinfo)
{
    const Point_set_node* ps_node = dynamic_cast<const Point_set_node*>(node);

    if(!ps_node)
        return false;

    return ps_node->save(fileinfo.filePath().toStdString().data());
}


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
