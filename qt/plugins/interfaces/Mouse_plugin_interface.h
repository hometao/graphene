//=============================================================================
// Copyright (C) 2013 by Graphics & Geometry Group, Bielefeld University
//=============================================================================


#ifndef GRAPHENE_MOUSE_PLUGIN_INTERFACE_H
#define GRAPHENE_MOUSE_PLUGIN_INTERFACE_H


//=============================================================================


#include <QMouseEvent>


//=============================================================================

namespace graphene {
namespace qt {


//=============================================================================


/// Interface for plugins to retrieve mouse events.
///
/// A plugin that needs to get all mouse events from the core
/// application needs implement this interface.
class Mouse_plugin_interface
{

public:

    /// Destructor
    virtual ~Mouse_plugin_interface() {};

private slots:

    /// Get mouse events from the core application
    virtual void slot_mouse_event(QMouseEvent*) {};

};

//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
Q_DECLARE_INTERFACE(graphene::qt::Mouse_plugin_interface,
                    "de.uni-bielefeld.graphics.graphene.Mouse_plugin_interface/1.0")
//=============================================================================
#endif // GRAPHENE_MOUSE_PLUGIN_INTERFACE_H
//=============================================================================

