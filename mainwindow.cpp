#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "folder_tree.h"
#include "views_handler.h"

QSharedPointer<Views_handler> view_handler;

void setup_views(Ui::MainWindow &ui){
    view_handler = Views_handler::get_instance(ui);
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

