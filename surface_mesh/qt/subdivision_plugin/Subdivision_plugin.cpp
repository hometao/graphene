//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================


#include <graphene/surface_mesh/qt/subdivision_plugin/Subdivision_plugin.h>
#include <graphene/surface_mesh/algorithms/subdivision/sqrt3_subdivision.h>
#include <graphene/surface_mesh/algorithms/subdivision/loop_subdivision.h>
#include <graphene/surface_mesh/algorithms/subdivision/catmull_clark_subdivision.h>
#include <graphene/surface_mesh/algorithms/subdivision/line_dilate.h>
#include <graphene/surface_mesh/algorithms/subdivision/feature extension.h>

#include <QToolBox>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <sstream>


//=============================================================================


namespace graphene {
namespace qt {


//=============================================================================


Subdivision_plugin::
Subdivision_plugin()
{
    name_ = "Subdivision_plugin";
}


//-----------------------------------------------------------------------------


Subdivision_plugin::
~Subdivision_plugin()
{
}


//-----------------------------------------------------------------------------


void
Subdivision_plugin::
init(Main_window *w)
{
    main_window_ = w;
    QToolBox *toolbox = main_window_->toolbox_;

    QGroupBox* groupBox = new QGroupBox("", toolbox);

    rb_loop_        = new QRadioButton("Loop", toolbox);
    rb_catmull_     = new QRadioButton("Catmull Clark", toolbox);
    rb_sqrt3_       = new QRadioButton("Sqrt(3)", toolbox);
    rb_triangulate_ = new QRadioButton("Triangle-Split", toolbox);
	rb_dilate_      = new QRadioButton("Dilate Feature Line", toolbox);
	rb_extension_   = new QRadioButton("Feature Extension", toolbox);

    QButtonGroup* bg = new QButtonGroup(this);
    bg->addButton(rb_loop_);
    bg->addButton(rb_catmull_);
    bg->addButton(rb_sqrt3_);
    bg->addButton(rb_triangulate_);
	bg->addButton(rb_dilate_);
	bg->addButton(rb_extension_);

    QPushButton* pb_subdivide = new QPushButton("Subdivide", toolbox);
    connect(pb_subdivide, SIGNAL(clicked()), this, SLOT(subdivide()));

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(rb_loop_);
    vbox->addWidget(rb_catmull_);
    vbox->addWidget(rb_sqrt3_);
    vbox->addWidget(rb_triangulate_);
	vbox->addWidget(rb_dilate_);
	vbox->addWidget(rb_extension_);
    vbox->addWidget(pb_subdivide);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    toolbox->addItem(groupBox, "Subdivision");
    toolbox->setItemIcon(toolbox->indexOf(groupBox),QIcon(":/graphene/icons/subdivision.png"));
}


//-----------------------------------------------------------------------------


void
Subdivision_plugin::
subdivide()
{
    Surface_mesh_node* node = selected_node();

    if (node)
    {
        if (rb_sqrt3_->isChecked())
        {
            sqrt3_subdivision(node->mesh_);
        }
        else if (rb_loop_->isChecked())
        {
            loop_subdivision(node->mesh_);
        }
        else if (rb_catmull_->isChecked())
        {
            catmull_clark_subdivision(node->mesh_);
        }
		else if (rb_triangulate_->isChecked())
		{
			triangle_split();
		}
		else if(rb_dilate_->isChecked())
		{
			node->is_visual = false;
			line_dilate(node->mesh_);
		}
		else if (rb_extension_->isChecked())
		{
			feature_extension(node->mesh_);
		}
        node->update_mesh();
        QApplication::postEvent(main_window_, new Geometry_changed_event());
    }
}


//-----------------------------------------------------------------------------


void
Subdivision_plugin::
triangle_split()
{
    Surface_mesh_node* node = selected_node();

    if (node)
    {
        auto points = node->mesh_.get_vertex_property<Point>("v:point");

        for (auto f : node->mesh_.faces())
        {
            if (node->mesh_.valence(f) > 3)
            {
                Point  c(0,0,0);
                Scalar n(0);

                for (auto v : node->mesh_.vertices(f))
                {
                    c += points[v];
                    ++n;
                }

                c /= n;

                node->mesh_.split(f, c);
            }
        }
    }
}


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
