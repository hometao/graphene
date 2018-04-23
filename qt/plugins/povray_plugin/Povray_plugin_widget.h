#ifndef GRAPHENE_POVRAY_PLUGIN_WIDGET_H
#define GRAPHENE_POVRAY_PLUGIN_WIDGET_H

#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QSplitter>
#include <QFileDialog>

#include <graphene/qt/QGL_viewer_widget.h>

namespace graphene {
namespace qt {


// ui widget for the Povray plugin
class Povray_plugin_widget : public QWidget
{

    Q_OBJECT

public:

    // Constructor expects pointer to the qglviewer
    Povray_plugin_widget(const QGL_viewer_widget* qglviewer);
    virtual ~Povray_plugin_widget();

public:
    void setupUi();

public slots:
    // get the file name prefix from the save file dialog
    // for the Povray files (spaces are removed).
    void slot_choose_filename();

    // get the file name from the lineedit field (spaces
    // are removed.
    void slot_update_filename();

    // turns antialias options on/off depending on state
    void slot_state_changed_antialias(int state);

    // changes the title of pb_execute depending on
    // the state (of cb_no_rendering)
    void slot_update_pb_execute_title(int state);

    // uses the qglviewer dimension as output dimension
    void slot_use_qglviewer_dimension();

public:
    // used for the elements of the ui
    // most of them are probably not needed to be specified
    // in the class definition
    QGroupBox* gb2;
    QPushButton*pb_execute;
    QLineEdit*le_filename;
    QPushButton* pb_choose;

    QLabel* l_dimension, *l_x;
    QSpinBox* sb_width,*sb_height;
    QCheckBox* cb_keep_aspect_ratio;
    QCheckBox*cb_antialias;

    QGroupBox* gb_antialias;
    QVBoxLayout* vbl_antialias;
    QGridLayout* gl_antialias;
    QLabel* l_sampling_method;
    QLabel* l_depth;
    QLabel* l_threshold;
    QLabel* l_jitter;
    QSpinBox* sb_sampling_method;
    QSpinBox* sb_depth;
    QDoubleSpinBox* dsb_threshold;
    QDoubleSpinBox* dsb_jitter;

    QLabel* l_quality;
    QSpinBox* sb_quality;
    QCheckBox* cb_no_rendering;
    QCheckBox* cb_verbose;
    QCheckBox* cb_display;

    QVBoxLayout* vbl_rendering;
    QHBoxLayout* hbl_filename;
    QHBoxLayout* hbl_1;
    QHBoxLayout* hbl_2;
    QSplitter* splitter;

private:
    // pointer to qglviewer, used to obtain
    // dimension for output picture
    const QGL_viewer_widget* qglviewer_;


};

}
}

#endif // GRAPHENE_POVRAY_PLUGIN_WIDGET_H
