#ifndef NAVIGATIONVIEW_H
#define NAVIGATIONVIEW_H

#include <QTreeView>
#include <QObject>
#include <QWidget>
#include <QMenu>

class NavigationView : public QTreeView
{
    Q_OBJECT
public:
    NavigationView(QWidget * parent);

private:
    QMenu menu;
    QAction addFileAction ;
    QAction renameFileAction;
    QAction deleteFileAction;

 public slots:
    void ContextMenuHandler(QPoint pos);
    void addFile();
    void renameFile();
    void deleteFile();

signals:
    void createFileFolder(QModelIndex &index);
    void deleteFileFolder(QModelIndex &index);
};

#endif // NAVIGATIONVIEW_H
