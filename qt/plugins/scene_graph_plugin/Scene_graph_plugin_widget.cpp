//=============================================================================

#include "Scene_graph_plugin_widget.h"


#include "Surface_mesh_node_widget.h"
#include "Point_set_node_widget.h"

#ifdef WITH_KNOTS
#include "Knot_node_widget.h"
#endif

#ifdef WITH_MOLECULES
#include "Molecule_node_widget.h"
#endif


//=============================================================================

namespace graphene {
namespace qt {

//=============================================================================


Scene_graph_plugin_widget::
Scene_graph_plugin_widget(Scene_graph* _graph) :
    gb_node_details(0),
    tv_scene_graph(new QTreeView()),
    graph_(_graph),
    model_(new QStandardItemModel())
{
    tv_scene_graph->setModel(model_);
}


//---------------------------------------------------------------------------


Scene_graph_plugin_widget::
~Scene_graph_plugin_widget()
{
    // will the model be deleted by treeview?
    delete model_;
    model_ = 0;

    delete tv_scene_graph;
}


//---------------------------------------------------------------------------


void
Scene_graph_plugin_widget::
setupUi()
{
    // group box rendering
    QGroupBox* gb_rendering = new QGroupBox(this);

    // groub box rendering layout
    vbl_rendering = new QVBoxLayout();

    vbl_rendering->addWidget(tv_scene_graph);
    gb_rendering->setLayout(vbl_rendering);

    //
    QVBoxLayout* vbl = new QVBoxLayout(this);
    vbl->addWidget(gb_rendering);
    vbl->addStretch(1);


    tv_scene_graph->setContextMenuPolicy(Qt::CustomContextMenu);
    update_model();


    // show ui for selected node in treeview
    connect(tv_scene_graph, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(slot_show_node_details()));
}


//---------------------------------------------------------------------------


void
Scene_graph_plugin_widget::
add_children(QStandardItem* parent,Base_node* _node)
{
    std::list<Base_node*>::iterator it     = _node->children().begin();
    std::list<Base_node*>::iterator it_end = _node->children().end();

    while (it != it_end)
    {
        Base_node* child = *it;
        QFileInfo file(child->fileinfo().c_str());
        MyStandardItem* item = new MyStandardItem((child->name() + " ").c_str() +
                                                  file.fileName(),child);

        parent->appendRow(item);
        add_children(item, child);
        ++it;
    }
}


//---------------------------------------------------------------------------


void
Scene_graph_plugin_widget::
update_model()
{
    // Reuse old model
    model_->clear();

    // Reconstruct model out of scene_graph
    MyStandardItem* root = new MyStandardItem("root", graph_->root_);
    add_children(root,graph_->root_);

    model_->appendRow(root);
    model_->setHorizontalHeaderLabels(QStringList() << "Scene graph");
}


//---------------------------------------------------------------------------


void
Scene_graph_plugin_widget::
showEvent(QShowEvent* /*event*/)
{
    update_treeview();
}


//---------------------------------------------------------------------------


void Scene_graph_plugin_widget::
update_treeview()
{
    update_model();
    tv_scene_graph->expandAll();
    slot_show_node_details();
}


//---------------------------------------------------------------------------


Node_widget*
Scene_graph_plugin_widget::
create_widget(Base_node* node)
{
    Node_widget* node_widget = NULL;


    // try all known node types...
    if (dynamic_cast<Surface_mesh_node*>(node))
    {
        node_widget = new Surface_mesh_node_widget(static_cast<Surface_mesh_node*>(node),this);
    }
    else if (dynamic_cast<Point_set_node*>(node))
    {
        node_widget = new Point_set_node_widget(static_cast<Point_set_node*>(node),this);
    }
#ifdef WITH_KNOTS
    else if (dynamic_cast<Knot_node*>(node))
    {
        node_widget = new Knot_node_widget(static_cast<Knot_node*>(node),this);
    }
#endif
#ifdef WITH_MOLECULES
    else if (dynamic_cast<Molecule_node*>(node))
    {
        node_widget = new Molecule_node_widget(static_cast<Molecule_node*>(node),this);
    }
#endif

    // applied changes in this widget results in scene graph changes
    if (node_widget)
    {
        connect(node_widget, SIGNAL(signal_scene_graph_modified()),
                this, SIGNAL(signal_scene_graph_changed()));
    }

    return node_widget;
}


//---------------------------------------------------------------------------


void Scene_graph_plugin_widget::
slot_show_node_details()
{
    Base_node* node = selected_node();

    // in case selection is invalid or root node
    if (node == 0 || node->parent() == 0)
    {
        vbl_rendering->removeWidget(gb_node_details);
        delete gb_node_details;
        gb_node_details = 0;
        return;
    }

    // get widget to make node modifiable
    Node_widget* node_widget = create_widget(node);

    // no matching widget found?
    if (node_widget == 0)
    {
        // no widget for this node, hide details
        vbl_rendering->removeWidget(gb_node_details);
        delete gb_node_details;
        gb_node_details = 0;
    }
    else
    {
        // show details for this node
        delete gb_node_details;
        gb_node_details = node_widget;
        vbl_rendering->addWidget(gb_node_details);

    }
}


//---------------------------------------------------------------------------


Base_node* Scene_graph_plugin_widget::selected_node()
{
    QModelIndex node_index = tv_scene_graph->currentIndex();

    if (node_index == QModelIndex())
    {
        return NULL;
    }

    // selected item in the treeview
    MyStandardItem* node_item = static_cast<MyStandardItem*>(model_->itemFromIndex(node_index));
    return node_item->node;
}


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
