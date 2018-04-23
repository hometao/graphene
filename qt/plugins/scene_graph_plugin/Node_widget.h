#ifndef NODE_WIDGET_H
#define NODE_WIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTreeView>
#include <QStandardItemModel>

#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>
#include <QColorDialog>

#include <graphene/scene_graph/Base_node.h>
using namespace ::graphene::scene_graph;

namespace graphene {
namespace qt {

// Base class for ui-widgets, that represent scene graph nodes.
class Node_widget : public QGroupBox
{
    Q_OBJECT

public:
    Node_widget(QWidget* parent=0);
    virtual ~Node_widget();

    // initialize ui for this widget
    virtual void init();

    // writes widget data into node
    virtual void update_node() = 0;

    // push button to apply changes made to a node
    QPushButton* pb_apply;

signals:
    void signal_scene_graph_modified();

public slots:
    // update node and emit signal signal_scene_graph_modified
    void slot_apply_node_changes();
};

}
}
#endif
