 #include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "theme.h"
#include "views_handler.h"

QSharedPointer<ViewsHandler> view_handler;

void setup_views(Ui::MainWindow &ui){
    view_handler = ViewsHandler::getInstance(ui);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_views(*ui);

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

