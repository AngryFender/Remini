#include "mainwindow.h"
#include "theme.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    win = &WindowApi::instance();
    ui->setupUi(this);
    setup_views(this, *ui);

    lightThemeStyle = QStyleFactory::create("windowsvista");
    darkThemeStyle = QStyleFactory::create("fusion");

    themeContents = darkTheme;
    themeState = darkThemeState;
    QApplication::setStyle(QStyleFactory::create("fusion"));
    this->setStyleSheet(themeContents);

    rightShiftTimer = new QTimer(this);
    leftShiftTimer = new QTimer(this);
    QObject::connect(rightShiftTimer,&QTimer::timeout,
                     this, &MainWindow::shiftTimerHandle);

    QObject::connect(leftShiftTimer,&QTimer::timeout,
                     this, &MainWindow::shiftTimerHandle);

    QObject::connect(this,&MainWindow::openRecentFilesDialog,
                     view_handler.get(),&ViewsHandler::openRecentFilesDialogHandle);

    QObject::connect(this,&MainWindow::startSearchAll,
                     view_handler.get(),&ViewsHandler::startTextSearchInAllFilesHandle);

    QObject::connect(this,&MainWindow::startFileSearch,
                     view_handler.get(),&ViewsHandler::startFileSearchHandle);

    QObject::connect(win, &WindowApi::showApp,
                     this,&MainWindow::showHideApp);

}

MainWindow::~MainWindow()
{
    delete rightShiftTimer;
    delete lightThemeStyle;
    delete darkThemeStyle;
    //delete ui;
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->nativeScanCode()) {

    case RIGHT_SHIFT_KEY:
            if(rightShiftTimer->isActive()){
                emit startSearchAll();
            }
            rightShiftTimer->start(DOUBLE_SHIFT_TIMER_MS);
            leftShiftTimer->stop();
            break;
    case LEFT_SHIFT_KEY:
            if(leftShiftTimer->isActive()){
                emit startFileSearch();
            }
            leftShiftTimer->start(DOUBLE_SHIFT_TIMER_MS);
            rightShiftTimer->stop();
            break;
    default:
            rightShiftTimer->stop();
            leftShiftTimer->stop();
    };

    switch(event->key()){
        case Qt::Key_F12:
                if (themeState == darkThemeState){
                    themeState = lightThemeState;
                    this->setStyleSheet(lightTheme);
                    QApplication::setStyle(lightThemeStyle);
                }else{
                    themeState = darkThemeState;
                    this->setStyleSheet(darkTheme);
                    QApplication::setStyle(darkThemeStyle);
                }
                break;

        case Qt::Key_Alt:
                fileSearchBox->hide();
                folderTreeBox->hide();
                mkEditorBox->hide();
                break;

    }
}

void MainWindow::recentFilesHandler(bool show)
{
        emit openRecentFilesDialog(show);
}

void MainWindow::showHideApp()
{
    if(this->isMinimized()){
        this->showNormal();
    }else{
        this->showMinimized();
    }
}

void MainWindow::shiftTimerHandle()
{
    rightShiftTimer->stop();
    leftShiftTimer->stop();
}

void MainWindow::setup_views(QWidget *parent, Ui::MainWindow &ui)
{
    view_handler = ViewsHandler::getInstance(parent, ui);
}
