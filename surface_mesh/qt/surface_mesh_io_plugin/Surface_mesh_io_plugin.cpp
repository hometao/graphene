//=============================================================================

#include "Surface_mesh_io_plugin.h"

#include <graphene/macros.h>

//=============================================================================

namespace graphene {
namespace qt {

//=============================================================================


using graphene::scene_graph::Base_node;
using graphene::scene_graph::Surface_mesh_node;


//=============================================================================


Surface_mesh_io_plugin::
Surface_mesh_io_plugin()
{
  name_ = "Surface_mesh_io_plugin";
}


//-----------------------------------------------------------------------------


QStringList
Surface_mesh_io_plugin::
filters() const
{
    return QStringList() << "OFF files (*.off)"
                         << "OBJ files (*.obj)"
                         << "STL files (*.stl)"
                         << "NAS files (*.nas)"
		                 << "FLD files (*.fld)"
		                 << "CLR files (*.clr)";
}


//-----------------------------------------------------------------------------


QStringList
Surface_mesh_io_plugin::
data_types() const
{
    return QStringList() << "Surface_mesh";
}


//-----------------------------------------------------------------------------


bool
Surface_mesh_io_plugin::
can_load(QFileInfo fileinfo) const
{
    QString suffix = fileinfo.suffix();

    if (suffix == "off" ||
        suffix == "obj" ||
        suffix == "stl" ||
        suffix == "nas" ||
		suffix == "fld" ||
		suffix == "clr")
    {
        return true;
    }

    return false;
}


//-----------------------------------------------------------------------------


Base_node*
Surface_mesh_io_plugin::
load(QFileInfo fileinfo, int model)
{
    Surface_mesh_node* pnode = new Surface_mesh_node(main_window_->scene_graph_->root_,
                                                     "Surface_mesh");
	if (model == 0)
	{
		pnode->load(fileinfo.filePath().toStdString().data());
		pnode->set_target(true);
		node = pnode;
	}
	else
	{
		node->load(fileinfo.filePath().toStdString().data());
		node->set_target(true);
	}
	
    LOG(Log_info) << "Loaded " << fileinfo.filePath().toStdString().data() << ".\n";
    return (Base_node*)pnode;
}


//-----------------------------------------------------------------------------


bool
Surface_mesh_io_plugin::
can_save(const Base_node* node, QFileInfo fileinfo)
{
    QString suffix = fileinfo.completeSuffix();

    if (suffix == "off")
    {
        const Surface_mesh_node* pnode = dynamic_cast<const Surface_mesh_node*>(node);

        if (pnode)
        {
            return true;
        }
    }
    if (suffix == "obj")
    {
        const Surface_mesh_node* pnode = dynamic_cast<const Surface_mesh_node*>(node);
        if (pnode)
        {
            return true;
        }
    }

    return false;
}


//-----------------------------------------------------------------------------


bool
Surface_mesh_io_plugin::
save(const Base_node* node, QFileInfo fileinfo)
{
    const Surface_mesh_node* pnode = dynamic_cast<const Surface_mesh_node*>(node);

    if (pnode)
    {
        return pnode->save(fileinfo.filePath().toStdString().data());
    }

    return false;
}


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
