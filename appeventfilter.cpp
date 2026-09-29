#include "appeventfilter.h"

AppEventFilter::AppEventFilter(QObject *parent) : QObject{parent}
{
    altPressed = false;
}

bool AppEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if (QEvent::KeyPress == event->type())
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        // Use a bitwise test instead of strict equality.
        // On X11 with NumLock enabled, modifiers() also contains
        // Qt::KeypadModifier, which would break "== Qt::ControlModifier".
        // The same can happen on some Wayland compositors.
        const bool ctrlHeld = (keyEvent->modifiers() & Qt::ControlModifier);

        if (Qt::Key_Tab == keyEvent->key() && ctrlHeld)
        {
            emit openRecentFiles(true);
        }
    }
    else if (QEvent::KeyRelease == event->type())
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        // Ctrl released -> close the popup.
        // Also close it when a release event arrives with Ctrl no longer
        // in the modifier mask (covers the case where the compositor
        // delivered the release of another key while Ctrl was released
        // outside the application window on Wayland).
        if (Qt::Key_Control == keyEvent->key() ||
            !(keyEvent->modifiers() & Qt::ControlModifier))
        {
            emit openRecentFiles(false);
        }
    }

    return false;
}
