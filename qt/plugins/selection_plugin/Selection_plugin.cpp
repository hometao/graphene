//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================


#include <graphene/qt/plugins/selection_plugin/Selection_plugin.h>
#include <graphene/scene_graph/Object_node.h>
#include <graphene/gl/gl_includes.h>

#include <fstream>
#include <sstream>

#include <QSettings>
#include <QFileDialog>


//=============================================================================


namespace graphene {
namespace qt {


//=============================================================================


using graphene::scene_graph::Object_node;


//=============================================================================


Selection_plugin::
Selection_plugin()
{
    name_ = "Selection_plugin";
    lasso_ = NULL;
}


//-----------------------------------------------------------------------------


Selection_plugin::
~Selection_plugin()
{
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
init(Main_window *w)
{
    main_window_ = w;
    QToolBox *toolbox_ = main_window_->toolbox_;
    Selection_plugin_widget* ui = new Selection_plugin_widget();
    ui->setupUi();

    connect(ui->pb1, SIGNAL(clicked()), this, SLOT(slot_clear_selections()));
    connect(ui->pb2, SIGNAL(clicked()), this, SLOT(slot_invert_selections()));
    connect(ui->pb3, SIGNAL(clicked()), this, SLOT(slot_select_all()));
    connect(ui->pb4, SIGNAL(clicked()), this, SLOT(slot_select_isolated()));
    connect(ui->pb5, SIGNAL(clicked()), this, SLOT(slot_grow_selection()));
    connect(ui->pb6, SIGNAL(clicked()), this, SLOT(slot_load()));
    connect(ui->pb7, SIGNAL(clicked()), this, SLOT(slot_save()));

    toolbox_->addItem(ui,"Selections");
    toolbox_->setItemIcon(toolbox_->indexOf(ui),QIcon(":/graphene/icons/selection.png"));
    ui->show();

    connect(main_window_->qglviewer_,
            SIGNAL(signal_mouse_mode_changed(Mouse_mode)),
            this,
            SLOT(slot_mouse_mode_changed(Mouse_mode)));
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
slot_clear_selections()
{
    Object_node* node = main_window_->scene_graph_->selected_node();

    if (node)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        node->clear_selections();

        QApplication::restoreOverrideCursor();

        main_window_->qglviewer_->updateGL();
    }
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
slot_invert_selections()
{
    Object_node* node = main_window_->scene_graph_->selected_node();

    if (node)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        node->invert_selections();

        QApplication::restoreOverrideCursor();

        main_window_->qglviewer_->updateGL();
    }
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
slot_select_all()
{
    Object_node* node = main_window_->scene_graph_->selected_node();

    if (node)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        node->select_all();

        QApplication::restoreOverrideCursor();

        main_window_->qglviewer_->updateGL();
    }
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
slot_select_isolated()
{
    Object_node* node = main_window_->scene_graph_->selected_node();

    if (node)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        node->select_isolated();

        QApplication::restoreOverrideCursor();

        main_window_->qglviewer_->updateGL();
    }
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
slot_grow_selection()
{
    Object_node* node = main_window_->scene_graph_->selected_node();

    if (node)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        node->grow_selection();

        QApplication::restoreOverrideCursor();

        main_window_->qglviewer_->updateGL();
    }
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
slot_key_press_event(QKeyEvent* _event)
{
    switch (_event->key())
    {
        case Qt::Key_C:
        {
            Object_node* node = main_window_->scene_graph_->selected_node();

            if (node)
            {
                node->clear_selections();
                if (lasso_) lasso_->points_.clear();
                main_window_->qglviewer_->updateGL();
            }
            break;
        }
        case Qt::Key_A:
        {
            if (QApplication::keyboardModifiers() == Qt::ControlModifier)
                slot_select_all();
            if ((QApplication::keyboardModifiers() == Qt::ControlModifier) &&
                (QApplication::keyboardModifiers() == Qt::ShiftModifier))
                slot_invert_selections();
            break;
        }
        case Qt::Key_Delete:
        {
            Object_node* node = main_window_->scene_graph_->selected_node();

            if (node)
            {
                node->delete_selected();
                main_window_->qglviewer_->updateGL();
            }

            break;
        }
    }
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
slot_mouse_event(QMouseEvent* _event)
{
    if (mouse_mode_ == selection_mode)
    {
        Object_node* node = main_window_->scene_graph_->selected_node();

        if (node)
        {
            std::string s = node->get_selection_mode();

            if (s == "Lasso")
            {
                if (lasso_ == NULL && (_event->type() == QEvent::MouseButtonPress))
                {
                    lasso_ = new Lasso_selection_node(main_window_->scene_graph_->root_);
                }
                select_lasso(_event);
            }
            else if (s == "Constraints")
            {
                select_point(_event);
            }
        }

        main_window_->qglviewer_->updateGL();
    }
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
slot_mouse_mode_changed(Mouse_mode _m)
{
    mouse_mode_ = _m;

    if (_m != selection_mode)
    {
        Object_node* node = main_window_->scene_graph_->selected_node();

        if (node && lasso_)
        {
            lasso_->points_.clear();
        }
    }
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
slot_load()
{
    QStringList filenames;

    QSettings settings;
    QString dir(settings.value("recentDir").toString());

    QFileDialog dialog(main_window_,"Load Selection",dir);
    dialog.setNameFilters(QStringList("Selections (*.sel)"));
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (dialog.exec())
        filenames = dialog.selectedFiles();

    if (filenames.isEmpty())
        return;

    load_selection(filenames.first().toStdString());

    QFileInfo fileinfo(filenames.first());
    settings.setValue("recentDir", fileinfo.absoluteDir().absolutePath());
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
load_selection(const std::string& filename)
{
    std::vector<size_t> indices;

    std::ifstream ifs(filename.c_str());

    if (ifs.fail())
        return;

    while(!ifs.eof())
    {
        std::string line;
        getline(ifs,line);
        std::stringstream sstr(line);
        size_t idx;
        sstr >> idx;
        indices.push_back(idx);
    }

    Object_node* node = main_window_->scene_graph_->selected_node();

    if (node)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        node->set_selection(indices);
        QApplication::restoreOverrideCursor();

        main_window_->qglviewer_->updateGL();
    }
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
slot_save()
{
    QStringList filenames;

    QSettings settings;
    QString dir(settings.value("recentDir").toString());

    QFileDialog dialog(main_window_,"Save Selection",dir);
    dialog.setNameFilters(QStringList("Selections (*.sel)"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if (dialog.exec())
        filenames = dialog.selectedFiles();

    if (filenames.isEmpty())
        return;

    save_selection(filenames.first().toStdString());

    QFileInfo fileinfo(filenames.first());
    settings.setValue("recentDir", fileinfo.absoluteDir().absolutePath());
}


void
Selection_plugin::
save_selection(const std::string& filename)
{
    std::vector<size_t> indices;

    Object_node* node = main_window_->scene_graph_->selected_node();

    if (node)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        node->get_selection(indices);
        QApplication::restoreOverrideCursor();

        main_window_->qglviewer_->updateGL();
    }

    std::ofstream ofs(filename.c_str());

    if (ofs.fail())
        return;

    for (size_t i(0); i < indices.size(); i++)
    {
        ofs << indices[i] << std::endl;
    }

    ofs.close();
}


//-----------------------------------------------------------------------------


void
Selection_plugin::
select_point(QMouseEvent* _event)
{
    if (_event->type() == QEvent::MouseButtonDblClick)
    {
        Object_node* node = main_window_->scene_graph_->selected_node();

        if (node)
        {
            QPoint p = _event->pos();
            int x = p.x();
            int y = p.y();
            Vec3f v;

            if (pick(x,y,v))
            {
                node->select_point(v);
                main_window_->qglviewer_->updateGL();
            }
        }
    }

}


//-----------------------------------------------------------------------------


void
Selection_plugin::
select_lasso(QMouseEvent* _event)
{
    Object_node* node = main_window_->scene_graph_->selected_node();

    if (node && lasso_)
    {
        if (((_event->type() == QEvent::MouseMove) ||
             (_event->type() == QEvent::MouseButtonPress)) &&
            _event->buttons() == Qt::LeftButton)
        {
            QPoint p = _event->pos();
            lasso_->push_back(Vec3f(p.x(),p.y(),0));
        }

        if (_event->buttons() == Qt::MidButton)
        {
            lasso_->points_.clear();
            main_window_->qglviewer_->updateGL();
        }

        if (_event->type() == QEvent::MouseButtonDblClick)
        {
            std::vector<Vec2f> lasso;

            for (unsigned int i(0); i< lasso_->points_.size(); i++)
            {
                Vec3f p = lasso_->points_[i];
                lasso.push_back(Vec2f(p[0],main_window_->qglviewer_->height()-p[1]));
            }


            // collect indices of points in the lasso
            std::vector<size_t> selection;
            gl::GL_state*  gl_state = main_window_->qglviewer_->get_GL_state();

            if (QApplication::keyboardModifiers() == Qt::ControlModifier)
            {
                selection = node->select_lasso(lasso, gl_state, true);
            }
            else
            {
                selection = node->select_lasso(lasso, gl_state, false);
            }


            // clear selection using shift, set selected otherwise
            if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
            {
                node->clear_selection(selection);
            }
            else
            {
                node->set_selection(selection);
            }

            delete lasso_;
            lasso_ = NULL;

            main_window_->qglviewer_->updateGL();
        }
    }
}


//-----------------------------------------------------------------------------


bool
Selection_plugin::
pick(int x, int y, Vec3f& _p)
{
    GLint     viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // read depth buffer value at (x, y_new)
    float  z;
    int    y_new = viewport[3] - y; // in OpenGL y=0 is at the 'bottom'
    glReadPixels(x, y_new, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );


    if (z != 1.0f)
    {
        float xf = ((float)x - (float) viewport[0]) / ((float) viewport[2]) * 2.0f - 1.0f;
        float yf = ((float)y_new - (float) viewport[1]) / ((float) viewport[3]) * 2.0f - 1.0f;
        z = z * 2.0f - 1.0f;

        Vec3f v(xf, yf, z);

        Mat4f mvp_inv = inverse(main_window_->qglviewer_->get_GL_state()->modelviewproj_);

        Vec4f p = mvp_inv * Vec4f(v, 1.0f);

        p /= p[3];

        _p = Vec3f(p[0], p[1], p[2]);

        return true;
    }

    return false;
}


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
