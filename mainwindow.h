#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./ui_mainwindow.h"
#include "views_handler.h"
#include "theme.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event)override;

private:
    enum ThemeState{
        lightThemeState,
        darkThemeState,
    };

    Ui::MainWindow *ui;
    QString themeContents;
    ThemeState themeState;
    QSharedPointer<ViewsHandler> view_handler;

    void setup_views(QWidget *parent, Ui::MainWindow &ui);
};
#endif // MAINWINDOW_H
