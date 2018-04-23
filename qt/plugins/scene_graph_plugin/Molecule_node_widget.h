#ifndef GRAPHENE_MOLECULE_NODE_WIDGET_H
#define GRAPHENE_MOLECULE_NODE_WIDGET_H

#include <graphene/molecule/scene_graph/Molecule_node.h>
#include "Node_widget.h"

namespace graphene {
namespace qt {

// ui-widget for representing an material node
class Molecule_node_widget : public Node_widget
{
    Q_OBJECT

public:
    Molecule_node_widget(Molecule_node* node, QWidget* parent=0);

    virtual void init();
    virtual void update_node();

protected:
    Molecule_node* node_;

private:

    QDoubleSpinBox* sb_ambient;
    QDoubleSpinBox* sb_diffuse;
    QDoubleSpinBox* sb_specular;
    QDoubleSpinBox* sb_shininess;
    QDoubleSpinBox* sb_sphere_radius;
    QDoubleSpinBox* sb_cylinder_radius;
};

}
}

#endif

