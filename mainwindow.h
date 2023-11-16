#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./ui_mainwindow.h"
#include "transparentdialog.h"
#include "views_handler.h"
#include <QTimer>
#include <QStyleFactory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyReleaseEvent(QKeyEvent *event)override;

public slots:
    void KeyPressAltHandler(bool press);       //true for press, false for release
    void viewChosenHandler(Qt::Key key);
    void recentFilesHandler(bool show);
private slots:
    void shiftTimerHandle();

private:
    enum ThemeState{
        lightThemeState,
        darkThemeState,
    };

    QTimer *shiftTimer;
    Ui::MainWindow *ui;
    QString themeContents;
    ThemeState themeState;
    QSharedPointer<ViewsHandler> view_handler;
    QStyle *lightThemeStyle;
    QStyle *darkThemeStyle;

    TransparentDialog * fileSearchBox;
    TransparentDialog * folderTreeBox;
    TransparentDialog * mkEditorBox;
    void setup_views(QWidget *parent, Ui::MainWindow &ui);

signals:
    void openRecentFilesDialog(bool show);
    void startSearchAll();
};
#endif // MAINWINDOW_H
