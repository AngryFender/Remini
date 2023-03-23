#include "mainwindow.h"
#include "theme.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_views(this, *ui);

    themeContents = darkTheme;
    themeState = darkThemeState;
    this->setStyleSheet(themeContents);

    shiftTimer = new QTimer(this);
    QObject::connect(shiftTimer,SIGNAL(timeout()),
                     this, SLOT(shiftTimerHandle()));

    dialog = new Dialog(this);
//    dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
}

MainWindow::~MainWindow()
{
    delete shiftTimer;
    delete dialog;
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
    }else if(event->key() == Qt::Key_Shift){
        if(shiftTimer->isActive()){
            openSearch();
        }
        shiftTimer->start(200);
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::shiftTimerHandle()
{
    shiftTimer->stop();
}

void MainWindow::setup_views(QWidget *parent, Ui::MainWindow &ui)
{
    view_handler = ViewsHandler::getInstance(parent, ui);
}

void MainWindow::openSearch()
{
//    int nWidth = 300;
//    int nHeight = 400;
//    dialog->resize(nWidth,nHeight);
    dialog->show();
    qDebug()<<"searching now";
}

