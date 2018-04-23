//== INCLUDES =================================================================

#include <graphene/qt/QGL_viewer_widget.h>
#include <graphene/scene_graph/Object_node.h>
#include <graphene/macros.h>

#include <QApplication>
#include <QMouseEvent>
#include <QTime>

#include <fstream>


//== NAMESPACES ===============================================================

using ::QMenu;

namespace graphene {
namespace qt {


//== IMPLEMENTATION ===========================================================


QGL_viewer_widget::
QGL_viewer_widget(QWidget*         _parent,
                  const QGLWidget* _share,
                  Qt::WindowFlags  _f )
    : QGLWidget(_parent, _share, _f), gl_state_(NULL), initialized_(false)
{
    gl_state_ = new gl::GL_state();

    current_mouse_mode_  = move_mode;
    previous_mouse_mode_ = selection_mode;
    fovy_                = 45.0;
    near_                = 0.1 * radius_;
    far_                 = 10.0 * radius_;
    view_is_set_         = false;

    context_menu_           = new QMenu();
    draw_menu_              = new QMenu("Visualization");
    post_processing_menu_   = new QMenu("Post-processing");
    selection_menu_         = new QMenu("Selection");
    manipulation_menu_      = new QMenu("Manipulation");
    fps_label_              = NULL;

    set_scene_pos(Vec3f(0.0, 0.0, 0.0), 1.0);
}


//----------------------------------------------------------------------------


QGL_viewer_widget::
~QGL_viewer_widget()
{
    delete context_menu_;
    delete draw_menu_;
    delete selection_menu_;
    delete manipulation_menu_;
    delete post_processing_menu_;
    if (gl_state_) delete gl_state_;//twaltema: delete gl_state
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
initializeGL()
{
    // initialize GLEW and OpenGL
    if (!initialized_)
    {
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        initialized_ = true;

        if (GLEW_OK != err)
        {
            LOG(Log_error) << "GLEW error: " << glewGetErrorString(err) << std::endl;
        }
        LOG(Log_info) << "GLEW   " << glewGetString(GLEW_VERSION) << std::endl;
        if (this->format().profile() == QGLFormat::CompatibilityProfile)
        {
            LOG(Log_info) << "OpenGL " << glGetString(GL_VERSION)
                          << " (Compatibility Profile)" << std::endl;
        }
        else if (this->format().profile() == QGLFormat::CoreProfile)
        {
            LOG(Log_info) << "OpenGL " << glGetString(GL_VERSION)
                          << " (Core Profile)" << std::endl;
        }
        LOG(Log_info) << "GLSL   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        LOG(Log_info) << glGetString(GL_RENDERER) << std::endl << std::endl;
    }


    // core profile GLState init
    gl_state_->init(QApplication::arguments()[0].toStdString().c_str());

    //init post-processing
    post_processing_.init(width(), height());


    // OpenGL state
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0f);
    glPointSize(1.0f);
    set_scene_pos(Vec3f(0.0, 0.0, 0.0), 1.0);


    // add lights
    std::vector<gl::Light> light_sources;
    Vec3f light_color(0.6);
    light_sources.push_back( gl::Light( Vec3f( 0.1f, -0.1f, -0.2f), light_color ));
    light_sources.push_back( gl::Light( Vec3f(-0.1f, -0.1f, -0.2f), light_color ));
    light_sources.push_back( gl::Light( Vec3f( 0.0f,  0.0f, -0.1f), light_color ));
    gl_state_->setup_lights(light_sources);
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
resizeGL( int _w, int _h )
{
    update_projection_matrix();
    glViewport(0, 0, _w, _h);
    post_processing_.resize(_w,_h);
    updateGL();
    emit(signal_resized());
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
paintGL()
{
    // measure fps
    QTime  timer;
    timer.start();


    // adjust near and far plane to tightly fit the scene
    GLdouble mv[16];
    modelview(mv);
    float z = -(mv[ 2]*center_[0] + mv[ 6]*center_[1] + mv[10]*center_[2] + mv[14]);
    near_ = std::max(z-radius_, 0.1f*radius_);
    far_  = near_ + 2.0f*radius_;
    update_projection_matrix();

    //multiply matrices
    gl_state_->update_matrices();

    // clear framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // traverse scene_graph
    if (scene_graph_ != 0)
    {
        if (post_processing_.get_current_effect() != gl::post_processing_effects::NONE)
        {
            post_processing_.bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            scene_graph_->draw(gl_state_);
            post_processing_.apply(gl_state_);
        }
        else
        {
            scene_graph_->draw(gl_state_);
        }

    }

    // measure fps
    glFinish();

    if (timer.elapsed() != 0)
    {
        fps_ = 1000.0f / (float)timer.elapsed();
        if (fps_label_)
            fps_label_->setText(QString("FPS: %1").arg((int)fps_,4,10,QChar('0')));
    }
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
update_projection_matrix()
{
    // stress test for molecule rendering
    near_ *= 0.1;
    far_  *= 10.0;

    gl_state_->proj_ = Mat4f::perspective(45.0f,
                                          (float) width() / (float) height(),
                                          near_,
                                          far_);

    gl_state_->near_ = near_;
    gl_state_->far_  = far_;
    gl_state_->fovy_ = 45.0;
    gl_state_->ar_   = (float)width() / (float)height();
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
view_all()
{
    //twaltema: set view
    camera_.center_ = center_;
    camera_.eye_    = center_ + Vec3f(0.0f, 0.0f, 2.5f*radius_);
    gl_state_->view_ = Mat4f::look_at(camera_.eye_, camera_.center_, camera_.up_);
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
fly_to(Vec3f p)
{
    // set rotation center
    rotation_center_ = p;

    Vec3f c = affine_transform(gl_state_->view_, p);

    // translate camera center (i.e. origin) half-way to center
    Vec3f t = 0.5f * c;

    // fast enough to animate in 0.25s?
    if (fps_ > 10)
    {
        int frames = (int) (0.1f * fps_);
        t /= (float) frames;
        makeCurrent();
        for (int i=0; i<frames; ++i)
        {
            //twaltema: translate camera
            translate_camera(t);
            paintGL();
            swapBuffers();
        }
    }

    // else: move in one step
    else
    {
        //twaltema: translate camera
        translate_camera(t);

        updateGL();
    }
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
set_scene_pos( const Vec3f& _cog, float _radius )
{
    center_ = rotation_center_ = _cog;
    radius_ = _radius;
    update_projection_matrix();
    view_all();
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
contextMenuEvent(QContextMenuEvent* )
{
    Object_node* node = scene_graph_->selected_node();

    if (node)
    {
        draw_menu_->clear();
        post_processing_menu_->clear();
        selection_menu_->clear();
        manipulation_menu_->clear();
        context_menu_->clear();

        const std::vector<std::string>& draw_modes = node->get_draw_modes_menu();
        const std::vector<std::string>& selection_modes = node->get_selection_modes_menu();
        const std::vector<std::string>& manipulation_modes = node->get_manipulation_modes_menu();
        const std::vector<std::string>& pp_effects = post_processing_.get_effects_menu();

        std::string draw_mode = node->get_draw_mode();
        std::string selection_mode = node->get_selection_mode();
        std::string manipulation_mode = node->get_manipulation_mode();
        std::string current_pp_effect = post_processing_.get_current_effect_as_string();

        QAction* a;

        for (unsigned int i(0); i < draw_modes.size(); i++)
        {
            a = draw_menu_->addAction(draw_modes[i].c_str());
            a->setCheckable(true);
            if (draw_modes[i] == draw_mode)
                a->setChecked(true);
        }

        draw_menu_->addSeparator();
        draw_menu_->addMenu(post_processing_menu_);

        for (unsigned int i(0); i < pp_effects.size(); i++)
        {
            a = post_processing_menu_->addAction(pp_effects[i].c_str());
            a->setCheckable(true);
            if (pp_effects[i] == current_pp_effect)
                a->setChecked(true);
        }


        for (unsigned int i(0); i < selection_modes.size(); i++)
        {
            a = selection_menu_->addAction(selection_modes[i].c_str());
            a->setCheckable(true);
            if (selection_modes[i] == selection_mode)
                a->setChecked(true);
        }

        for (unsigned int i(0); i < manipulation_modes.size(); i++)
        {
            a = manipulation_menu_->addAction(manipulation_modes[i].c_str());
            a->setCheckable(true);
            if (manipulation_modes[i] == manipulation_mode)
                a->setChecked(true);
        }

        context_menu_->addMenu(draw_menu_);
        context_menu_->addMenu(selection_menu_);
        context_menu_->addMenu(manipulation_menu_);

        connect(draw_menu_, SIGNAL(triggered(QAction*)),
                this, SLOT(slot_draw_mode(QAction*)));
        connect(post_processing_menu_, SIGNAL(triggered(QAction*)),
                this, SLOT(slot_post_processing_effect(QAction*)));
        connect(selection_menu_, SIGNAL(triggered(QAction*)),
                this, SLOT(slot_selection_mode(QAction*)));
        connect(manipulation_menu_, SIGNAL(triggered(QAction*)),
                this, SLOT(slot_manipulation_mode(QAction*)));

        context_menu_->popup(QCursor::pos());
    }
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
slot_draw_mode(QAction* a)
{
    Object_node* node = scene_graph_->selected_node();

    if (node)
    {
        QList<QAction*> actions = draw_menu_->actions();

        for (int i(0); i < actions.size(); i++)
        {
            actions[i]->setChecked(false);

            if (actions[i] == a)
            {
                a->setChecked(true);
                node->set_draw_mode(i);
            }
        }
    }

    updateGL();
}


//----------------------------------------------------------------------------

void
QGL_viewer_widget::
slot_post_processing_effect(QAction *a)
{
    QList<QAction*> actions = post_processing_menu_->actions();

    for (int i(0); i < actions.size(); i++)
    {
        actions[i]->setChecked(false);

        if (actions[i] == a)
        {
            a->setChecked(true);
            post_processing_.set_effect(a->text().toStdString());
        }
    }
    updateGL();
}

//----------------------------------------------------------------------------


void
QGL_viewer_widget::
slot_selection_mode(QAction* a)
{
    Object_node* node = scene_graph_->selected_node();

    if (node)
    {
        QList<QAction*> actions = selection_menu_->actions();

        for (int i(0); i < actions.size(); i++)
        {
            actions[i]->setChecked(false);

            if (actions[i] == a)
            {
                a->setChecked(true);
                node->set_selection_mode(i);
            }
        }
    }

    updateGL();
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
slot_manipulation_mode(QAction* a)
{
    Object_node* node = scene_graph_->selected_node();

    if (node)
    {
        QList<QAction*> actions = manipulation_menu_->actions();

        for (int i(0); i < actions.size(); i++)
        {
            actions[i]->setChecked(false);

            if (actions[i] == a)
            {
                a->setChecked(true);
                node->set_manipulation_mode(i);
            }
        }
    }

    updateGL();
}



//----------------------------------------------------------------------------


void
QGL_viewer_widget::
mouseEvent(QMouseEvent* _event)
{
    emit signalMouseEvent(_event);
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
mousePressEvent(QMouseEvent* _event)
{
    if (current_mouse_mode_ == selection_mode || current_mouse_mode_ == manipulation_mode)
    {
        last_point_2D_=_event->pos();
        emit signalMouseEvent(_event);
        updateGL();
    }

    // remember mouse position
    // map the 2D point onto the virtual unit sphere
    last_point_ok_ = map_to_sphere(last_point_2D_=_event->pos(),
                                   last_point_3D_);
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
mouseDoubleClickEvent(QMouseEvent* _event)
{
    switch (current_mouse_mode_)
    {
        case selection_mode:
        {
            emit signalMouseEvent(_event);
            break;
        }

        case move_mode:
        {
            QPoint p2d = _event->pos();
            Vec3f v;
            if (pick(p2d.x(),p2d.y(),v)) {
                fly_to(v);
            }
            break;
        }

        default:
            break;
    }
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
mouseMoveEvent( QMouseEvent* _event )
{
    // this function is called when the mouse is moved while some
    // mouse button is pressed

    QPoint newPoint2D = _event->pos();

    // Left button: rotate around its center stored in center_
    // Middle button: translate object parallel to image plane
    // Left & middle button: zoom in/out
    // Use the function translate() and rotate() defined below

    float  value_y;

    // map new mouse position onto the 3D unit sphere
    Vec3f  newPoint3D;
    bool   newPoint_hitSphere = map_to_sphere( newPoint2D, newPoint3D );

    // change in x and y since last mouse event
    float dx = newPoint2D.x() - last_point_2D_.x();
    float dy = newPoint2D.y() - last_point_2D_.y();

    if (current_mouse_mode_ == move_mode)
    {
        //-------------------------------------------------------- move in z direction

        if ( ((_event->buttons() & Qt::LeftButton) && (_event->buttons() & Qt::MidButton)) ||
             ((_event->buttons() & Qt::LeftButton) && (_event->modifiers() & Qt::ShiftModifier)) )
        {
            // amount of displacement depends on object radius and the y-change
            // relative to the window height
            value_y = radius_ * dy * 3.0 / (float)height();

            //twaltema: move camera instead of model
            translate_camera(Vec3f(0.0, 0.0, value_y));
        }

        //------------------------------------------------------ move in x,y direction

        else if ((_event->buttons() & Qt::MidButton) ||
                 ((_event->buttons() & Qt::LeftButton) && (_event->modifiers() & Qt::AltModifier)))
        {
            Vec4f v;
            v = (gl_state_->view_ * gl_state_->model_) * Vec4f(center_, 1.0f);

            if (v[2] > 0.0f)
            {
                v[2] *= -1.0f;
            }
            v[2] /= -v[3];

            // find scaling of dx and dy from window coordiantes to near plane
            // coordiantes and from there to camera coordinates at the object's depth
            float aspect = (float)width() / (float)height();
            float fnear  = 0.01 * radius_;
            float top    = tan(fovy_/2.0f*M_PI/180.f) * fnear;
            float right  = aspect*top;


            //twaltema: move camera instead of model
            translate_camera(Vec3f( -2.0 * dx / (float)width()  * right / fnear * v[2],
                                    2.0 * dy / (float)height() * top / fnear * v[2],
                                    0.0f));
        }

        //------------------------------------------------------ rotate around rotation_center_

        else if (_event->buttons() & Qt::LeftButton)
        {
            Vec3f axis(0,0,0);
            float angle(0);

            if (last_point_ok_ && newPoint_hitSphere)
            {
                // we have 2 points on the unit sphere (last_point_3D_ and newPoint3D)
                // and want to find a rotation to map one onto the other.

                // rotation axis is cross product of the two points,
                axis = cross(last_point_3D_,newPoint3D);

                // get angle from scalar product
                float cos_angle = dot(last_point_3D_,newPoint3D);
                if ( fabs(cos_angle) < 1.0 )
                {
                    angle = acos( cos_angle ) * 180.0 / M_PI;
                    angle *= 2.0; // factor 2 gives slightly nicer behavior
                }

                rotate( axis, angle );
            }
        }

    } // endif mouse_mode_ == move_mode
    else if (current_mouse_mode_ == selection_mode)
    {
        emit signalMouseEvent(_event);
    }
    else if (current_mouse_mode_ == manipulation_mode)
    {
        emit signalMouseEvent(_event);
    }

    // remember this point
    last_point_2D_ = newPoint2D;
    last_point_3D_ = newPoint3D;
    last_point_ok_ = newPoint_hitSphere;

    updateGL();
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
mouseReleaseEvent(QMouseEvent* event)
{
    last_point_ok_ = false;
    emit signalMouseEvent(event);
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
wheelEvent(QWheelEvent* _event)
{
    // compute amount of z-displacement from mouse wheel delta and object radius
    float d = (float)_event->delta() / 120 * 0.2 * radius_;

    //twaltema: translate camera
    translate_camera(Vec3f(0.0, 0.0, d));

    // redraw and mark event as accepted
    updateGL();
    _event->accept();
}


//----------------------------------------------------------------------------


void
QGL_viewer_widget::
translate_camera(const Vec3f &_t)
{
    //twaltema: translate camera and set new view
    camera_.center_ += _t;
    camera_.eye_ += _t;
    gl_state_->view_ = Mat4f::look_at(camera_.eye_, camera_.center_, camera_.up_);
}

//----------------------------------------------------------------------------


void
QGL_viewer_widget::
rotate( const Vec3f& _axis, float _angle )
{
    //twaltema: rotate model matrix
    Mat4f tmp = gl_state_->model_;
    gl_state_->model_ = Mat4f::identity();
    gl_state_->model_ = Mat4f::translate(rotation_center_) * Mat4f::rotate(_axis, _angle) * Mat4f::translate(-rotation_center_) * tmp;
}


//-----------------------------------------------------------------------------


bool
QGL_viewer_widget::
map_to_sphere( const QPoint& _v2D, Vec3f& _v3D )
{
    // test if mouse position is inside our window
    if ( (_v2D.x() >= 0) && (_v2D.x() <= width()) &&
         (_v2D.y() >= 0) && (_v2D.y() <= height()) )
    {
        // maps mouse (x,y) coords to [-0.5, 0.5]^2
        double x  = (double)(_v2D.x() - 0.5*width())  / (double)width();
        double y  = (double)(0.5*height() - _v2D.y()) / (double)height();

        // compute their sinus values
        double sinx         = sin(M_PI * x * 0.5);
        double siny         = sin(M_PI * y * 0.5);
        double sinx2siny2   = sinx * sinx + siny * siny;

        // take sine value for x and y coord compute z coord such that
        // x^2+y^2+z^2=1 (point on unit sphere)
        _v3D[0] = sinx;
        _v3D[1] = siny;
        _v3D[2] = sinx2siny2 < 1.0 ? sqrt(1.0 - sinx2siny2) : 0.0;

        return true;
    }
    else return false;
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
set_scene(Scene_graph *_scene_graph_)
{
    scene_graph_ = _scene_graph_;
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
update_bounding_box()
{
    bbox_ = scene_graph_->bbox();
    center_ = bbox_.center();
    radius_ = 0.5 * bbox_.size();
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
new_node_loaded_triggered()
{
    //twaltema: set model matrix to identity
    gl_state_->model_ = Mat4f::identity();

    update_bounding_box();
    set_scene_pos(center_, radius_);

    updateGL();
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
model_changed_triggered()
{
    update_bounding_box();
    updateGL();
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
set_mouse_mode(Mouse_mode _m)
{
    previous_mouse_mode_ = current_mouse_mode_;
    current_mouse_mode_ = _m;
    emit signal_mouse_mode_changed(_m);
}


//-----------------------------------------------------------------------------


Mouse_mode
QGL_viewer_widget::
get_mouse_mode()
{
    return current_mouse_mode_;
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
toggle_mouse_mode()
{
    Mouse_mode tmp = current_mouse_mode_;
    current_mouse_mode_ = previous_mouse_mode_;
    previous_mouse_mode_ = tmp;

    emit signal_mouse_mode_changed(current_mouse_mode_);
}


//-----------------------------------------------------------------------------


bool
QGL_viewer_widget::
pick(int x, int y, Vec3f& _p)
{
    GLint viewport[4];
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
        Mat4f proj_inv = inverse(gl_state_->proj_*gl_state_->view_);

        Vec4f p = proj_inv * Vec4f(v, 1.0f);

        p /= p[3];

        _p = Vec3f(p[0], p[1], p[2]);

        return true;
    }


    return false;
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
set_view()
{
    center_bak_ = center_;
    rotation_center_bak_ = rotation_center_;
    radius_bak_ = radius_;
    near_bak_ = near_;
    far_bak_ = far_;
    //twaltema: save matrices
    model_matrix_bak_ = gl_state_->model_;
    view_matrix_bak_ = gl_state_->view_;
    projection_matrix_bak_ = gl_state_->proj_;
    camera_bak_ = camera_;
    view_is_set_ = true;
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
restore_view()
{
    if (view_is_set_)
    {
        center_ = center_bak_;
        rotation_center_ = rotation_center_bak_;
        radius_ = radius_bak_;
        near_ = near_bak_;
        far_ = far_bak_;
        gl_state_->model_ = model_matrix_bak_;
        gl_state_->view_ = view_matrix_bak_;
        gl_state_->proj_ = projection_matrix_bak_;
        camera_ = camera_bak_;
        this->updateGL();
    }
    else
    {
        new_node_loaded_triggered();
    }
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
save_view(QString filename)
{
    std::ofstream ofs(filename.toStdString().data());
    ofs << center_ << std::endl;
    ofs << rotation_center_ << std::endl;
    ofs << radius_ << std::endl;
    ofs << near_ << std::endl;
    ofs << far_ << std::endl;

    //twaltema: save camera
    ofs << camera_.center_ << std::endl;
    ofs << camera_.eye_ << std::endl;
    ofs << camera_.up_ << std::endl;

    //twaltema: save matrices
    uint i,j;
    for (i=0; i<4; ++i)
    {
        for (j=0; j<4; ++j)
        {
            ofs << gl_state_->model_(i,j) << " ";
        }
    }
    ofs << std::endl;

    for (i=0; i<4; ++i)
    {
        for (j=0; j<4; ++j)
        {
            ofs << gl_state_->proj_(i,j) << " ";
        }
    }
    ofs << std::endl;
}


//-----------------------------------------------------------------------------


void
QGL_viewer_widget::
load_view(QString filename)
{
    std::ifstream ifs(filename.toStdString().data());
    ifs >> center_;
    ifs >> rotation_center_;
    ifs >> radius_;
    ifs >> near_;
    ifs >> far_;

    //twaltema: load camera and set view matrix
    ifs >> camera_.center_;
    ifs >> camera_.eye_;
    ifs >> camera_.up_;
    gl_state_->view_ = Mat4f::look_at(camera_.eye_, camera_.center_, camera_.up_);

    //twaltema: load matrices
    uint i,j;
    for (i=0; i<4; ++i)
    {
        for (j=0; j<4; ++j)
        {
            ifs >> gl_state_->model_(i,j);
        }
    }

    for (i=0; i<4; ++i)
    {
        for (j=0; j<4; ++j)
        {
            ifs >> gl_state_->proj_(i,j);
        }
    }


   this->updateGL();
}


//-----------------------------------------------------------------------------w


void
QGL_viewer_widget::
screen_shot(const QString& filename)
{
    grabFrameBuffer().save(filename,0,100);
}


//-----------------------------------------------------------------------------w


void
QGL_viewer_widget::
modelview(GLdouble _m[16])
{
    Mat4f modelview_matrix_ = gl_state_->view_ * gl_state_->model_;
    std::copy(modelview_matrix_.data(),modelview_matrix_.data()+16,_m);
}


//-----------------------------------------------------------------------------


float
QGL_viewer_widget::
near()
{
    return near_;
}


//-----------------------------------------------------------------------------


float
QGL_viewer_widget::
fovy()
{
    return fovy_;
}


//-----------------------------------------------------------------------------


float
QGL_viewer_widget::
scene_radius() const
{
    return radius_;
}


//-----------------------------------------------------------------------------


gl::GL_state*
QGL_viewer_widget::
get_GL_state()
{
    return gl_state_;
}


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
