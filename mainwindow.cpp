 #include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "views_handler.h"

QSharedPointer<ViewsHandler> view_handler;

void setup_views(Ui::MainWindow &ui){
    view_handler = ViewsHandler::getInstance(ui);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QFile theme( "dark.qss" );
    QString themeContents;
    if ( theme.exists() )
    {
        theme.open( QFile::ReadOnly | QFile::Text );
        QTextStream ts( &theme );
        themeContents = ts.readAll();
        this->setStyleSheet(themeContents);
    }
    ui->setupUi(this);
    setup_views(*ui);
    this->setStyleSheet(themeContents);
}

MainWindow::~MainWindow()
{
    delete ui;
}

