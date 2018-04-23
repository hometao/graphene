#ifndef GRAPHENE_POVRAY_PLUGIN_H
#define GRAPHENE_POVRAY_PLUGIN_H

#include <QProcess>
#include <QLabel>
#include <QPixmap>

#include <graphene/qt/Main_window.h>
#include <graphene/qt/plugins/interfaces/Plugin_interface.h>

#include <graphene/qt/plugins/povray_plugin/Povray_plugin_widget.h>
#include <graphene/qt/plugins/povray_plugin/Scene_graph_converter.h>

#include <graphene/scene_graph/Camera_node.h>
#include <graphene/scene_graph/Light_node.h>
#include <graphene/scene_graph/Transformation_node.h>


namespace graphene {
namespace qt {


/// A plugin to generate Povray files from the scene graph and to render the
/// scene afterwards.
class Povray_plugin :
    public QObject,
    public Plugin_interface
{
    Q_OBJECT
    Q_INTERFACES(graphene::qt::Plugin_interface)
    Q_PLUGIN_METADATA(IID "graphene.Povray_plugin")

public :

        Povray_plugin();
    ~Povray_plugin();

    void init(Main_window* w);

private:
    // adds elements to the scene graph, that are already in
    // eye-coordinates
    virtual void add_eye_coordinates_elements_to_scene_graph();

    // removes elements from the scene graph previously added to
    // restore the scene graph
    virtual void remove_eye_coordinates_elements_from_scene_graph();

    // computes the qglviewers current aspect ratio
    float current_aspect_ratio();

public slots:

    // generates Povray files for the scene graph and
    // renders the scene
    void slot_render_scene();

    // presents rendering result
    void slot_rendering_finished();

    // set new height for picture, in case the aspect ratio
    // should not be changed.
    void slot_check_height(int value);

    // set new width for picture, in case the aspect ratio
    // should not be changed.
    void slot_check_width(int value);

    // adjust the dimension for the picture to keep the aspect ratio
    void slot_update_aspect_ratio();

private slots:
//    void slot_mouse_event(QMouseEvent* /*_event*/){}
//    void slot_mouse_mode_changed(Mouse_mode _m){}
//    void slot_key_press_event(QKeyEvent* _event){}

private:
    // used to execute Povray
    QProcess* proc_;

    // prefix part for the name of the Povray files (.{pov,ini})
    QString povray_file_name_prefix_;

    // saves the pointer to the ui-widget for this plugin
    Povray_plugin_widget* ui_;

    // contains the original root of the scene graph
    Base_node* old_root_;

    // container of added nodes to the scene graph
    std::vector<Base_node*> hardcoded_nodes_;



};

}
}

#endif // GRAPHENE_POVRAY_PLUGIN_H
