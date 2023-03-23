#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include "./ui_mainwindow.h"
#include "views_handler.h"
#include <QTimer>
#include <QListView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
public:
    Dialog(QWidget*parent):QDialog(parent)
    {

        layout = new QVBoxLayout(this);
        textView = new QLineEdit(this);
//        textView->setStyleSheet("padding-left:40; padding-top:10; padding-bottom:10; padding-right:40");
        searchView = new QListView(this);

        layout->addWidget(textView);
        layout->addWidget(searchView);
    }
    ~Dialog(){
        delete layout;
        delete textView;
    }
private:
    QVBoxLayout *layout;
    QLineEdit * textView;
    QListView * searchView;

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event)override;
private slots:
    void shiftTimerHandle();

private:
    enum ThemeState{
        lightThemeState,
        darkThemeState,
    };

    QTimer *shiftTimer;
    Dialog * dialog;
    Ui::MainWindow *ui;
    QString themeContents;
    ThemeState themeState;
    QSharedPointer<ViewsHandler> view_handler;

    void setup_views(QWidget *parent, Ui::MainWindow &ui);
    void openSearch();
};
#endif // MAINWINDOW_H
