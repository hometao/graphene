#include "Selection_plugin_widget.h"

namespace graphene {
namespace qt {

void
Selection_plugin_widget::
setupUi()
{
    gb2 = new QGroupBox("",this);

    pb1 = new QPushButton("Clear");
    pb2 = new QPushButton("Invert");
    pb3 = new QPushButton("All");
    pb4 = new QPushButton("Isolated");
    pb5 = new QPushButton("Grow");
    pb6 = new QPushButton("Load");
    pb7 = new QPushButton("Save");

    vb2 = new QVBoxLayout;
    vb2->addWidget(pb1);
    vb2->addWidget(pb2);
    vb2->addWidget(pb3);
    vb2->addWidget(pb4);
    vb2->addWidget(pb5);
    vb2->addWidget(pb6);
    vb2->addWidget(pb7);
    vb2->addStretch(1);
    gb2->setLayout(vb2);
    vb = new QVBoxLayout(this);
    vb->addWidget(gb2);
    vb->addStretch(1);
}

}
}
