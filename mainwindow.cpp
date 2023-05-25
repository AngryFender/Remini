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
    }
    else if(event->key()== Qt::Key_1 && event->modifiers() == Qt::AltModifier){
        QPoint globalPos = mapToGlobal(ui->uiSearch->pos());
        fileSearchBox->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        fileSearchBox->setGeometry(ui->uiSearch->geometry());
        fileSearchBox->move(globalPos);
        fileSearchBox->show();
    }else if(event->key()== Qt::Key_2&& event->modifiers() == Qt::AltModifier){
        QPoint globalPos = mapToGlobal(ui->uiTreeView->pos());
        fileSearchBox->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        fileSearchBox->setGeometry(ui->uiTreeView->geometry());
        fileSearchBox->move(globalPos);
        fileSearchBox->show();
    }else if(event->key()== Qt::Key_3&& event->modifiers() == Qt::AltModifier){
        QPoint globalPos = mapToGlobal(ui->uiTextView->pos());
        fileSearchBox->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        fileSearchBox->setGeometry(ui->uiTextView->geometry());
        fileSearchBox->move(globalPos);
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
