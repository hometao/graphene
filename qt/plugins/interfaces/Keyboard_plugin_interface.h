//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================
#ifndef KEYBOARD_PLUGIN_INTERFACE_H
#define KEYBOARD_PLUGIN_INTERFACE_H
//=============================================================================


#include <QKeyEvent>


//=============================================================================


namespace graphene {
namespace qt {


//=============================================================================


/// \addtogroup qt
/// @{

/// Interface for plugins to retrieve keyboard events.
///
/// A plugin that needs to get all keyboard events from the core
/// application needs implement this interface.
class Keyboard_plugin_interface
{

public:

    // Destructor.
    virtual ~Keyboard_plugin_interface() {};


public slots:

    /// Get keyboard events from the core application.
    virtual void slot_key_press_event(QKeyEvent* ) {};


};


//=============================================================================
/// @}
//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
Q_DECLARE_INTERFACE(graphene::qt::Keyboard_plugin_interface,
                    "de.uni-bielefeld.graphics.graphene.Keyboard_plugin_interface/1.0")
//=============================================================================
#endif // KEYBOARD_PLUGIN_INTERFACE_H
//=============================================================================

