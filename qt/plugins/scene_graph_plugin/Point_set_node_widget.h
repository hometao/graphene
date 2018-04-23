#ifndef GRAPHENE_POINT_SET_NODE_WIDGET_H
#define GRAPHENE_POINT_SET_NODE_WIDGET_H

#include <QFileDialog>
#include <QComboBox>

#include "Node_widget.h"
#include <graphene/scene_graph/Point_set_node.h>

namespace graphene {
namespace qt {

// ui-widget to represent a point set node
class Point_set_node_widget : public Node_widget
{
    Q_OBJECT

public:
    Point_set_node_widget(Point_set_node* node, QWidget* parent = 0);

    virtual void init();
    virtual void update_node();

protected:
    Point_set_node* node_;

private:
    QLineEdit* le_mesh_name;

    QDoubleSpinBox* dsb_sphere_radius;
    QComboBox* cb_visualization;

public slots:
    void slot_set_push_button_color();
    void slot_choose_mesh();
};

}
}
#endif
