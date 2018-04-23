//=============================================================================

#ifndef GRAPHENE_SCENE_GRAPH_PLUGIN_H
#define GRAPHENE_SCENE_GRAPH_PLUGIN_H

//=============================================================================

#include <graphene/qt/Main_window.h>
#include <graphene/qt/plugins/interfaces/Plugin_interface.h>
#include <graphene/qt/plugins/scene_graph_plugin/Scene_graph_plugin_widget.h>

//=============================================================================

namespace graphene {
namespace qt {

//=============================================================================


/// A plugin to insepct and modify the object of the scene.
class Scene_graph_plugin :
        public QObject,
        public Plugin_interface
{
    Q_OBJECT
    Q_INTERFACES(graphene::qt::Plugin_interface)
    Q_PLUGIN_METADATA(IID "graphene.Scene_graph_plugin")


public:

    Scene_graph_plugin();
    ~Scene_graph_plugin();

    void init(Main_window* w);


private slots:

    void slot_mouse_event(QMouseEvent* /*_event*/){}
    void slot_mouse_mode_changed(Mouse_mode /*_m*/){}
    void slot_key_press_event(QKeyEvent* /*_event*/){}
};


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_SCENE_GRAPH_PLUGIN_H
//=============================================================================
