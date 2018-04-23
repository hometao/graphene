//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================
#ifndef SURFACE_MESH_NODE_WIDGET_H
#define SURFACE_MESH_NODE_WIDGET_H
//=============================================================================


#include <QFileDialog>
#include <QComboBox>

#include "Node_widget.h"
#include <graphene/surface_mesh/scene_graph/Surface_mesh_node.h>


//=============================================================================


namespace graphene {
namespace qt {


//=============================================================================


// ui-widget to represent a surface mesh node
class Surface_mesh_node_widget : public Node_widget
{
    Q_OBJECT

public:
    Surface_mesh_node_widget(Surface_mesh_node* node, QWidget* parent = 0);

    virtual void init();
    virtual void update_node();


protected:

    Surface_mesh_node* node_;


private:

    QPushButton*    pb_front_color;
    QPushButton*    pb_back_color;
    QPushButton*    pb_wire_color;
    QPushButton*    pb_info;
    QDoubleSpinBox* sb_crease_angle;
    QDoubleSpinBox* sb_ambient;
    QDoubleSpinBox* sb_diffuse;
    QDoubleSpinBox* sb_specular;
    QDoubleSpinBox* sb_shininess;
    QComboBox*      cb_drawmode;


public slots:

    void slot_set_push_button_color();
    void slot_mesh_info();
};


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
#endif
//=============================================================================
