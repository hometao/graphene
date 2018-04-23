//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================
#ifndef GRAPHENE_SUBDIVISION_PLUGIN_H
#define GRAPHENE_SUBDIVISION_PLUGIN_H
//=============================================================================


#include <graphene/qt/Main_window.h>
#include <graphene/surface_mesh/qt/Surface_mesh_plugin_interface.h>

#include <QComboBox>
#include <QRadioButton>


//=============================================================================


namespace graphene {
namespace qt {


//=============================================================================


/// A plugin for performing subdivision on surface meshes.
class Subdivision_plugin :
        public QObject,
        public Surface_mesh_plugin_interface
{
    Q_OBJECT
    Q_INTERFACES(graphene::qt::Surface_mesh_plugin_interface)
    Q_PLUGIN_METADATA(IID "graphene.Subdivision_plugin")

public:

    Subdivision_plugin();
    ~Subdivision_plugin();

    void init(Main_window *w);
    void triangle_split();


public slots:
    void subdivide();

protected:
    QRadioButton *rb_loop_, *rb_catmull_, *rb_sqrt3_, *rb_triangulate_,*rb_dilate_,*rb_extension_;
};


//=============================================================================
} // namespcae qt
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_SUBDIVISION_PLUGIN_H
//=============================================================================
