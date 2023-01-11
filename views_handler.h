#ifndef VIEWS_HANDLER_H
#define VIEWS_HANDLER_H
#include <QFileSystemModel>
#include <QSharedPointer>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

class Views_handler
{

public:
    Views_handler(Views_handler &other) = delete;
    void operator=(const Views_handler&) = delete;

    static QSharedPointer<Views_handler> get_instance(Ui::MainWindow &ui)
    {
        static QSharedPointer<Views_handler> handle{new Views_handler(ui) };
        return handle;
    }

private:
    Views_handler(Ui::MainWindow &ui){
        init_models();
        init_views(ui);
    }
    QFileSystemModel model_tree;
    void init_models();
    void init_views(Ui::MainWindow &ui);
};

#endif // VIEWS_HANDLER_H
