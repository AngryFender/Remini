#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_views(parent, *ui);

    themeContents = darkTheme;
    themeState = darkThemeState;
    this->setStyleSheet(themeContents);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F12){
        if (themeState == darkThemeState){
            themeContents = lightTheme;
            themeState = lightThemeState;
        }else{
            themeContents = darkTheme;
            themeState = darkThemeState;
        }
        this->setStyleSheet(themeContents);
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::setup_views(QWidget *parent, Ui::MainWindow &ui)
{
    view_handler = ViewsHandler::getInstance(parent, ui);
}

