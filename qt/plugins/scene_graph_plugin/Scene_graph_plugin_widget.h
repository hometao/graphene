//=============================================================================

#ifndef GRAPHENE_SCENE_GRAPH_PLUGIN_WIDGET_H
#define GRAPHENE_SCENE_GRAPH_PLUGIN_WIDGET_H

//=============================================================================

#include <QGroupBox>
#include <QVBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QMenu>

#include <graphene/scene_graph/Scene_graph.h>
#include <graphene/scene_graph/Base_node.h>

#include "Surface_mesh_node_widget.h"
#include "Point_set_node_widget.h"
#include "Knot_node_widget.h"
#include "Molecule_node_widget.h"

//=============================================================================

namespace graphene {
namespace qt {

//=============================================================================


// ui-widget to represent the scene graph
class Scene_graph_plugin_widget : public QWidget
{
    Q_OBJECT

    // items for the standard model in the treeview of the scene graph.
    // just added a pointer to Base_node to connect an item in the
    // treeview with a node in the scene_graph
    class MyStandardItem : public QStandardItem
    {
    public:
        MyStandardItem(const QString& text, Base_node* _node)
            : QStandardItem(text),node(_node)
        {}

    public:
        Base_node* node;
    };


public:

    Scene_graph_plugin_widget(Scene_graph* _graph);
    virtual ~Scene_graph_plugin_widget();

    void setupUi();

    // adds to the parent-item all his children-items, keeping the scene graph
    // hierarchy
    void add_children(QStandardItem* parent,Base_node* _node);


    // returns the node selected in the treeview
    Base_node* selected_node();

    // creates ui-widget for node
    Node_widget* create_widget(Base_node* node);


public:
    QVBoxLayout* vbl_rendering;
    QGroupBox* gb_node_details;
    QTreeView* tv_scene_graph;


private:
    // used scene graph
    Scene_graph* graph_;

    // represents the scene graph for the treeview
    QStandardItemModel* model_;


protected:
    virtual void showEvent ( QShowEvent* event );


private:
    // update treeview model
    void update_model();


public slots:

    // shows the corresponding ui-widget for the selected treeview node
    void slot_show_node_details();

    // updates the treeview model, corresponding to the scene graph
    void update_treeview();


signals:

    // signals for scene graph plugin to react on context menu action
    void signal_scene_graph_changed();
};


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_SCENE_GRAPH_PLUGIN_WIDGET_H
//=============================================================================
