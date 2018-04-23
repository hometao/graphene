//=============================================================================

#ifndef GRAPHENE_QGL_VIEWER_WIDGET_H
#define GRAPHENE_QGL_VIEWER_WIDGET_H


//== INCLUDES =================================================================


#include <graphene/gl/gl_includes.h> // keep that before any openGL includes due to glew.h
#include <graphene/gl/GL_state.h>
#include <graphene/gl/Post_processing.h>
#include <graphene/geometry/Vector.h>
#include <graphene/types.h>
#include <graphene/scene_graph/Scene_graph.h>

#include <QtOpenGL/QGLWidget>
#include <QMenu>
#include <QLabel>

#include <string>
#include <vector>

#ifdef _WIN32
    #undef near
    #undef min
    #undef max
#endif


//== NAMESPACES ===============================================================


namespace graphene {
namespace qt {


//=============================================================================


/// \addtogroup qt
/// @{


//== CLASS DEFINITION =========================================================


enum Mouse_mode { move_mode, manipulation_mode, selection_mode };


/// A Qt-based OpenGL viewer widget.
class QGL_viewer_widget : public QGLWidget
{
    Q_OBJECT

public:

    typedef graphene::scene_graph::Scene_graph Scene_graph;
    typedef graphene::scene_graph::Object_node Object_node;
    typedef graphene::geometry::Bounding_box Bounding_box;


public:

    // Default constructor.
    QGL_viewer_widget(QWidget*          _parent = 0,
                       const QGLWidget* _share  = 0,
                       Qt::WindowFlags  _f      = 0 );

    // Destructor.
    virtual ~QGL_viewer_widget();

    /* Sets the center and size of the whole scene.
       The _center is used as fixpoint for rotations and for adjusting
       the camera/viewer (see view_all()). */
    void set_scene_pos( const Vec3f& _center, float _radius );

    /* view the whole scene: the eye point is moved far enough from the
       center so that the whole scene is visible. */
    void view_all();

    // fly (closer) to position p
    void fly_to(Vec3f p);

    // set scene graph
    void set_scene(Scene_graph *_scene_graph_);

    // Now this is beautiful
    void set_mouse_mode(Mouse_mode _m);
    Mouse_mode get_mouse_mode();
    void toggle_mouse_mode();

    void save_view(QString filename);
    void load_view(QString filename);

    void modelview(GLdouble _m[16]);
    float near();
    float fovy();
    float scene_radius() const;

    void screen_shot(const QString& filename);

    void set_fps_label(QLabel* label) { fps_label_ = label; };

    //twaltema: return gl_state pointer
    gl::GL_state* get_GL_state();

    Mouse_mode current_mouse_mode_;
    Mouse_mode previous_mouse_mode_;

public slots:
    void model_changed_triggered();
    void new_node_loaded_triggered();

    void set_view();
    void restore_view();


signals:
    void signalMouseEvent(QMouseEvent*);
    void signal_mouse_mode_changed(Mouse_mode _m);

    void signal_resized();

protected:

    // initialize OpenGL states (triggered by Qt)
    virtual void initializeGL();
    // draw the scene (triggered by Qt)
    virtual void paintGL();
    // handle resize events (triggered by Qt)
    virtual void resizeGL( int w, int h );


    // Qt key and mouse events
    virtual void mouseEvent(QMouseEvent* _event);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseDoubleClickEvent(QMouseEvent* _event);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void wheelEvent(QWheelEvent*);
    virtual void contextMenuEvent(QContextMenuEvent* _event);


    // updates projection matrix
    void update_projection_matrix();

    // update bounding box
    void update_bounding_box();

    // twaltema: translate the camera aka view of the scene
    void translate_camera(const Vec3f& _t);

    // rotate the scene (around its center) and update modelview matrix
    void rotate(const Vec3f& _axis, float _angle);

    // get 3D point corresponding to window position (x,y)
    bool pick(int x, int y, Vec3f& _p);


protected:

    Scene_graph* scene_graph_;

    QMenu* context_menu_;
    QMenu* draw_menu_;
    QMenu* post_processing_menu_;
    QMenu* selection_menu_;
    QMenu* manipulation_menu_;

    QLabel* fps_label_;


    // virtual trackball: map 2D screen point to 3D unit sphere
    bool map_to_sphere(const QPoint& _point, Vec3f& _result);


protected slots:
    void slot_draw_mode(QAction* a);
    void slot_post_processing_effect(QAction* a);
    void slot_selection_mode(QAction* a);
    void slot_manipulation_mode(QAction* a);


private:
    Bounding_box bbox_;
    Vec3f        center_;
    Vec3f        rotation_center_;
    float        radius_;
    float        near_;
    float        far_;
    float        fovy_;
    QPoint       last_point_2D_;
    Vec3f        last_point_3D_;
    bool         last_point_ok_;
    bool         last_click_dbl;
    float        fps_;

    // backup
    bool view_is_set_;
    Vec3f      center_bak_;
    Vec3f      rotation_center_bak_;
    float      radius_bak_;
    float      near_bak_;
    float      far_bak_;
    //twaltema save Mat4f matrices
    Mat4f      model_matrix_bak_;
    Mat4f      view_matrix_bak_;
    Mat4f      projection_matrix_bak_;
    //save camera, too
    gl::Camera camera_bak_;

    //twaltema: core profile GLState
    gl::GL_state* gl_state_;

    //twaltema: use simple Camera struct to save lookAt data
    gl::Camera camera_;

    bool initialized_;

    //twaltema: post_processing object
    gl::Post_processing post_processing_;
};


//=============================================================================
/// @}
//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_QGL_VIEWER_WIDGET_H
//=============================================================================
