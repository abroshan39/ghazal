/*
    [Ghazal: The library of persian poetry]
    Publisher: Rosybit
    Url: http://www.rosybit.com
    GitHub: https://github.com/abroshan39/ghazal
    Author: Aboutaleb Roshan [ab.roshan39@gmail.com]
    License: MIT License
*/

#include "event_functions.h"

KeyPress::KeyPress(QObject *parent) :
    QObject(parent)
{
    if(parent)
        parent->installEventFilter(this);
}

bool KeyPress::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);  // QKeyEvent *keyEvent = (QKeyEvent *)event;
        emit keyPressed(object, keyEvent);
    }

    return false;  // return QObject::eventFilter(object, event);
}

FocusWatcher::FocusWatcher(QObject *parent) :
    QObject(parent)
{
    if(parent)
        parent->installEventFilter(this);
}

bool FocusWatcher::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut)
       emit focusChanged(object, event);

    return false;  // return QObject::eventFilter(object, event);
}

ZWNJPress::ZWNJPress(QObject *parent) :
    QObject(parent)
{
    if(parent)
        parent->installEventFilter(this);
}

bool ZWNJPress::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        pressedKeys += ((QKeyEvent *)event)->key();
        if(pressedKeys.contains(Qt::Key_Shift) && pressedKeys.contains(Qt::Key_Space))
        {
            emit zwnjPressed(object, Qt::ShiftModifier);
            return true;
        }
        else if(pressedKeys.contains(Qt::Key_Control) && pressedKeys.contains(Qt::Key_Space))
        {
            emit zwnjPressed(object, Qt::ControlModifier);
            return true;
        }
    }
    else if(event->type() == QEvent::KeyRelease)
    {
        pressedKeys -= ((QKeyEvent *)event)->key();
    }

    return false;  // return QObject::eventFilter(object, event);
}
