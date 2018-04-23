#include "Node_widget.h"

namespace graphene {
namespace qt {

Node_widget::Node_widget(QWidget* parent): QGroupBox(parent)
{}

Node_widget::~Node_widget()
{}

void Node_widget::init()
{}

void Node_widget::update_node()
{}

void
Node_widget::slot_apply_node_changes()
{
    update_node();
    emit(signal_scene_graph_modified());
}

}
}
