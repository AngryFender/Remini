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
    ui->setupUi(this);
    setup_views(*ui);
}

MainWindow::~MainWindow()
{
    delete ui;
}

