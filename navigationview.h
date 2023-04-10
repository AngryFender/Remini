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

#define TIME_PERIOD_FOR_EXPANSION 600

class NavigationView : public QTreeView
{
    Q_OBJECT
public:
    NavigationView(QWidget * parent);
    void expandEveryItems(QModelIndex &index);
private:
    QMenu menu;
    QAction addFileAction ;
    QAction addFolderAction ;
    QAction renameFileAction;
    QAction deleteFileAction;
    QAction openLocationAction ;
    QString newEntryName;
    QModelIndex lastClickedIndex;
    QTimer expandTimer;
    QTimer fileFolderCreatedTimer;

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
    void openFileFolder();
    void folderChangedHandler();
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) override;
    void expandTimerHandler();
signals:
    void createFile(QModelIndex &index, QString &name);
    void createFolder(QModelIndex &index, QString &name);
    void deleteFileFolder(QModelIndex &index);
    void newFileCreated(const QModelIndex &index);
    void openLocation(QModelIndex &index);
    void expansionComplete();

};

#endif // NAVIGATIONVIEW_H
