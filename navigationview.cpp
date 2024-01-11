#include "navigationview.h"

NavigationView::NavigationView(QWidget *parent, bool editable):QTreeView(parent)
{
    this->setColumnHidden(1,true);
    this->setHeaderHidden(true);
    if(!editable){
        this->setEditTriggers(EditTrigger::NoEditTriggers);
    }

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    addFileAction.setText("Add File");
    addFolderAction.setText("Add Folder");
    renameFileAction.setText("Rename");
    deleteFileAction.setText("Delete");
    openLocationAction.setText("Open Location");
    copyPath.setText("Copy Path");
    SetVault.setText("Set Vault Path");

    connect(&addFileAction, &QAction::triggered, this, &NavigationView::addFile);
    connect(&addFolderAction, &QAction::triggered, this, &NavigationView::addFolder);
    connect(&renameFileAction, &QAction::triggered, this, &NavigationView::renameFile);
    connect(&deleteFileAction, &QAction::triggered, this, &NavigationView::deleteFile);
    connect(&openLocationAction, &QAction::triggered, this, &NavigationView::openFileFolder);
    connect(&copyPath, &QAction::triggered, this, &NavigationView::copyFileFolderPath);
    connect(&SetVault, &QAction::triggered, this, &NavigationView::setVaultHandler);
    connect(this, &NavigationView::customContextMenuRequested, this, &NavigationView::ContextMenuHandler);
    connect(&expandTimer,&QTimer::timeout, this, &NavigationView::expandTimerHandler);
    connect(this,&NavigationView::clicked, this, &NavigationView::rowClicked);
}

void NavigationView::expandEveryItems(QModelIndex index)
{
    if(!expandTimer.isActive())
        expandTimer.start(TIME_PERIOD_FOR_EXPANSION);
}

void NavigationView::keyPressEvent(QKeyEvent *event)
{
    QTreeView::keyPressEvent(event);

    QModelIndex index = this->currentIndex();
    if(!isPersistentEditorOpen(index))
    {
        if(event->key()== Qt::Key_Enter || event->key() == Qt::Key_Return ){
            emit pressed(index);

            if(this->isExpanded(index))
            {
                this->collapse(index);
            }
            else
            {
                this->expand(index);
            }
        }
    }
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
        menu.addAction(&openLocationAction);
        menu.addAction(&copyPath);
        menu.addAction(&SetVault);
        menu.exec(viewport()->mapToGlobal(pos));
        return;
    }

    NavigationProxyModel *fileModel = qobject_cast<NavigationProxyModel *>(this->model());

    if(!fileModel)
        return;

    QFileInfo fileInfo = fileModel->getFileInfoMappedToSource(index);

    if (fileInfo.isDir()) {
        menu.addAction(&addFileAction);
        menu.addAction(&addFolderAction);
    }
    menu.addAction(&renameFileAction);
    menu.addAction(&deleteFileAction);
    menu.addAction(&openLocationAction);
    menu.addAction(&copyPath);
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
    editingFilename = index.data().toString();
    this->edit(index);
}

void NavigationView::deleteFile()
{
    auto indexes = this->selectedIndexes();
    for(auto index: indexes){
        emit deleteFileFolder(index);
    }
}

void NavigationView::openFileFolder()
{
    auto index = lastClickedIndex;
    emit openLocation(index);
}

void NavigationView::copyFileFolderPath()
{
    auto index = lastClickedIndex;
    emit copyFolderFilePath(index);
}

void NavigationView::setVaultHandler()
{
    emit setVaultPath();
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
            this->setCurrentIndex(fileIndex);
            emit newFileCreated(fileIndex);
            this->edit(fileIndex);
            newEntryName = "";
            return;
        }
    }
}

void NavigationView::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    QModelIndex selected = this->currentIndex();
    QString finishedEditingFilename = selected.data(Qt::DisplayRole).toString();
    QTreeView::closeEditor(editor,hint);
    emit newFileCreated(selected);
    if(editingFilename != finishedEditingFilename){
        emit fileRenamed(finishedEditingFilename, editingFilename, selected);
    }
}


void NavigationView::expandTimerHandler()
{
    expandTimer.stop();
    emit expansionComplete();
}

void NavigationView::rowClicked(const QModelIndex &index)
{
    if(this->isExpanded(index))
    {
        this->collapse(index);
    }
    else
    {
        this->expand(index);
    }
}
