//=============================================================================

#include "Scene_graph_plugin.h"

//=============================================================================

namespace graphene {
namespace qt {

//=============================================================================

Scene_graph_plugin_widget* ui;

//=============================================================================


Scene_graph_plugin::
Scene_graph_plugin()
{
    name_ = "Scene_graph_plugin";
}


//---------------------------------------------------------------------------


Scene_graph_plugin::
~Scene_graph_plugin()
{}


//---------------------------------------------------------------------------


void Scene_graph_plugin::init(Main_window* w)
{
    main_window_ = w;

    // create plugin-ui
    ui = new Scene_graph_plugin_widget(main_window_->scene_graph_);
    ui->setupUi();

    // add plugin-ui to toolbox
    QToolBox*toolbox_ = main_window_->toolbox_;
    toolbox_->addItem(ui,"Scene graph");
    toolbox_->setItemIcon(toolbox_->indexOf(ui),QIcon(":/graphene/icons/scene_graph.png"));
    ui->show();

    connect(main_window_->qglviewer_,
            SIGNAL(signal_mouse_mode_changed(Mouse_mode)),
            this,
            SLOT(slot_mouse_mode_changed(Mouse_mode)));

    // inform qglviewer that scene graph has changed
    connect(ui,
            SIGNAL(signal_scene_graph_changed()),
            main_window_->qglviewer_,
            SLOT(model_changed_triggered())
           );

    // update ui treeview, if new mesh is loaded
    connect(main_window_,
            SIGNAL(signal_scene_graph_changed()),
            ui,
            SLOT(update_treeview())
           );
}


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
