//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================


#include "Surface_mesh_node_widget.h"
#include <QMessageBox>
#include <QString>


//=============================================================================


namespace graphene {
namespace qt {


//=============================================================================


Surface_mesh_node_widget::
Surface_mesh_node_widget(Surface_mesh_node* node,QWidget* parent)
    : Node_widget(parent), node_(node)
{
    init();
}


//---------------------------------------------------------------------------


void Surface_mesh_node_widget::init()
{
    this->setTitle(node_->name().c_str());

    Vec3f color;


    // setup draw mode combo box
    QLabel* l_drawmode = new QLabel(tr("Draw Mode"));
    cb_drawmode = new QComboBox();
    std::vector<std::string> draw_modes = node_->get_draw_modes_menu();

    for (auto dm : draw_modes)
        cb_drawmode->addItem(dm.c_str());

    QString draw_mode = node_->get_draw_mode().c_str();
    int select_draw_mode = cb_drawmode->findText(draw_mode);
    if (select_draw_mode != -1)
        cb_drawmode->setCurrentIndex(select_draw_mode);


    QLabel* l_front_color = new QLabel("Front color:");
    pb_front_color = new QPushButton();
    color = 255 * node_->front_color_;
    pb_front_color->setPalette(QPalette(QColor(color[0],color[1],color[2])));
    pb_front_color->setFlat(true);
    pb_front_color->setAutoFillBackground(true);


    QLabel* l_back_color = new QLabel("Back color:");
    pb_back_color = new QPushButton();
    color = 255 * node_->back_color_;
    pb_back_color->setPalette(QPalette(QColor(color[0],color[1],color[2])));
    pb_back_color->setFlat(true);
    pb_back_color->setAutoFillBackground(true);


    QLabel* l_wire_color = new QLabel(tr("Wireframe color:"));
    pb_wire_color = new QPushButton();
    color = 255 * node_->wire_color_;
    pb_wire_color->setPalette(QPalette(QColor(color[0],color[1],color[2])));
    pb_wire_color->setFlat(true);
    pb_wire_color->setAutoFillBackground(true);


    QLabel* l_ambient = new QLabel("Ambient");
    sb_ambient = new QDoubleSpinBox();
    sb_ambient->setRange(0.0,1.0);
    sb_ambient->setSingleStep(0.1);
    sb_ambient->setValue(node_->material_[0]);


    QLabel* l_diffuse = new QLabel("Diffuse");
    sb_diffuse = new QDoubleSpinBox();
    sb_diffuse->setRange(0.0,1.0);
    sb_diffuse->setSingleStep(0.1);
    sb_diffuse->setValue(node_->material_[1]);


    QLabel* l_specular = new QLabel("Specular");
    sb_specular = new QDoubleSpinBox();
    sb_specular->setRange(0.0,1.0);
    sb_specular->setSingleStep(0.1);
    sb_specular->setValue(node_->material_[2]);


    QLabel* l_shininess = new QLabel("Shininess");
    sb_shininess = new QDoubleSpinBox();
    sb_shininess->setRange(1.0,100.0);
    sb_shininess->setSingleStep(1.0);
    sb_shininess->setValue(node_->material_[3]);


    QLabel* l_crease_angle = new QLabel(tr("Crease Angle"));
    sb_crease_angle = new QDoubleSpinBox();
    sb_crease_angle->setRange(0,180);
    sb_crease_angle->setValue(node_->crease_angle_);


    // info button
    QPushButton* pb_info = new QPushButton(tr("Mesh Info"));

    // apply button
    QPushButton* pb_apply = new QPushButton(tr("Apply Settings"));


    // add widgets to grid layout
    QGridLayout* gl  = new QGridLayout();
    int i=0;

    gl->addWidget(l_drawmode, i, 0, 1, 1);
    gl->addWidget(cb_drawmode, i, 1, 1, 1);
    ++i;

    gl->addWidget(l_front_color, i, 0, 1, 1);
    gl->addWidget(pb_front_color, i, 1, 1, 1);
    ++i;

    gl->addWidget(l_back_color, i, 0, 1, 1);
    gl->addWidget(pb_back_color, i, 1, 1, 1);
    ++i;

    gl->addWidget(l_wire_color, i, 0, 1, 1);
    gl->addWidget(pb_wire_color, i, 1, 1, 1);
    ++i;

    gl->addWidget(l_ambient, i, 0, 1, 1);
    gl->addWidget(sb_ambient, i, 1, 1, 1);
    ++i;

    gl->addWidget(l_diffuse, i, 0, 1, 1);
    gl->addWidget(sb_diffuse, i, 1, 1, 1);
    ++i;

    gl->addWidget(l_specular, i, 0, 1, 1);
    gl->addWidget(sb_specular, i, 1, 1, 1);
    ++i;

    gl->addWidget(l_shininess, i, 0, 1, 1);
    gl->addWidget(sb_shininess, i, 1, 1, 1);
    ++i;

    gl->addWidget(l_crease_angle, i, 0, 1, 1);
    gl->addWidget(sb_crease_angle, i, 1, 1, 1);
    ++i;



    // add all components to layout
    QVBoxLayout* vbl = new QVBoxLayout(this);
    vbl->addLayout(gl);
    vbl->addWidget(pb_apply);
    vbl->addWidget(pb_info);


    // connect buttons
    connect(pb_wire_color,  SIGNAL(clicked(bool)),
            this,           SLOT(slot_set_push_button_color()));
    connect(pb_front_color, SIGNAL(clicked(bool)),
            this,           SLOT(slot_set_push_button_color()));
    connect(pb_back_color,  SIGNAL(clicked(bool)),
            this,           SLOT(slot_set_push_button_color()));
    connect(pb_apply,       SIGNAL(clicked(bool)),
            this,           SLOT(slot_apply_node_changes()));
    connect(pb_info,        SIGNAL(clicked(bool)),
            this,           SLOT(slot_mesh_info()));
}


//---------------------------------------------------------------------------


void
Surface_mesh_node_widget::
slot_set_push_button_color()
{
    QPushButton* sender = static_cast<QPushButton*>(this->sender());
    QColor initial_color = sender->palette().color(QPalette::Window);
    sender->setPalette(QPalette(QColorDialog::getColor(initial_color)));
}


//---------------------------------------------------------------------------


void 
Surface_mesh_node_widget::
update_node()
{
    QColor color;

    color = pb_front_color->palette().color(QPalette::Window);
    node_->front_color_ = Vec3f(color.red()/255.0, color.green()/255.0, color.blue()/255.0);

    color = pb_back_color->palette().color(QPalette::Window);
    node_->back_color_ = Vec3f(color.red()/255.0, color.green()/255.0, color.blue()/255.0);

    color = pb_wire_color->palette().color(QPalette::Window);
    node_->wire_color_ = Vec3f(color.red()/255.0, color.green()/255.0, color.blue()/255.0);

    node_->material_[0] = sb_ambient->value();
    node_->material_[1] = sb_diffuse->value();
    node_->material_[2] = sb_specular->value();
    node_->material_[3] = sb_shininess->value();

    node_->crease_angle_ = sb_crease_angle->value();
    node_->update_mesh();

    node_->set_draw_mode(cb_drawmode->currentIndex());
}


//---------------------------------------------------------------------------


void 
Surface_mesh_node_widget::
slot_mesh_info()
{
    QString s("Mario was here!");
    QMessageBox::information(this, tr("Mesh Statistics"), s);
}


//===========================================================================
} // namespace qt
} // namespace graphene
//===========================================================================
