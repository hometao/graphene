#include "Knot_node_widget.h"

#include <graphene/qt/events.h>

namespace graphene {
namespace qt {


//---------------------------------------------------------------------------


Knot_node_widget:: Knot_node_widget(Knot_node* node,QWidget* parent)
    : Node_widget(parent), node_(node)
{
    init();
}


//---------------------------------------------------------------------------


void Knot_node_widget::init()
{
    this->setTitle(node_->name().c_str());


    Vec3f color;


    QLabel* l_sphere_color = new QLabel("Sphere color");
    pb_sphere_color = new QPushButton();
    color = 255 * node_->sphere_color_;
    pb_sphere_color->setPalette(QPalette(QColor(color[0],color[1],color[2])));
    pb_sphere_color->setFlat(true);
    pb_sphere_color->setAutoFillBackground(true);


    QLabel* l_cylinder_color = new QLabel("Cylinder color");
    pb_cylinder_color = new QPushButton();
    color = 255 * node_->cylinder_color_;
    pb_cylinder_color->setPalette(QPalette(QColor(color[0],color[1],color[2])));
    pb_cylinder_color->setFlat(true);
    pb_cylinder_color->setAutoFillBackground(true);
    

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


    QLabel* l_sphere_radius = new QLabel("Sphere radius (Ball & Stick):");
    sb_sphere_radius = new QDoubleSpinBox();
    sb_sphere_radius->setRange(0.0,32.0);
    sb_sphere_radius->setDecimals(3);
    sb_sphere_radius->setSingleStep(0.01);
    sb_sphere_radius->setValue(node_->sphere_radius_);


    QLabel* l_cylinder_radius = new QLabel("Cylinder radius (Ball & Stick):");
    sb_cylinder_radius = new QDoubleSpinBox();
    sb_cylinder_radius->setRange(0.0,32.0);
    sb_cylinder_radius->setSingleStep(0.01);
    sb_cylinder_radius->setDecimals(3);
    sb_cylinder_radius->setValue(node_->cylinder_radius_);



    // add widget to grid
    QGridLayout* gl = new QGridLayout();
    int i=0;
    gl->addWidget(l_sphere_color, i, 0, 1, 1);
    gl->addWidget(pb_sphere_color, i, 1, 1, 1);
    ++i;
    gl->addWidget(l_cylinder_color, i, 0, 1, 1);
    gl->addWidget(pb_cylinder_color, i, 1, 1, 1);
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
    gl->addWidget(l_sphere_radius, i, 0, 1, 1);
    gl->addWidget(sb_sphere_radius, i, 1, 1, 1);
    ++i;
    gl->addWidget(l_cylinder_radius, i, 0, 1, 1);
    gl->addWidget(sb_cylinder_radius, i, 1, 1, 1);
    

    // apply button
    pb_apply = new QPushButton("Apply");


    // add all components to layout
    QVBoxLayout* vbl = new QVBoxLayout(this);
    vbl->addLayout(gl);
    vbl->addWidget(pb_apply);


    // connect buttons
    connect(pb_apply,SIGNAL(clicked(bool)),this,SLOT(slot_apply_node_changes()));
    connect(pb_sphere_color,SIGNAL(clicked(bool)),this,SLOT(slot_set_push_button_color()));
    connect(pb_cylinder_color,SIGNAL(clicked(bool)),this,SLOT(slot_set_push_button_color()));
}


//---------------------------------------------------------------------------


void
Knot_node_widget::
slot_set_push_button_color()
{
    QPushButton* sender = static_cast<QPushButton*>(this->sender());
    QColor initial_color = sender->palette().color(QPalette::Window);
    sender->setPalette(QPalette(QColorDialog::getColor(initial_color)));
}


//---------------------------------------------------------------------------


void Knot_node_widget::
update_node()
{
    QColor color;

    color = pb_sphere_color->palette().color(QPalette::Window);
    node_->sphere_color_ = Vec3f(color.red()/255.0, color.green()/255.0, color.blue()/255.0);

    color = pb_cylinder_color->palette().color(QPalette::Window);
    node_->cylinder_color_ = Vec3f(color.red()/255.0, color.green()/255.0, color.blue()/255.0);

    node_->material_[0]     = sb_ambient->value();
    node_->material_[1]     = sb_diffuse->value();
    node_->material_[2]     = sb_specular->value();
    node_->material_[3]     = sb_shininess->value();

    node_->sphere_radius_   = sb_sphere_radius->value();
    node_->cylinder_radius_ = sb_cylinder_radius->value();

    node_->update_knot();

    //this SHOULD work with 'this' as receiver because postEvent dispatches event to the qt event handler...
    QApplication::postEvent(this, new Geometry_changed_event, INT_MAX);
}


//===========================================================================
}
}
//===========================================================================
