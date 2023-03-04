#include "navigationview.h"

NavigationView::NavigationView(QWidget *parent):QTreeView(parent)
{
    this->setColumnHidden(1,true);
    this->setHeaderHidden(true);
    this->setRootIsDecorated(false);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    addFileAction.setText("Add File");
    renameFileAction.setText("Rename File");
    deleteFileAction.setText("Delete File");

    connect(&addFileAction, SIGNAL(triggered()), this, SLOT(addFile()));
    connect(&renameFileAction, SIGNAL(triggered()), this, SLOT(renameFile()));
    connect(&deleteFileAction, SIGNAL(triggered()), this, SLOT(deleteFile()));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(ContextMenuHandler(QPoint)));
}

void NavigationView::ContextMenuHandler(QPoint pos)
{
    QMenu menu(this);
    menu.addAction(&addFileAction);
    menu.addAction(&renameFileAction);
    menu.addAction(&deleteFileAction);
    menu.exec(viewport()->mapToGlobal(pos));
}

void NavigationView::addFile()
{
    auto indexes = this->selectedIndexes();
    for(auto index: indexes){
        emit createFileFolder(index);
    }
}

void NavigationView::renameFile()
{
    auto indexes = this->selectedIndexes();
    for(auto index: indexes){
        this->edit(index);
    }
}

void NavigationView::deleteFile()
{
    auto indexes = this->selectedIndexes();
    for(auto index: indexes){
        emit deleteFileFolder(index);
    }
}
