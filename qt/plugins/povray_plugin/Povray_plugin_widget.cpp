#include "Povray_plugin_widget.h"


// ---------------------------------------------------------------------------


namespace graphene {
namespace qt {

// ---------------------------------------------------------------------------


Povray_plugin_widget::Povray_plugin_widget(const QGL_viewer_widget* _qglviewer)
    : qglviewer_(_qglviewer)
{
}


// ---------------------------------------------------------------------------


Povray_plugin_widget::~Povray_plugin_widget()
{
}


// ---------------------------------------------------------------------------


void
Povray_plugin_widget::
setupUi()
{
    // group box rendering
    QGroupBox* gb_rendering = new QGroupBox(this);
    // groub box rendering layout
    vbl_rendering = new QVBoxLayout();

    // Push button for Povray start
    pb_execute = new QPushButton();
    vbl_rendering->addWidget(pb_execute);

    le_filename = new QLineEdit();
    pb_choose = new QPushButton(tr("Choose"));

    hbl_filename= new QHBoxLayout();
    hbl_filename->addWidget(le_filename);
    hbl_filename->addWidget(pb_choose);
    vbl_rendering->addLayout(hbl_filename);

    // dimension elements
    QGroupBox* gb_geometry = new QGroupBox(tr("Geometry"));
    QHBoxLayout* hbl_1 = new QHBoxLayout();
    l_dimension = new QLabel();
    sb_width = new QSpinBox();
    l_x = new QLabel();
    sb_height = new QSpinBox();
    cb_keep_aspect_ratio = new QCheckBox();
    hbl_1->addWidget(l_dimension);
    hbl_1->addWidget(sb_width);
    hbl_1->addWidget(l_x);
    hbl_1->addWidget(sb_height);
    hbl_1->addWidget(cb_keep_aspect_ratio);

    gb_geometry->setLayout(hbl_1);
    vbl_rendering->addWidget(gb_geometry);


    // antialias elements
    gb_antialias = new QGroupBox(tr("Antialias"));
    vbl_antialias = new QVBoxLayout();
    cb_antialias = new QCheckBox();
    gl_antialias = new QGridLayout();
    l_sampling_method = new QLabel();
    sb_sampling_method = new QSpinBox();
    dsb_threshold = new QDoubleSpinBox();
    l_threshold = new QLabel();
    sb_depth = new QSpinBox();
    l_depth = new QLabel();
    dsb_jitter = new QDoubleSpinBox();
    l_jitter = new QLabel();

    gl_antialias->addWidget(l_sampling_method, 0, 0, 1, 1);
    gl_antialias->addWidget(sb_sampling_method, 0, 1, 1, 1);
    gl_antialias->addWidget(l_depth, 0, 2, 1, 1);
    gl_antialias->addWidget(sb_depth, 0, 3, 1, 1);
    gl_antialias->addWidget(l_threshold, 1, 0, 1, 1);
    gl_antialias->addWidget(dsb_threshold, 1, 1, 1, 1);
    gl_antialias->addWidget(l_jitter, 1, 2, 1, 1);
    gl_antialias->addWidget(dsb_jitter, 1, 3, 1, 1);

    vbl_antialias->addWidget(cb_antialias);
    vbl_antialias->addLayout(gl_antialias);
    gb_antialias->setLayout(vbl_antialias);

    vbl_rendering->addWidget(gb_antialias);


    // quality elements
    QGroupBox* gb_quality = new QGroupBox(tr("Quality"));
    hbl_2 = new QHBoxLayout();
    l_quality = new QLabel();
    sb_quality = new QSpinBox();
    splitter = new QSplitter();
    hbl_2->addWidget(l_quality);
    hbl_2->addWidget(sb_quality);
    hbl_2->addWidget(splitter);
    gb_quality->setLayout(hbl_2);
    vbl_rendering->addWidget(gb_quality);


    // flags elements
    QGroupBox* gb_flags = new QGroupBox(tr("Flags"));
    QHBoxLayout* hbl_3 = new QHBoxLayout();
    cb_no_rendering = new QCheckBox();
    cb_verbose= new QCheckBox();
    cb_display= new QCheckBox();
    splitter = new QSplitter();
    hbl_3->addWidget(cb_no_rendering);
    hbl_3->addWidget(cb_verbose);
    hbl_3->addWidget(cb_display);
    gb_flags->setLayout(hbl_3);
    vbl_rendering->addWidget(gb_flags);


    // elements initialization
    pb_execute->setText(tr("Start Rendering"));
    le_filename->setText(tr("scenegraph"));
    le_filename->selectAll();

    l_dimension->setText(tr("Size:"));
    l_x->setText(tr("x"));
    cb_keep_aspect_ratio->setText(tr("Keep aspect ratio"));
    sb_width->setToolTip(tr("Width of output picture"));
    sb_width->setRange(0,10000);
    sb_width->setValue(qglviewer_->width());
    sb_height->setToolTip(tr("Height of output picture"));
    sb_height->setRange(0,10000);
    sb_height->setValue(qglviewer_->height());
    cb_keep_aspect_ratio->setChecked(false);

    cb_antialias->setText(tr("on/off"));
    cb_antialias->setChecked(true);

    l_sampling_method->setText(tr("Sampling method"));

    sb_sampling_method->setRange(1,2);
    sb_sampling_method->setValue(2);
    l_jitter->setText(tr("Jitter"));
    dsb_jitter->setRange(0.0,1.0);
    dsb_jitter->setValue(1.0);
    dsb_jitter->setSingleStep(0.1);
    dsb_jitter->setToolTip(tr("Wiggles the location of the supersamples."));
    l_threshold->setText(tr("Threshold"));
    dsb_threshold->setRange(0.0,3.0);
    dsb_threshold->setValue(0.3);
    dsb_threshold->setSingleStep(0.1);
    dsb_threshold->setToolTip(tr("Supersample two pixels, if their color differs more than this threshold."));

    l_depth->setText(tr("Depth"));
    sb_depth->setRange(0,9);
    sb_depth->setValue(2);

    l_quality->setText(tr("Quality"));
    sb_quality->setRange(0,11);
    sb_quality->setValue(9);

    cb_no_rendering->setText(tr("No rendering"));
    cb_no_rendering->setChecked(false);
    cb_verbose->setChecked(true);
    cb_verbose->setText(tr("Verbose"));
    cb_verbose->setChecked(true);
    cb_display->setText(tr("Display"));
    cb_display->setChecked(true);

    vbl_rendering->addStretch(1);
    gb_rendering->setLayout(vbl_rendering);


    // comment out this block, if rendering box should not stretch
    // horizontal
    QVBoxLayout* vbl = new QVBoxLayout(this);
    vbl->addWidget(gb_rendering);
    vbl->addStretch(1);


    // connect ui-actions with functionality
    connect(le_filename, SIGNAL(editingFinished()), this, SLOT(slot_update_filename()));
    connect(pb_choose, SIGNAL(clicked()), this, SLOT(slot_choose_filename()));
    connect(cb_antialias, SIGNAL(stateChanged(int)), this, SLOT(slot_state_changed_antialias(int)));
    connect(cb_no_rendering, SIGNAL(stateChanged(int)), this, SLOT(slot_update_pb_execute_title(int)));

    // we want to know, if the qglviewer dimension is changed,
    // so that we can use this dimension as new output dimension.
    connect(qglviewer_, SIGNAL(signal_resized()),this,SLOT(slot_use_qglviewer_dimension()));
}


// ---------------------------------------------------------------------------


void Povray_plugin_widget::slot_choose_filename()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Choose File"), "./", tr("Files (*.*)"));
    QFileInfo info (fileName);

    // remove spaces from filename
    QString output_filename_ = info.absoluteFilePath().replace(" ","_");
    le_filename->setText(output_filename_);
}


// ---------------------------------------------------------------------------


void Povray_plugin_widget::
slot_update_filename()
{
    // remove spaces from filename
    const QString& output_filename = le_filename->text().replace(" ","_");
    le_filename->setText(output_filename);
}


// ---------------------------------------------------------------------------


void
Povray_plugin_widget::
slot_state_changed_antialias(int state)
{
    // turn on/off antialias options, depending on state
    sb_sampling_method->setEnabled(state);
    sb_depth->setEnabled(state);
    dsb_jitter->setEnabled(state);
    dsb_threshold->setEnabled(state);
}


// ---------------------------------------------------------------------------


void
Povray_plugin_widget::
slot_use_qglviewer_dimension()
{
    // use qglviewer dimensions for output picture dimension
    sb_width->setValue(qglviewer_->width());
    sb_height->setValue(qglviewer_->height());

}


// ---------------------------------------------------------------------------


void
Povray_plugin_widget::
slot_update_pb_execute_title(int state)
{
    if (state)
    {
        pb_execute->setText(tr("Generate files"));
    }
    else
    {
        pb_execute->setText(tr("Start Rendering"));
    }
}


// ---------------------------------------------------------------------------

}
}
