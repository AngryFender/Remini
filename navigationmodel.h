#ifndef NAVIGATIONMODEL_H
#define NAVIGATIONMODEL_H

#include <QFileSystemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QObject>

class NavigationModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit NavigationModel(QObject *parent = nullptr);
public slots:
    void createFileHandler(QModelIndex& index, QString &name);
    void createFolderHandler(QModelIndex& index, QString &name);
    void deleteFileFolderHandler(QModelIndex& index);
    void openLocationHandler(QModelIndex& index);
signals:
    void folderChanged();
private:
    void uniqueFileName(QFile &file, QString &name, QString &type, const QString &path);
    void uniqueFolderName(QDir &dir, QString &name, const QString &path);
};

#endif // NAVIGATIONMODEL_H
