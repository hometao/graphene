//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================
#ifndef GRAPHENE_SELECTION_PLUGIN_H
#define GRAPHENE_SELECTION_PLUGIN_H
//=============================================================================



//== INCLUDES =================================================================


#include <graphene/qt/Main_window.h>
#include <graphene/qt/plugins/interfaces/Plugin_interface.h>
#include <graphene/qt/plugins/interfaces/Mouse_plugin_interface.h>
#include <graphene/qt/plugins/interfaces/Keyboard_plugin_interface.h>
#include <graphene/qt/plugins/selection_plugin/Selection_plugin_widget.h>
#include <graphene/scene_graph/Lasso_selection_node.h>


//=============================================================================


namespace graphene {
namespace qt {

using graphene::scene_graph::Lasso_selection_node;


//=============================================================================


/// \addtogroup qt
/// @{


/// A plugin to handle standard selection oprations
class Selection_plugin : public QObject,
                         public Plugin_interface,
                         public Mouse_plugin_interface,
                         public Keyboard_plugin_interface
{
    Q_OBJECT
    Q_INTERFACES(graphene::qt::Plugin_interface)
    Q_INTERFACES(graphene::qt::Mouse_plugin_interface)
    Q_INTERFACES(graphene::qt::Keyboard_plugin_interface)
    Q_PLUGIN_METADATA(IID "graphene.Selection_plugin")


public:

    Selection_plugin();
    ~Selection_plugin();
    void init(Main_window *w);


public slots:

    void slot_clear_selections();
    void slot_invert_selections();
    void slot_select_all();
    void slot_select_isolated();
    void slot_grow_selection();
    void slot_load();
    void slot_save();


protected:

    Mouse_mode mouse_mode_;
    Lasso_selection_node* lasso_;


protected:

    void load_selection(const std::string& filename);
    void save_selection(const std::string& filename);

    void select_lasso(QMouseEvent* _event);
    void select_point(QMouseEvent* _event);

    bool pick(int x, int y, Vec3f& _p);


private slots:

    void slot_mouse_event(QMouseEvent* _event);
    void slot_mouse_mode_changed(Mouse_mode _m);
    void slot_key_press_event(QKeyEvent* _event);


};


//=============================================================================
/// @}
//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_SELECTION_PLUGIN_H
//=============================================================================
