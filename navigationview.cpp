#include "navigationview.h"

NavigationView::NavigationView(QWidget *parent):QTreeView(parent)
{
    this->setColumnHidden(1,true);
    this->setHeaderHidden(true);
    this->setRootIsDecorated(false);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    addFileAction.setText("Add File");
    addFolderAction.setText("Add Folder");
    renameFileAction.setText("Rename");
    deleteFileAction.setText("Delete");

    connect(&addFileAction, SIGNAL(triggered()), this, SLOT(addFile()));
    connect(&addFolderAction, SIGNAL(triggered()), this, SLOT(addFolder()));
    connect(&renameFileAction, SIGNAL(triggered()), this, SLOT(renameFile()));
    connect(&deleteFileAction, SIGNAL(triggered()), this, SLOT(deleteFile()));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(ContextMenuHandler(QPoint)));
}

void NavigationView::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        QModelIndex index = indexAt(event->pos());
        if(this->isExpanded(index))
        {
            this->collapse(index);
        }
        else
        {
            this->expand(index);
        }
    }
    else if(event->buttons() & Qt::RightButton)
    {
        QModelIndex index = indexAt(event->pos());
        this->expand(index);
    }
    QTreeView::mousePressEvent(event);
}

void NavigationView::mouseDoubleClickEvent(QMouseEvent *event)
{
    //do nothing to avoid editing on doubleclicks
}

void NavigationView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    QTreeView::rowsInserted(parent,start,end);
    if(!newEntryName.isEmpty())
        folderChangedHandler();
}

void NavigationView::ContextMenuHandler(QPoint pos)
{
    QMenu menu(this);

    QModelIndex index = indexAt(pos);
    lastClickedIndex = index;

    if(!index.isValid()){
        menu.addAction(&addFileAction);
        menu.addAction(&addFolderAction);
        menu.exec(viewport()->mapToGlobal(pos));
        return;
    }

    QFileSystemModel *fileModel = qobject_cast<QFileSystemModel *>(this->model());

    if(!fileModel)
        return;

    QFileInfo fileInfo = fileModel->fileInfo(index);

    if (fileInfo.isDir()) {
        menu.addAction(&addFileAction);
        menu.addAction(&addFolderAction);
    }
    menu.addAction(&renameFileAction);
    menu.addAction(&deleteFileAction);
    menu.exec(viewport()->mapToGlobal(pos));
}

void NavigationView::addFile()
{
    auto index = lastClickedIndex;
    QString fileName;
    emit createFile(index, fileName);
    newEntryName = fileName;
}

void NavigationView::addFolder()
{
    auto index = lastClickedIndex;
    QString folderName;
    emit createFolder(index, folderName);
    newEntryName = folderName;
}

void NavigationView::renameFile()
{
    auto index = this->currentIndex();
    this->edit(index);
}

void NavigationView::deleteFile()
{
    auto indexes = this->selectedIndexes();
    for(auto index: indexes){
        emit deleteFileFolder(index);
    }
}

void NavigationView::folderChangedHandler()
{
    QModelIndex index =lastClickedIndex;
    if(!index.isValid()){
        index = rootIndex();
    }
    int totalFiles = model()->rowCount(index);
    for( int i = 0 ; i < totalFiles; i++){
        QModelIndex fileIndex = model()->index(i,0,index);
        if(!fileIndex.isValid())
            continue;

        QString name = model()->data(fileIndex, Qt::DisplayRole).toString();
        if(name == newEntryName){
            recentlyCreatedFile = fileIndex;
            this->edit(fileIndex);
            newEntryName = "";
            return;
        }
    }
}

void NavigationView::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    if(recentlyCreatedFile.isValid()){
        emit newFileCreated(recentlyCreatedFile);
    }
    QTreeView::closeEditor(editor,hint);
}
