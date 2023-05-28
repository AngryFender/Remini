#include "mainwindow.h"
#include "theme.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_views(this, *ui);

    fileSearchBox = new TransparentDialog(nullptr,"1");
    folderTreeBox = new TransparentDialog(this,"2");
    mkEditorBox = new TransparentDialog(this,"3");

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
    if(press){
        QPoint fileSearchPos = mapToGlobal(ui->uiSearch->pos());
        fileSearchPos.setY(fileSearchPos.y()-15);
        fileSearchBox->setGeometry(ui->uiSearch->geometry());
        fileSearchBox->move(fileSearchPos);
        if(fileSearchBox->isHidden())fileSearchBox->show();

        QPoint folderTreePos = mapToGlobal(ui->uiTreeView->pos());
        folderTreeBox->setGeometry(ui->uiTreeView->geometry());
        folderTreeBox->move(folderTreePos);
        if(folderTreeBox->isHidden())folderTreeBox->show();

        QPoint mkEditorPos = QWidget::mapToGlobal(ui->uiRightPane->pos());
        mkEditorBox->setGeometry(ui->uiRightPane->geometry());
        mkEditorBox->move(mkEditorPos);
        if(mkEditorBox->isHidden())mkEditorBox->show();
    }else{
        fileSearchBox->hide();
        folderTreeBox->hide();
        mkEditorBox->hide();
    }
}

void MainWindow::viewChosenHandler(Qt::Key key)
{
    fileSearchBox->hide();
    folderTreeBox->hide();
    mkEditorBox->hide();
    switch(key){
        case Qt::Key_1: ui->uiSearch->activateWindow();  ui->uiSearch->setFocus();break;
        case Qt::Key_2: ui->uiTreeView->activateWindow();ui->uiTreeView->setFocus();break;
        case Qt::Key_3: ui->uiTextView->isActiveWindow();ui->uiTextView->setFocus();break;
        default:break;
        }
}

void MainWindow::shiftTimerHandle()
{
    shiftTimer->stop();
}

void MainWindow::setup_views(QWidget *parent, Ui::MainWindow &ui)
{
    view_handler = ViewsHandler::getInstance(parent, ui);
}
