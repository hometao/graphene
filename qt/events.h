//=============================================================================
// Copyright (C) 2013 by Graphics & Geometry Group, Bielefeld University
//=============================================================================

#ifndef GRAPHENE_EVENTS_H
#define GRAPHENE_EVENTS_H


//== INCLUDES =================================================================


#include <QEvent>


//== NAMESPACES ===============================================================


namespace graphene {
namespace qt {


//== CLASS DEFINITION =========================================================


const QEvent::Type GEOMETRY_CHANGED = static_cast<QEvent::Type>(QEvent::User + 1);

class Geometry_changed_event : public QEvent
{
public:
    Geometry_changed_event() : QEvent(GEOMETRY_CHANGED) {};

};


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_EVENTS_H
//=============================================================================
