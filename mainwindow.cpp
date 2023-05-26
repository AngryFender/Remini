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
    switch(event->key()){
        case Qt::Key_F12:{
                if (themeState == darkThemeState){
                    themeState = lightThemeState;
                    this->setStyleSheet(lightTheme);
                    QApplication::setStyle(lightThemeStyle);
                }else{
                    themeState = darkThemeState;
                    this->setStyleSheet(darkTheme);
                    QApplication::setStyle(darkThemeStyle);
                }
            }; break;

        case Qt::Key_Shift:{
                if(shiftTimer->isActive()){
                    emit startSearchAll();
                }
                shiftTimer->start(200);
            }break;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Alt){
        fileSearchBox->hide();
        folderTreeBox->hide();
        mkEditorBox->hide();
    }
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::KeyPressAltHandler(bool press)
{
    fileSearchBox->hide();
    folderTreeBox->hide();
    mkEditorBox->hide();

    if(press){
        QPoint fileSearchPos = mapToGlobal(ui->uiSearch->pos());
        fileSearchBox->setGeometry(ui->uiSearch->geometry());
        fileSearchBox->move(fileSearchPos);
        fileSearchBox->show();

        QPoint folderTreePos = mapToGlobal(ui->uiTreeView->pos());
        folderTreeBox->setGeometry(ui->uiTreeView->geometry());
        folderTreeBox->move(folderTreePos);
        folderTreeBox->show();

        QPoint mkEditorPos = QWidget::mapToGlobal(ui->uiRightPane->pos());
        mkEditorBox->setGeometry(ui->uiRightPane->geometry());
        mkEditorBox->move(mkEditorPos);
        mkEditorBox->show();
    }
}

void MainWindow::viewChosenHandler(Qt::Key key)
{

}

void MainWindow::shiftTimerHandle()
{
    shiftTimer->stop();
}

void MainWindow::setup_views(QWidget *parent, Ui::MainWindow &ui)
{
    view_handler = ViewsHandler::getInstance(parent, ui);
}
