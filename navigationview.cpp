#include "navigationview.h"

#include <QApplication>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QMenu>
#include <QTimer>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
NavigationView::NavigationView(QWidget *parent, bool editable)
    : QTreeView(parent), editable(editable)
{
    this->setColumnHidden(1, true);
    this->setHeaderHidden(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    addFileAction.setText(QStringLiteral("Add File"));
    addFolderAction.setText(QStringLiteral("Add Folder"));
    renameFileAction.setText(QStringLiteral("Rename"));
    deleteFileAction.setText(QStringLiteral("Delete"));
    openLocationAction.setText(QStringLiteral("Open Location"));
    copyPath.setText(QStringLiteral("Copy Path"));
    SetVault.setText(QStringLiteral("Set Vault Path"));

    connect(&addFileAction, &QAction::triggered, this,
            &NavigationView::addFile);
    connect(&addFolderAction, &QAction::triggered, this,
            &NavigationView::addFolder);
    connect(&renameFileAction, &QAction::triggered, this,
            &NavigationView::renameFile);
    connect(&deleteFileAction, &QAction::triggered, this,
            &NavigationView::deleteFile);
    connect(&openLocationAction, &QAction::triggered, this,
            &NavigationView::openFileFolder);
    connect(&copyPath, &QAction::triggered, this,
            &NavigationView::copyFileFolderPath);
    connect(&SetVault, &QAction::triggered, this,
            &NavigationView::setVaultHandler);

    connect(this, &NavigationView::customContextMenuRequested, this,
            &NavigationView::ContextMenuHandler);

    connect(&expandTimer, &QTimer::timeout, this,
            &NavigationView::expandTimerHandler);

    connect(this, &NavigationView::clicked, this, &NavigationView::rowClicked);
}

// ---------------------------------------------------------------------------
// Enable or disable inline editing
// ---------------------------------------------------------------------------
void NavigationView::setRowsEditable(bool enable) { editable = enable; }

// ---------------------------------------------------------------------------
// Start the delayed expansion timer (used after bulk model changes)
// ---------------------------------------------------------------------------
void NavigationView::expandEveryItems(QModelIndex index)
{
    Q_UNUSED(index);

    if (!expandTimer.isActive())
        expandTimer.start(TIME_PERIOD_FOR_EXPANSION);
}

// ---------------------------------------------------------------------------
// Key handling
// ---------------------------------------------------------------------------
void NavigationView::keyPressEvent(QKeyEvent *event)
{
    QTreeView::keyPressEvent(event);

    QModelIndex index = this->currentIndex();
    if (isPersistentEditorOpen(index))
        return;

    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        emit pressed(index);

        if (this->isExpanded(index))
            this->collapse(index);
        else
            this->expand(index);
    }
    else if (event->key() == Qt::Key_Backspace)
    {
        emit sendFocusToSearch(this);
    }
}

// ---------------------------------------------------------------------------
// Font
// ---------------------------------------------------------------------------
void NavigationView::setFont(const QFont &font)
{
    addFileAction.setFont(font);
    addFolderAction.setFont(font);
    renameFileAction.setFont(font);
    deleteFileAction.setFont(font);
    openLocationAction.setFont(font);
    copyPath.setFont(font);
    SetVault.setFont(font);

    QTreeView::setFont(font);
}

// ---------------------------------------------------------------------------
// Called after new rows are inserted into the model
// ---------------------------------------------------------------------------
void NavigationView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    QTreeView::rowsInserted(parent, start, end);

    if (!newEntryName.isEmpty())
        folderChangedHandler();
}

// ---------------------------------------------------------------------------
// Mouse double click
// ---------------------------------------------------------------------------
void NavigationView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (editable)
    {
        renameFile();
        event->accept();
        return;
    }

    // Fall back to the default expand/collapse behaviour when not editable.
    QTreeView::mouseDoubleClickEvent(event);
}

// ---------------------------------------------------------------------------
// Context menu
// ---------------------------------------------------------------------------
void NavigationView::ContextMenuHandler(QPoint pos)
{
    QMenu menu(this);

    QModelIndex index = indexAt(pos);
    lastClickedIndex = index;

    if (!index.isValid())
    {
        menu.addAction(&addFileAction);
        menu.addAction(&addFolderAction);
        menu.addAction(&openLocationAction);
        menu.addAction(&copyPath);
        menu.addAction(&SetVault);
        menu.exec(viewport()->mapToGlobal(pos));
        return;
    }

    NavigationProxyModel *fileModel =
        qobject_cast<NavigationProxyModel *>(this->model());
    if (!fileModel)
        return;

    QFileInfo fileInfo = fileModel->getFileInfoMappedToSource(index);

    if (fileInfo.isDir())
    {
        menu.addAction(&addFileAction);
        menu.addAction(&addFolderAction);
    }
    menu.addAction(&renameFileAction);
    menu.addAction(&deleteFileAction);
    menu.addAction(&openLocationAction);
    menu.addAction(&copyPath);

    menu.exec(viewport()->mapToGlobal(pos));
}

// ---------------------------------------------------------------------------
// Add file
// ---------------------------------------------------------------------------
void NavigationView::addFile()
{
    QModelIndex index = lastClickedIndex;
    this->expand(index);

    emit emptySearch();
    this->scrollTo(index);

    const QList<QModelIndex> selections = this->selectedIndexes();
    if (!selections.empty() && lastClickedIndex.isValid())
        index = selections.first();

    QString fileName;
    emit createFile(index, fileName);
    lastClickedIndex = index;
    newEntryName = fileName;
}

// ---------------------------------------------------------------------------
// Add folder
// ---------------------------------------------------------------------------
void NavigationView::addFolder()
{
    QModelIndex index = lastClickedIndex;
    this->expand(index);

    emit emptySearch();
    this->scrollTo(index);

    const QList<QModelIndex> selections = this->selectedIndexes();
    if (!selections.isEmpty() && lastClickedIndex.isValid())
        index = selections.first();

    QString folderName;
    emit createFolder(index, folderName);
    lastClickedIndex = index;
    newEntryName = folderName;
}

// ---------------------------------------------------------------------------
// Rename
// ---------------------------------------------------------------------------
void NavigationView::renameFile()
{
    const QModelIndex index = this->currentIndex();
    if (!index.isValid())
        return;

    editingFilename = index.data(Qt::DisplayRole).toString();
    this->edit(index);
}

// ---------------------------------------------------------------------------
// Delete
// ---------------------------------------------------------------------------
void NavigationView::deleteFile()
{
    const QList<QModelIndex> indexes = this->selectedIndexes();
    for (const QModelIndex &index : indexes)
        emit deleteFileFolder(const_cast<QModelIndex &>(index));
}

// ---------------------------------------------------------------------------
// Open containing location
// ---------------------------------------------------------------------------
void NavigationView::openFileFolder()
{
    QModelIndex index = lastClickedIndex;
    emit openLocation(index);
}

// ---------------------------------------------------------------------------
// Copy path to clipboard
// ---------------------------------------------------------------------------
void NavigationView::copyFileFolderPath()
{
    QModelIndex index = lastClickedIndex;
    emit copyFolderFilePath(index);
}

// ---------------------------------------------------------------------------
// Set vault path
// ---------------------------------------------------------------------------
void NavigationView::setVaultHandler() { emit setVaultPath(); }

// ---------------------------------------------------------------------------
// After the model has been updated, look for the newly created entry and
// start inline editing on it. Delayed via QTimer::singleShot(0) so that the
// view has regained focus before edit() is called (important on Wayland).
// ---------------------------------------------------------------------------
void NavigationView::folderChangedHandler()
{
    QModelIndex index = lastClickedIndex;
    if (!index.isValid())
        index = rootIndex();

    const int totalFiles = model()->rowCount(index);
    for (int i = 0; i < totalFiles; ++i)
    {
        const QModelIndex fileIndex = model()->index(i, 0, index);
        if (!fileIndex.isValid())
            continue;

        const QString name =
            model()->data(fileIndex, Qt::DisplayRole).toString();
        if (name != newEntryName)
            continue;

        this->setCurrentIndex(fileIndex);
        this->setFocus();

        editingFilename = name;
        newEntryName.clear();

        emit newFileCreated(fileIndex);

        // Defer edit() to the next event-loop iteration so that focus is
        // guaranteed to be back on this view. This avoids silent failures
        // on Wayland where focus may still be pending.
        QTimer::singleShot(0, this,
                           [this, fileIndex]() { this->edit(fileIndex); });
        return;
    }
}

// ---------------------------------------------------------------------------
// Close inline editor.
// Read the new value from the editor widget, not from the model (the model
// still holds the old value at this point).
// ---------------------------------------------------------------------------
void NavigationView::closeEditor(QWidget *editor,
                                 QAbstractItemDelegate::EndEditHint hint)
{
    QString newName;
    if (auto *lineEdit = qobject_cast<QLineEdit *>(editor))
        newName = lineEdit->text();

    QTreeView::closeEditor(editor, hint);

    const QModelIndex selected = this->currentIndex();
    emit newFileCreated(selected);

    if (!newName.isEmpty() && editingFilename != newName)
        emit fileRenamed(newName, editingFilename, selected);
}

// ---------------------------------------------------------------------------
// Expansion timer handler
// ---------------------------------------------------------------------------
void NavigationView::expandTimerHandler()
{
    expandTimer.stop();
    emit expansionComplete();
}

// ---------------------------------------------------------------------------
// Row clicked -> toggle expand/collapse
// ---------------------------------------------------------------------------
void NavigationView::rowClicked(const QModelIndex &index)
{
    if (this->isExpanded(index))
        this->collapse(index);
    else
        this->expand(index);
}
