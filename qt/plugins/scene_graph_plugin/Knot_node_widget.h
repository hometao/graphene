#ifndef KNOT_NODE_WIDGET_H
#define KNOT_NODE_WIDGET_H

#include <graphene/knot/scene_graph/Knot_node.h>
#include "Node_widget.h"

namespace graphene {
namespace qt {

// ui-widget for representing an material node
class Knot_node_widget : public Node_widget
{
    Q_OBJECT

public:
    Knot_node_widget(Knot_node* node, QWidget* parent=0);

    virtual void init();
    virtual void update_node();

protected:
    Knot_node* node_;

private:
    QPushButton*    pb_sphere_color;
    QPushButton*    pb_cylinder_color;
    QDoubleSpinBox* sb_ambient;
    QDoubleSpinBox* sb_diffuse;
    QDoubleSpinBox* sb_specular;
    QDoubleSpinBox* sb_shininess;
    QDoubleSpinBox* sb_sphere_radius;
    QDoubleSpinBox* sb_cylinder_radius;

public slots:
    void slot_set_push_button_color();
};

}
}

#endif
