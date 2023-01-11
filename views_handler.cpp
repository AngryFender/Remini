#include "views_handler.h"



void Views_handler::init_models()
{
    model_tree.setRootPath(QDir::homePath());
}

void Views_handler::init_views(Ui::MainWindow &ui)
{
    ui.ui_tree_view->setModel(&model_tree);
}
