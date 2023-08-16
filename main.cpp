#include "mainwindow.h"

#include <QApplication>
#include <QKeyEvent>
#include "appeventfilter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon icon(":/icons/application.png");
    a.setWindowIcon(icon);
    AppEventFilter filter;
    a.installEventFilter(&filter);
    MainWindow w;
    QObject::connect(&filter,&AppEventFilter::KeyPressAlt,
                     &w,&MainWindow::KeyPressAltHandler);

    QObject::connect(&filter,&AppEventFilter::viewChosen,
                     &w,&MainWindow::viewChosenHandler);

    QObject::connect(&filter,&AppEventFilter::openRecentFiles,
                     &w,&MainWindow::openRecentFilesDialog);

    QObject::connect(&a, &QApplication::focusChanged, [&](QWidget *old, QWidget *now) {
        QKeyEvent *event = new QKeyEvent(QKeyEvent::KeyRelease, Qt::Key_Alt, Qt::AltModifier);
        a.sendEvent(&a,event);
    });



    w.show();
    return a.exec();
}
