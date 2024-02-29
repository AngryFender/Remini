#include "mainwindow.h"

#include <QApplication>
#include <QKeyEvent>
#include "appeventfilter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon icon(":/icons/mainIcon");
    a.setWindowIcon(icon);
    AppEventFilter filter;
    a.installEventFilter(&filter);
    MainWindow w;

    QObject::connect(&filter,&AppEventFilter::openRecentFiles,
                     &w,&MainWindow::recentFilesHandler);

    QObject::connect(&a, &QApplication::focusChanged, [&](QWidget *old, QWidget *now) {
        QKeyEvent *event = new QKeyEvent(QKeyEvent::KeyRelease, Qt::Key_Alt, Qt::AltModifier);
        a.sendEvent(&a,event);
    });



    w.show();
    return a.exec();
}
