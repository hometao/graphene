//== INCLUDES =================================================================

#include "Remeshing_plugin.h"
#include <graphene/surface_mesh/algorithms/remeshing/Remesher.h>
#include <graphene/utility/Stop_watch.h>
#include <graphene/macros.h>

#include <sstream>
#include <QToolBox>
#include <QDoubleValidator>



//== NAMESPACE ================================================================


namespace graphene {
namespace qt {


//== IMPLEMENTATION ===========================================================


Remeshing_plugin::
Remeshing_plugin()
{
    name_ = "Remeshing_plugin";
}


//-----------------------------------------------------------------------------


void
Remeshing_plugin::
init(Main_window *w)
{
    main_window_ = w;
    QToolBox *toolbox_ = main_window_->toolbox_;

    widget_ = new Remeshing_plugin_widget();
    widget_->setupUi(widget_);

    QDoubleValidator* double_validator = new QDoubleValidator(widget_);
	QIntValidator* int_validator = new QIntValidator(widget_);
    widget_->le_edge_length->setValidator(double_validator);
    widget_->le_error->setValidator(double_validator);
    widget_->le_min_length->setValidator(double_validator);
    widget_->le_max_length->setValidator(double_validator);
	//widget_->ridge_id->setValidator(int_validator);
	//widget_->ravine_id->setValidator(int_validator);

    connect(widget_->pb_uniform_remesh, SIGNAL(clicked()),
            this, SLOT(slot_uniform_remeshing()));
    connect(widget_->pb_adaptive_remesh, SIGNAL(clicked()),
            this, SLOT(slot_adaptive_remeshing()));
    connect(widget_->pb_mean_edge_length, SIGNAL(clicked()),
            this, SLOT(slot_mean_edge_length()));
	connect(widget_->select_ridge, SIGNAL(clicked()),
		    this, SLOT(slot_select_ridge()));
	connect(widget_->select_ravine, SIGNAL(clicked()),
		this, SLOT(slot_select_ravine()));
	connect(widget_->calculate_line, SIGNAL(clicked()),
		this, SLOT(slot_calculate_line()));

    toolbox_->addItem(widget_,"Remeshing");
    toolbox_->setItemIcon(toolbox_->indexOf(widget_),QIcon(":/graphene/icons/remeshing.png"));
}


//-----------------------------------------------------------------------------


void
Remeshing_plugin::
slot_uniform_remeshing()
{
    Surface_mesh_node* node = selected_node();

    if (node)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        Surface_mesh* mesh = &node->mesh_;

        Scalar length = widget_->le_edge_length->text().toFloat();
        int iters     = widget_->sb_uniform_iterations->value();
        bool project  = widget_->cb_uniform_project->isChecked();

        if (length)
        {
            utility::Stop_watch timer;
            timer.start();

            uniform_remeshing(*mesh, length, iters, project);

            timer.stop();
            LOG(Log_info) << timer << std::endl;

        }

        analyze_mesh();

        node->update_mesh();
        QApplication::postEvent(main_window_, new Geometry_changed_event());

        QApplication::restoreOverrideCursor();
    }
}


//-----------------------------------------------------------------------------


void
Remeshing_plugin::
slot_adaptive_remeshing()
{
    Surface_mesh_node* node = selected_node();

    if (node)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        Surface_mesh* mesh = &node->mesh_;

        Scalar min_length = widget_->le_min_length->text().toFloat();
        Scalar max_length = widget_->le_max_length->text().toFloat();
        Scalar error      = widget_->le_error->text().toFloat();
        int iters         = widget_->sb_adaptive_iterations->value();
        bool relative     = widget_->cb_relative_lengths->isChecked();
        bool project      = widget_->cb_adaptive_project->isChecked();
        

        if (relative)
        {
            Scalar bbsize = node->bbox().size();
            min_length *= bbsize;
            max_length *= bbsize;
            error      *= bbsize;
        }


        utility::Stop_watch timer;
        timer.start();

        adaptive_remeshing(*mesh, min_length, max_length, error, iters, project);

        timer.stop();
        LOG(Log_info) << timer << std::endl;

        analyze_mesh();

        node->update_mesh();
        QApplication::postEvent(main_window_, new Geometry_changed_event());

        QApplication::restoreOverrideCursor();
    }
}


//-----------------------------------------------------------------------------


void
Remeshing_plugin::
slot_mean_edge_length()
{
    Surface_mesh* mesh = selected_mesh();

    if (mesh)
    {
        Scalar target(0);
        Surface_mesh::Edge_iterator eit = mesh->edges_begin();
        Surface_mesh::Edge_iterator end = mesh->edges_end();

        for (; eit != end; ++eit)
        {
            target += mesh->edge_length(*eit);
        }

        target /= mesh->n_edges();

        QString s; s.setNum(target);

        widget_->le_edge_length->setText(s);
    }
}
//----------------------------------------------------------------------------
void 
Remeshing_plugin::
slot_calculate_line()
{
	Surface_mesh_node* node = selected_node();
	Surface_mesh& mesh = node->mesh_;
	if (node == nullptr) return;
	assert(&mesh);
	auto ldeleted = mesh.get_line_property<bool>("l:deleted");
	int ridge_num(0), ravine_num(0);
	for (auto l : mesh.lines())
	{
		if (!ldeleted[l])
		{
			mesh.is_ridge(l) ? ridge_num++ : ravine_num++;
		}
	}
	QString s;
	s.setNum(ridge_num);
	widget_->ridge_num->setText(s);
	s.setNum(ravine_num);
	widget_->ravine_num->setText(s);
	node->update_mesh();
}
//-----------------------------------------------------------------------------
void 
Remeshing_plugin::
slot_select_ridge()
{
	QStringList s1;
	std::vector<int> vecid;
	Surface_mesh_node* node = selected_node();
	if (node == nullptr) return;
	Surface_mesh& mesh = node->mesh_;
	node->is_visual = true;
	auto lvisual = mesh.get_line_property<bool>("l:is visual");
	auto ldeleted = mesh.get_line_property<bool>("l:deleted");
	for (auto l : mesh.lines())
	{
		if (!ldeleted[l] && mesh.is_ridge(l))
		lvisual[l] = false;
	}
	bool show_all = widget_->show_all_ridges->isChecked();
	if (show_all)
	{
		for (auto l:mesh.lines())
			if (!ldeleted[l] && mesh.is_ridge(l))
				lvisual[l] = true;
	}
	else
	{
		QString s;
		QStringList id;
		s = widget_->ridge_id->text();
		id = s.split(",");
		QList<QString>::Iterator it = id.begin(), itend = id.end();
		int i = 0;
		for (; it != itend; it++, i++) {
			if(it->toInt() != -1)
			vecid.push_back(it->toInt());
		}
		std::vector<int>::iterator iter;
		for (iter = vecid.begin(); iter != vecid.end(); iter++)
		{
			int k = 1;
			for (auto l : mesh.lines())
			{
				if (!ldeleted[l])
				{
					if (mesh.is_ridge(l))
					{
						if (k == *iter)
						{
							lvisual[l] = true;
							s1.append(QString::number(l.idx(), 10));
							break;
						}
						k++;
					}
				}
			}
		}
	}
	widget_->origin1->setText(s1.join(","));
	node->update_mesh();
	QApplication::postEvent(main_window_, new Geometry_changed_event());
	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
void
Remeshing_plugin::
slot_select_ravine()
{
	QStringList s1;
	std::vector<int> vecid;
	Surface_mesh_node* node = selected_node();
	if (node == nullptr) return;
	Surface_mesh& mesh = node->mesh_;
	node->is_visual = true;
	auto lvisual = mesh.get_line_property<bool>("l:is visual");
	auto ldeleted = mesh.get_line_property<bool>("l:deleted");
	for (auto l : mesh.lines())
	{
		if (!ldeleted[l] && !mesh.is_ridge(l))
		lvisual[l] = false;
	}
	bool show_all = widget_->show_all_ravines->isChecked();
	if (show_all)
	{
		for (auto l : mesh.lines())
		{
			if (!ldeleted[l] && !mesh.is_ridge(l))
			{
				lvisual[l] = true;
			}
		}
	}
	else
	{
		QString s;
		QStringList id;
		s = widget_->ravine_id->text();
			id = s.split(",");
			QList<QString>::Iterator it = id.begin(), itend = id.end();
			int i = 0;
			for (; it != itend; it++, i++) {
				if (it->toInt() != -1)
					vecid.push_back(it->toInt());
			}
			std::vector<int>::iterator iter;
			for (iter = vecid.begin(); iter != vecid.end(); iter++)
			{
				int k = 1;
				for (auto l : mesh.lines())
				{
					if (!ldeleted[l])
					{
						if (!mesh.is_ridge(l))
						{
							if (k == *iter)
							{
								lvisual[l] = true;
								s1.append(QString::number(l.idx(), 10));
								break;
							}
							k++;
						}
					}
				}
			}
	}
	widget_->origin2->setText(s1.join(","));
	node->update_mesh();
	QApplication::postEvent(main_window_, new Geometry_changed_event());
	QApplication::restoreOverrideCursor();
}
//------------------------------------------------------------------------------
void
Remeshing_plugin::
analyze_mesh()
{
    Surface_mesh* mesh = selected_mesh();
    assert(mesh);


    // compute min, max, mean-min triangle angle
    Scalar amin(M_PI), amax(0.0), aminmean(0.0), a0, a1, a2;

    // loop over all triangles
    Surface_mesh::Face_iterator fit, fend=mesh->faces_end();
    Surface_mesh::Vertex_around_face_circulator  vfit;
    Point p0, p1, p2;

    for (fit=mesh->faces_begin(); fit!=fend; ++fit)
    {
        vfit = mesh->vertices(*fit);
        p0 = mesh->position(*vfit);
        p1 = mesh->position(*++vfit);
        p2 = mesh->position(*++vfit);

        a0 = acos(std::min(Scalar(1.0), std::max(Scalar(-1.0), dot(normalize(p1-p0), normalize(p2-p0)))));
        a1 = acos(std::min(Scalar(1.0), std::max(Scalar(-1.0), dot(normalize(p0-p1), normalize(p2-p1)))));
        a2 = acos(std::min(Scalar(1.0), std::max(Scalar(-1.0), dot(normalize(p0-p2), normalize(p1-p2)))));

        if (a0 < amin) amin = a0; else if (a0 > amax) amax = a0;
        if (a1 < amin) amin = a1; else if (a1 > amax) amax = a1;
        if (a2 < amin) amin = a2; else if (a2 > amax) amax = a2;

        aminmean += std::min(a0, std::min(a1, a2));
    }

    amin *= 180.0 / M_PI;
    amax *= 180.0 / M_PI;
    aminmean *= 180.0 / M_PI / (Scalar) mesh->n_faces();

    LOG(Log_info) << "triangle angles [" << amin << ", " << amax << "], mean-min  = " << aminmean << std::endl;
}


//=============================================================================
}
}
//=============================================================================
