#include "mainwindow.h"

#include <QApplication>
#include "appeventfilter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppEventFilter filter;
    a.installEventFilter(&filter);
    MainWindow w;
    QObject::connect(&filter,&AppEventFilter::KeyPressAlt,
                     &w,&MainWindow::KeyPressAltHandler);

    QObject::connect(&filter,&AppEventFilter::viewChosen,
                     &w,&MainWindow::viewChosenHandler);

    w.show();
    return a.exec();
}
