//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================


#include <graphene/gl/gl_includes.h>
#include <graphene/qt/Main_window.h>
#include <graphene/macros.h>

#include <QSettings>


//=============================================================================


using namespace graphene;


//=============================================================================


int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(Main_window);

    QApplication app(argc, argv);
    app.setOrganizationDomain("graphics.uni-bielefeld.de");
    app.setOrganizationName("Bielefeld Graphics & Geometry Group");
    app.setApplicationName("graphene");
    app.setApplicationVersion("1.0");

    QSettings settings;
    settings.setValue("ApplicationVersion", app.applicationVersion());

    if (!QGLFormat::hasOpenGL())
    {
        std::cerr << "This system has no OpenGL support.\n";
        return -1;
    }

    //set locale
    setlocale(LC_ALL, "C");


    QGLFormat glf = QGLFormat::defaultFormat();
    glf.setVersion(4,3);
    glf.setProfile(QGLFormat::CoreProfile);
    glf.setSampleBuffers(false);
    glf.setSampleBuffers(true);
    glf.setSamples(4);
    QGLFormat::setDefaultFormat(glf);

    graphene::qt::Main_window w;
    w.raise();
    w.showMaximized();

    QObject::connect(&w, SIGNAL(signal_quit()), &app, SLOT(quit()));

#ifdef NDEBUG
    utility::Logger::log_level() = utility::Logger::from_string("info");
    LOG(Log_info) << "using info log level" << std::endl;
#else
    utility::Logger::log_level() = utility::Logger::from_string("debug");
    LOG(Log_debug) << "using debug log level" << std::endl;
#endif

    if (argc > 1)
        w.open(QString(argv[1]));

    return app.exec();
}

//=============================================================================
