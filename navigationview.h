#ifndef NAVIGATIONVIEW_H
#define NAVIGATIONVIEW_H

#include <QTreeView>
#include <QObject>
#include <QWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QFileInfo>
#include <navigationmodel.h>
#include <QTimer>

#define TIME_PERIOD_FOR_EXPANSION 100

class NavigationView : public QTreeView
{
    Q_OBJECT
public:
    NavigationView(QWidget * parent = nullptr, bool editable = true);
    void setRowsEditable(bool enable);
    void expandEveryItems(QModelIndex index);
    void keyPressEvent(QKeyEvent *event)override;
    void setFont(const QFont &font);
private:
    bool editable;
    QMenu menu;
    QAction addFileAction ;
    QAction addFolderAction ;
    QAction renameFileAction;
    QAction deleteFileAction;
    QAction openLocationAction ;
    QAction copyPath;
    QAction SetVault;
    QString newEntryName;
    QModelIndex lastClickedIndex;
    QTimer expandTimer;
    QTimer fileFolderCreatedTimer;
    QString editingFilename;

protected:
    void rowsInserted(const QModelIndex &parent, int start, int end) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
 public slots:
    void ContextMenuHandler(QPoint pos);
    void addFile();
    void addFolder();
    void renameFile();
    void deleteFile();
    void openFileFolder();
    void copyFileFolderPath();
    void setVaultHandler();
    void folderChangedHandler();
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) override;
    void expandTimerHandler();
    void rowClicked(const QModelIndex &index);
signals:
    void createFile(QModelIndex &index, QString &name);
    void createFolder(QModelIndex &index, QString &name);
    void deleteFileFolder(QModelIndex &index);
    void newFileCreated(const QModelIndex &index);
    void openLocation(QModelIndex &index);
    void copyFolderFilePath(QModelIndex &index);
    void setVaultPath();
    void expansionComplete();
    void fileRenamed(const QString& newName,const QString& oldName, const QModelIndex& index);
    void sendFocusToSearch(QWidget * view);
};

#endif // NAVIGATIONVIEW_H
