#include <QApplication>
#include <QKeyEvent>

#include "appeventfilter.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QIcon icon(QStringLiteral(":/icons/mainIcon"));
    a.setWindowIcon(icon);

    AppEventFilter filter;
    a.installEventFilter(&filter);

    MainWindow w;

    QObject::connect(&filter, &AppEventFilter::openRecentFiles, &w,
                     &MainWindow::recentFilesHandler);

    // When the application loses focus, release any pending Alt state in
    // the event filter. Using applicationStateChanged is more accurate than
    // QApplication::focusChanged, which also fires for intra-window widget
    // focus changes and would incorrectly synthesise an Alt release.
    //
    // The event is created on the stack so sendEvent() does not leak it.
    QObject::connect(&a, &QGuiApplication::applicationStateChanged,
                     [&a](Qt::ApplicationState state)
                     {
                         if (state != Qt::ApplicationInactive)
                             return;

                         QKeyEvent event(QEvent::KeyRelease, Qt::Key_Alt,
                                         Qt::AltModifier);
                         QCoreApplication::sendEvent(&a, &event);
                     });

    w.show();
    return a.exec();
}
