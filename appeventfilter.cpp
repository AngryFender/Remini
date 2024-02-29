#include "appeventfilter.h"

AppEventFilter::AppEventFilter(QObject *parent)
    : QObject{parent}
{
    altPressed = false;
}

bool AppEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if(QEvent::KeyPress == event->type()){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if(Qt::Key_Tab == keyEvent->key() && Qt::ControlModifier == keyEvent->modifiers()){
            emit openRecentFiles(true);
        }
    }

    if(QEvent::KeyRelease == event->type()){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if(Qt::Key_Control == keyEvent->key()){
            emit openRecentFiles(false);
        }

    }

    return false;
}

