#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
};
#endif // MAINWINDOW_H
