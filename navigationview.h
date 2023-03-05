#ifndef NAVIGATIONVIEW_H
#define NAVIGATIONVIEW_H

#include <QTreeView>
#include <QObject>
#include <QWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QFileInfo>
#include <QFileSystemModel>

class NavigationView : public QTreeView
{
    Q_OBJECT
public:
    NavigationView(QWidget * parent);

private:
    QMenu menu;
    QAction addFileAction ;
    QAction addFolderAction ;
    QAction renameFileAction;
    QAction deleteFileAction;
    QString newEntryName;
    QModelIndex lastClickedIndex;
    QModelIndex recentlyCreatedFile;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void rowsInserted(const QModelIndex &parent, int start, int end) override;

 public slots:
    void ContextMenuHandler(QPoint pos);
    void addFile();
    void addFolder();
    void renameFile();
    void deleteFile();
    void folderChangedHandler();
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) override;
signals:
    void createFile(QModelIndex &index, QString &name);
    void createFolder(QModelIndex &index, QString &name);
    void deleteFileFolder(QModelIndex &index);
    void newFileCreated(const QModelIndex &index);

};

#endif // NAVIGATIONVIEW_H
