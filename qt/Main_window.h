//=============================================================================
// Copyright (C) 2013 by Graphics & Geometry Group, Bielefeld University
//=============================================================================
#ifndef GRAPHENE_MAIN_WINDOW_H
#define GRAPHENE_MAIN_WINDOW_H
//=============================================================================


//== INCLUDES =================================================================

#include <QMainWindow>
#include <QString>
#include <QLabel>

#include "ui_Main_window.h"

#include <graphene/qt/Qdebug_stream.h>
#include <graphene/qt/QGL_viewer_widget.h>
#include <graphene/scene_graph/Scene_graph.h>


//== NAMESPACES ===============================================================

namespace graphene {
namespace qt {


//== CLASS DEFINITION =========================================================


class Plugin_interface;
class IO_plugin_interface;
class Manipulation_plugin_interface;


/// \addtogroup qt qt
/// @{


/// The main application window for graphene.
class Main_window : public QMainWindow, Ui_Main_window
{
    Q_OBJECT

public:

    typedef graphene::scene_graph::Scene_graph Scene_graph;
    typedef graphene::scene_graph::Base_node Base_node;
    typedef graphene::scene_graph::Object_node Object_node;


public:
    Main_window();
    ~Main_window();
    void open(const QString& filename,int modle=0);

public:

    QGL_viewer_widget* qglviewer_;
    Scene_graph*       scene_graph_;
    QToolBox*          toolbox_;
    QToolBar*          toolbar_;
    QTextEdit*         console_;
    Qdebug_stream*     qout_;
    Qdebug_stream*     qerr_;
    QStringList        filters_;
    QStringList        manipulation_modes_;
    QLabel*            fps_label_;

    std::list<Manipulation_plugin_interface*> manipulation_plugins_;

signals:

    void signal_key_press_event(QKeyEvent* event);
    void signal_quit();

    void signal_scene_graph_changed();


protected slots:

    void on_actionOpen_triggered();
    void actionOpenRecentFile_triggered();
    void actionSelectToolboxItem_triggered();
    void on_actionClose_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionRevert_triggered();
    void on_actionQuit_triggered();
    void on_actionFullscreen_triggered();
    void on_actionResetView_triggered();
    void on_actionRestoreView_triggered();
    void on_actionSetView_triggered();
    void on_actionSaveView_triggered();
    void on_actionLoadView_triggered();
    void on_actionScreenshot_triggered();
    void on_actionAbout_triggered();
    void on_actionMoveMode_triggered();
    void on_actionSelectionMode_triggered();
    void on_actionManipulationMode_triggered();
	void on_actionLoadFeature_triggered();
	void on_actionDilateLine_triggered();

    void slot_update_statusbar();


protected:

    void keyPressEvent(QKeyEvent*);
    void customEvent(QEvent *event);

    void create_recent_files_menu();
    void update_recent_file_actions();
    void set_current_file(const QString& filename);

    void update_toolbox_shortcuts();

    void load_plugins();

    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);


protected:

    std::list<Plugin_interface*>    plugins;
    std::list<IO_plugin_interface*> io_plugins;
    std::list<QObject*>             receivers;
    std::list<QAction*>             toolbox_actions_;

    static const int s_max_recent_files = 5;

    QAction*    recent_file_actions[s_max_recent_files];
    QAction*    separator;
    QString     current_file_name;
    QStringList data_types_;

};


//=============================================================================
/// @}
//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_MAIN_WINDOW_H
//=============================================================================
