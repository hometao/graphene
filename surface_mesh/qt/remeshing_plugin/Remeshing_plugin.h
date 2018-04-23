#ifndef GRAPHENE_REMESHING_PLUGIN_H
#define GRAPHENE_REMESHING_PLUGIN_H

#include <iostream>

#include <graphene/qt/Main_window.h>
#include <graphene/surface_mesh/qt/Surface_mesh_plugin_interface.h>

#include "ui_Remeshing_plugin.h"

namespace graphene {
namespace qt {

class Remeshing_plugin_widget : public Ui_Remeshing_plugin, public QWidget
{
};

class Remeshing_plugin :
    public QObject,
    public Surface_mesh_plugin_interface
{
    Q_OBJECT
    Q_INTERFACES(graphene::qt::Surface_mesh_plugin_interface)
    Q_PLUGIN_METADATA(IID "graphene.Remeshing_plugin")

public:
    Remeshing_plugin();
    void init(Main_window *w);

public slots:
    void slot_uniform_remeshing();
    void slot_adaptive_remeshing();
    void slot_mean_edge_length();
	void slot_select_ridge();
	void slot_select_ravine();
	void slot_calculate_line();

private:
    void analyze_mesh();

protected:
    Remeshing_plugin_widget* widget_;
};

}
}

#endif // GRAPHENE_REMESHING_PLUGIN_H
