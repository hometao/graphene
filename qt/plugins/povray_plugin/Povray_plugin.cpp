#include "Povray_plugin.h"


// ---------------------------------------------------------------------------


namespace graphene {
namespace qt {


// ---------------------------------------------------------------------------


Povray_plugin::Povray_plugin() : proc_(0),ui_(0)
{
    name_ = "Povray_plugin";
}


// ---------------------------------------------------------------------------


Povray_plugin::~Povray_plugin()
{
}


// ---------------------------------------------------------------------------


void Povray_plugin::init(Main_window* w)
{
    main_window_ = w;
    QToolBox* toolbox_ = main_window_->toolbox_;

    // create ui
    ui_ = new Povray_plugin_widget(main_window_->qglviewer_);
    ui_->setupUi();


    // react on actions from the ui
    // start transformation of the scene graph, if button is pressed
    connect(ui_->pb_execute,
            SIGNAL(clicked()),
            this,
            SLOT(slot_render_scene())
           );
    // check height dimension, if width is changed
    connect(ui_->sb_width,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(slot_check_height(int))
           );
    // check width dimension, if height is changed
    connect(ui_->sb_height,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(slot_check_width(int))
           );
    // check height to keep aspect ratio, if corresponding check box
    // is toggled
    connect(ui_->cb_keep_aspect_ratio,
            SIGNAL(stateChanged(int)),
            this,
            SLOT(slot_update_aspect_ratio())
           );

    // add ui to toolbar_
    toolbox_->addItem(ui_,tr("POV-Ray"));
    toolbox_->setItemIcon(
            toolbox_->indexOf(ui_),QIcon(":/graphene/icons/povray.png"));
    ui_->show();


//    connect(main_window_->qglviewer_,
//            SIGNAL(signal_mouse_mode_changed(Mouse_mode)),
//            this,
//            SLOT(slot_mouse_mode_changed(Mouse_mode))
//           );
}


// ---------------------------------------------------------------------------


void
Povray_plugin::
add_eye_coordinates_elements_to_scene_graph()

{
    // get some parameters from the qglviewer_
    float near = main_window_->qglviewer_->near();
    float fovy = main_window_->qglviewer_->fovy();

    unsigned int width = ui_->sb_width->value();
    unsigned int height = ui_->sb_height->value();

    // get modelview matrix from qglviewer_
    double mv_matrix[16];
    main_window_->qglviewer_->modelview(mv_matrix);

    // compute aspect ratio for the user given width and height
    float aspect = (float) width / (float) height;


    // Light in the qglviewer_ has this color
    Vec4f light_color(1,1,1,1);

    // create nodes to append to scene graph
    Base_node* new_root = new Base_node();
    Camera_node* camera = new Camera_node(fovy, aspect,near);
    // Move the hardcoded lights, along their position vector
    // with the factor r, out of the scene.
    // With this the whole scene is illuminated.
    float r = 1000*main_window_->qglviewer_->scene_radius();
    Light_node* default_light0 = new Light_node(
        r*Vec3f(0.1,0.1,-0.02),
        light_color);
    Light_node* default_light1 = new Light_node(
        r*Vec3f(-0.1,0.1,-0.02),
        light_color);
    Light_node* default_light2 = new Light_node(
        r*Vec3f(0,0,0.1),
        light_color);
    Transformation_node* modelview = new Transformation_node(mv_matrix);

    // create a helper branch
    new_root->append(camera);
    camera->append(default_light0);
    default_light0->append(default_light1);
    default_light1->append(default_light2);
    default_light2->append(modelview);

    // current scene graph
    Scene_graph* graph = main_window_->scene_graph_;

    // prefix the old scene graph with new root, the camera,
    // lights and the modelview transformation
    old_root_ = graph->root_;
    graph->root_ = new_root;
    modelview->append(old_root_);


    // remember nodes for later deletion
    hardcoded_nodes_.push_back(new_root);
    hardcoded_nodes_.push_back(camera);
    hardcoded_nodes_.push_back(default_light0);
    hardcoded_nodes_.push_back(default_light1);
    hardcoded_nodes_.push_back(default_light2);
    hardcoded_nodes_.push_back(modelview);

}


// ---------------------------------------------------------------------------


void
Povray_plugin::remove_eye_coordinates_elements_from_scene_graph()
{
    // remove previously added nodes from the scene graph
    for (int i = hardcoded_nodes_.size() - 1; i >= 1; --i)
    {
        hardcoded_nodes_[i]->remove();
    }

    // delete new_root node, which cannot be removed with remove()
    delete hardcoded_nodes_[0];

    hardcoded_nodes_.clear();

    // make the old root node to the scene graph root
    old_root_->parent_ = 0;
    main_window_->scene_graph_->root_ = old_root_;
}


// ---------------------------------------------------------------------------


void Povray_plugin::slot_render_scene()
{
    // Assumption:
    //
    // All nodes in the scene graph are specified in world-coordinates.
    // Objects in eye-coordinates like camera and lights are now added to the
    // front of the scene graph. After that, a transformation node is added,
    // that represents the modelview-matrix. With that, effectively the old
    // scene graph is transformed from world- into eye-coordinates.
    //

    // add camera and lights, that are already in eye-coordinates and the
    // modelview-transformation node to the front of the scene graph
    add_eye_coordinates_elements_to_scene_graph();


    // get name prefix for Povray files (.{ini,pov})
    povray_file_name_prefix_ = ui_->le_filename->text();
    LOG(Log_info) << "Creating "
              << povray_file_name_prefix_.toStdString().data()
              << ".{pov,ini} files.\n";


    // Povray options collected in one variable
    Scene_graph_converter::Povray_options options = {
        povray_file_name_prefix_.toStdString().data(),
        ui_->sb_width->value(),
        ui_->sb_height->value(),
        ui_->sb_quality->value(),
        ui_->sb_sampling_method->value(),
        ui_->sb_depth->value(),
        ui_->dsb_threshold->value(),
        ui_->dsb_jitter->value(),
        ui_->cb_antialias->isChecked(),
        ui_->cb_verbose->isChecked(),
        ui_->cb_display->isChecked()
    };


    // create scene graph converter
    Scene_graph_converter converter(
        main_window_->scene_graph_,
        options);


    // begin conversion from scene graph to Povray file format .{ini,pov}
    bool success = converter.start();

    // restore scene graph
    remove_eye_coordinates_elements_from_scene_graph();


    // start Povray, in case user wants also the rendered scene
    if (success && !ui_->cb_no_rendering->isChecked())
    {

        // create external process for Povray rendering
        proc_ = new QProcess(this);

        // make sure, rendered scene is shown, after Povray is finished.
        connect(proc_,
                SIGNAL(finished(int, QProcess::ExitStatus)),
                this,
                SLOT(slot_rendering_finished()));


        // start Povray
        proc_->start("povray", QStringList() << povray_file_name_prefix_);


        // process started?
        if (proc_->waitForStarted())
        {
            LOG(Log_info) << "Scene rendering started.\n";
        }
        else
        {
            std::cerr << "Could not start scene rendering process!\n";
        }
    }


}


// ---------------------------------------------------------------------------


void Povray_plugin::slot_rendering_finished()
{
    // rendering successful?
    if (proc_->exitStatus() == QProcess::NormalExit)
    {
        LOG(Log_info) << "Scene rendering finished.\n";

        // image for loading the rendered scene
        QImage image;

        // do nothing, if image cant be loaded
        if (!image.load(povray_file_name_prefix_))
        {
            std::cerr << "Cannot show rendered scene!" << std::endl;
            return;
        }

        // display rendered scene in a QLabel
        QLabel* rendered_scene_label_ = new QLabel(
                tr("Rendered scene from ") + povray_file_name_prefix_);
        rendered_scene_label_->setText(povray_file_name_prefix_);
        QPixmap pixmap = QPixmap::fromImage(image);
        rendered_scene_label_->setPixmap( pixmap );
        rendered_scene_label_->resize(image.size());
        rendered_scene_label_->show();
        rendered_scene_label_->raise();
    }
    else
    {
        std::cerr << "Scene rendering failed!";
    }

}


// ---------------------------------------------------------------------------


float
Povray_plugin::
current_aspect_ratio()
{
        unsigned int height = main_window_->qglviewer_->height();
        unsigned int width = main_window_->qglviewer_->width();
        return (float) width / (float) height;
}


// ---------------------------------------------------------------------------


void
Povray_plugin::
slot_check_height(int value)
{
    // keep qglviewer aspect ratio for picture?
    if (ui_->cb_keep_aspect_ratio->checkState() == Qt::Checked)
    {
        // get qglviewer aspect ratio
        float aspect_ratio = current_aspect_ratio();

        // compute new height from new width value
        int new_height = value / aspect_ratio + 0.5;

        // Check, if height update is necessary, to break possible cycle
        // due to rounding errors.
        if (abs(new_height - ui_->sb_height->value()) > (1.0f + 1.0f / aspect_ratio))
        {
            ui_->sb_height->setValue(new_height);
        }
    }
}


// ---------------------------------------------------------------------------


void
Povray_plugin::
slot_check_width(int value)
{
    // keep qglviewer aspect ratio for picture?
    if (ui_->cb_keep_aspect_ratio->checkState() == Qt::Checked)
    {
        // get qglviewer aspect ratio
        float aspect_ratio = current_aspect_ratio();

        // compute new width from new height value
        int new_width = value * aspect_ratio + 0.5f;

        // Check, if width update is necessary, to break possible cycle
        // due to rounding errors.
        if (abs(new_width - ui_->sb_width->value()) > (1.0f + 1.0f / aspect_ratio))
        {
            ui_->sb_width->setValue(new_width);
        }
    }
}


// ---------------------------------------------------------------------------


void
Povray_plugin::
slot_update_aspect_ratio()
{
    // pretend the output width was changed to start adjusting the output
    // height to keep the aspect ratio
    int width = ui_->sb_width->value();
    slot_check_height(width);
}


// ---------------------------------------------------------------------------


}
}
