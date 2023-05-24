#include "mainwindow.h"
#include "theme.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_views(this, *ui);

    fileSearchBox = new TransparentDialog(this);
    folderTreeBox = new TransparentDialog(this);
    mkEditorBox = new TransparentDialog(this);
    themeContents = darkTheme;
    themeState = darkThemeState;
    QApplication::setStyle(QStyleFactory::create("fusion"));
    this->setStyleSheet(themeContents);

    shiftTimer = new QTimer(this);
    QObject::connect(shiftTimer,SIGNAL(timeout()),
                     this, SLOT(shiftTimerHandle()));

    QObject::connect(this,SIGNAL(startSearchAll()),
                     view_handler.get(),SLOT(startTextSearchInAllFilesHandle()));
}

MainWindow::~MainWindow()
{
    delete shiftTimer;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F12){
        if (themeState == darkThemeState){
            themeContents = lightTheme;
            themeState = lightThemeState;
            QApplication::setStyle(QStyleFactory::create("windowsvista"));
        }else{
            themeContents = darkTheme;
            themeState = darkThemeState;
            QApplication::setStyle(QStyleFactory::create("fusion"));
        }
        this->setStyleSheet(themeContents);
    }else if(event->key() == Qt::Key_Shift){
    } else if (event->key() == Qt::Key_Alt) {
        fileSearchBox->setDimension(ui->uiSearch->x(), ui->uiSearch->y(),ui->uiSearch->width(),ui->uiSearch->height());
        fileSearchBox->show();

    } else if (event->key() == Qt::Key_Shift) {
        if(shiftTimer->isActive()){
            emit startSearchAll();
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
