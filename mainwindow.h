#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./ui_mainwindow.h"
#include "transparentdialog.h"
#include "views_handler.h"
#include <QTimer>
#include <QStyleFactory>

#define DOUBLE_SHIFT_TIMER_MS 200
#define LEFT_SHIFT_KEY 42
#define RIGHT_SHIFT_KEY 54

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

    QTimer *rightShiftTimer;
    QTimer *leftShiftTimer;
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
    void startFileSearch();
};
#endif // MAINWINDOW_H
