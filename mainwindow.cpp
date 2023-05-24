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

    lightThemeStyle = QStyleFactory::create("windowsvista");
    darkThemeStyle = QStyleFactory::create("fusion");

    themeContents = darkTheme;
    themeState = darkThemeState;
    QApplication::setStyle(QStyleFactory::create("fusion"));
    this->setStyleSheet(themeContents);

    shiftTimer = new QTimer(this);
    QObject::connect(shiftTimer,&QTimer::timeout,
                     this, &MainWindow::shiftTimerHandle);

    QObject::connect(this,SIGNAL(startSearchAll()),
                     view_handler.get(),SLOT(startTextSearchInAllFilesHandle()));
}

MainWindow::~MainWindow()
{
    delete shiftTimer;
    delete lightThemeStyle;
    delete darkThemeStyle;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F12){
        if (themeState == darkThemeState){
            themeState = lightThemeState;
            this->setStyleSheet(lightTheme);
            QApplication::setStyle(lightThemeStyle);
        }else{
            themeState = darkThemeState;
            this->setStyleSheet(darkTheme);
            QApplication::setStyle(darkThemeStyle);
        }
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
