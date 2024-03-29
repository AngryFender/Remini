#ifndef NAVIGATIONMODEL_H
#define NAVIGATIONMODEL_H

#include <QFileSystemModel>
#include <QDesktopServices>
#include <QSortFilterProxyModel>
#include <QUrl>
#include <QObject>
#include <Windows.h>
#include <QClipboard>
#include <QApplication>

class NavigationProxyModel : public QSortFilterProxyModel{

    Q_OBJECT

public:
    NavigationProxyModel(QObject *parent = nullptr);
    QModelIndex setRootIndexFromPath(QString path);
    QFileInfo getFileInfoMappedToSource(const QModelIndex &index);
    QFileInfo getFileInfo(const QModelIndex &index);

public slots:
    void createFileHandler(QModelIndex& index, QString &name);
    void createFolderHandler(QModelIndex& index, QString &name);
    void deleteFileFolderHandler(QModelIndex& index);
    void openLocationHandler(QModelIndex& index);
    void copyFileFolderHandler(QModelIndex& index);
    void createAllFoldersList(QModelIndex index, QStringList &listPath);
    void createAllFilesList(QModelIndex index, QStringList &listPath);
signals:
    void folderChanged();
private:
    void uniqueFileName(QFile &file, QString &name, QString &type, const QString &path);
    void uniqueFolderName(QDir &dir, QString &name, const QString &path);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
private:

    bool filterChildIndex(QFileSystemModel * model, int source_row, const QModelIndex &source_parent) const;
    bool isSubdirectory(const QString &subDirPath, const QString &parentDirPath) const;
};


#endif // NAVIGATIONMODEL_H
