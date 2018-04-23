//=============================================================================
// Copyright (C) 2013 by Graphics & Geometry Group, Bielefeld University
//=============================================================================


#ifndef GRAPHENE_PLUGIN_INTERFACE_H
#define GRAPHENE_PLUGIN_INTERFACE_H


//=============================================================================


#include <graphene/qt/Main_window.h>
#include <graphene/qt/events.h>

#include <QObject>
#include <QtPlugin>
#include <QString>


//=============================================================================


namespace graphene {
namespace qt {


//=============================================================================


/// The Basic plugin interface.
///
/// All plugins have to implement this interface by providing an
/// implementation of the initialization function.
class Plugin_interface
{
public:
    /// Destructor
    virtual ~Plugin_interface() {};

    /// Initialize the plugin.
    ///
    /// @param main_window a pointer to the Main_window
    virtual void init(Main_window* main_window)
    {
        main_window_ = main_window;
    };

    /// Return the name of the plugin.
    virtual QString name() { return name_; };

protected:
    /// The name of the plugin.
    QString name_;

    /// A pointer to the core application main window.
    Main_window* main_window_;
};


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
Q_DECLARE_INTERFACE(graphene::qt::Plugin_interface,
                    "de.uni-bielefeld.graphics.graphene.Plugin_interface/1.0")
//=============================================================================
#endif // GRAPHENE_PLUGIN_INTERFACE_H
//=============================================================================

