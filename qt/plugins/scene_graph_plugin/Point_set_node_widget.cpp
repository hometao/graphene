#include "Point_set_node_widget.h"


//---------------------------------------------------------------------------


namespace graphene {
namespace qt {


//---------------------------------------------------------------------------


Point_set_node_widget:: Point_set_node_widget(Point_set_node* node,QWidget* parent)
    : Node_widget(parent), node_(node)
{
    init();
}


//---------------------------------------------------------------------------


void Point_set_node_widget::init()
{
    this->setTitle(node_->name().c_str());

    QVBoxLayout* vbl = new QVBoxLayout(this);
    QGridLayout* gl = new QGridLayout();

    QLabel* l_mesh_name = new QLabel(tr("Mesh:"));
    le_mesh_name = new QLineEdit();
    QPushButton* pb_choose = new QPushButton(tr("Choose"));

    le_mesh_name->setText(node_->fileinfo().c_str());

    // sphere radius
    QLabel* l_sphere_radius = new QLabel(tr("Sphere radius:"));

    dsb_sphere_radius = new QDoubleSpinBox();
    dsb_sphere_radius->setDecimals(5);
    dsb_sphere_radius->setRange(0.0,1.0);
    dsb_sphere_radius->setSingleStep(0.005);
    dsb_sphere_radius->setValue(node_->radius());


    // form of visualization
    new QLabel(tr("Visualization"));
    cb_visualization = new QComboBox();
    cb_visualization->addItem(tr("Points"));
    QString draw_mode = node_->get_draw_mode().c_str();
    int select_draw_mode = cb_visualization->findText(draw_mode);
    if (select_draw_mode != -1)
        cb_visualization->setCurrentIndex(select_draw_mode);


    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->addWidget(l_mesh_name);
    hbl->addWidget(le_mesh_name);
    hbl->addWidget(pb_choose);

    // add widget to grid
    gl->addWidget(l_sphere_radius, 3, 0, 1, 1);
    gl->addWidget(dsb_sphere_radius, 3, 1, 1, 1);



    // apply button
    QPushButton* pb_apply = new QPushButton(tr("Apply"));


    // add all components to layout
    vbl->addWidget(pb_apply);
    vbl->addLayout(hbl);
    vbl->addLayout(gl);


    // connect buttons
    // apply button pressed, act accordingly
    connect(pb_apply,
            SIGNAL(clicked(bool)),
            this,
            SLOT(slot_apply_node_changes())
           );


    // choose file name for mesh file
    connect(pb_choose,
            SIGNAL(clicked(bool)),
            this,
            SLOT(slot_choose_mesh()));

}


//---------------------------------------------------------------------------


void Point_set_node_widget::
slot_choose_mesh()
{
    QString file_name = QFileDialog::getOpenFileName(0, tr("Choose Mesh"), "./", tr("Files (*.xyz)"));
    le_mesh_name->setText(file_name);
    //QFileInfo info (fileName);

}


//---------------------------------------------------------------------------


void
Point_set_node_widget::
slot_set_push_button_color()
{
    QPushButton* sender = static_cast<QPushButton*>(this->sender());
    QColor initial_color = sender->palette().color(QPalette::Window);

    QColor new_color = QColorDialog::getColor(initial_color,0,"",QColorDialog::ShowAlphaChannel);
    sender->setPalette(QPalette(new_color));
}


//---------------------------------------------------------------------------


void Point_set_node_widget::
update_node()
{
    std::string file = le_mesh_name->text().toStdString().data();
    //LOG(Log_info) << "file = " << file << std::endl;
    //LOG(Log_info) << "fileinfo() = " << node_->fileinfo() << std::endl;
    if (node_->fileinfo() != file)
    {
        node_->load(file);
        //std::string filename(QFileInfo(file.c_str()).fileName().toStdString().data());
        //node_->name_ += " - " + filename;
    }

    node_->set_radius(dsb_sphere_radius->value());

    node_->set_draw_mode(cb_visualization->currentIndex());

}


//===========================================================================
}
}
//===========================================================================
