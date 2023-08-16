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

        if(Qt::AltModifier == keyEvent->modifiers()){
            if(!altPressed){
                altPressed = true;
                emit KeyPressAlt(true);
            }

            switch(keyEvent->key()){
            case Qt::Key_1:
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_Slash: emit viewChosen((Qt::Key)keyEvent->key());break;
            }
            return true;
        }

        if(Qt::Key_Tab == keyEvent->key() && Qt::ControlModifier == keyEvent->modifiers()){
            emit openRecentFiles(true);
        }
    }

    if(QEvent::KeyRelease == event->type()){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if(Qt::Key_Alt == keyEvent->key()){
            altPressed = false;
            emit KeyPressAlt(false);
            return false;
        }

        if(Qt::Key_Control == keyEvent->key()){
            emit openRecentFiles(false);
        }

    }

    return false;
}

