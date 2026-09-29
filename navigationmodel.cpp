#include "navigationmodel.h"

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QUrl>

NavigationProxyModel::NavigationProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

QModelIndex NavigationProxyModel::setRootIndexFromPath(QString path)
{
    QFileSystemModel *model =
        dynamic_cast<QFileSystemModel *>(this->sourceModel());
    if (model)
    {
        QModelIndex result = this->mapFromSource(model->index(path));
        return result;
    }
    return QModelIndex();
}

QFileInfo NavigationProxyModel::getFileInfoMappedToSource(
    const QModelIndex &index)
{
    QFileSystemModel *model =
        qobject_cast<QFileSystemModel *>(this->sourceModel());
    if (!model)
        return QFileInfo();

    QModelIndex sourceIndex = this->mapToSource(index);
    return model->fileInfo(sourceIndex);
}

QFileInfo NavigationProxyModel::getFileInfo(const QModelIndex &index)
{
    QFileSystemModel *model =
        qobject_cast<QFileSystemModel *>(this->sourceModel());
    if (!model)
        return QFileInfo();

    return model->fileInfo(index);
}

void NavigationProxyModel::createFileHandler(QModelIndex &index, QString &name)
{
    QFileSystemModel *model =
        qobject_cast<QFileSystemModel *>(this->sourceModel());
    if (!model)
        return;

    QModelIndex sourceIndex = this->mapToSource(index);

    QString fileName = QStringLiteral("Untitled");
    QString fileType = QStringLiteral(".txt");
    QString filePath;

    if (sourceIndex.isValid())
        filePath = QDir(model->filePath(sourceIndex)).absolutePath();
    else
        filePath = QDir(model->rootPath()).absolutePath();

    QFile file;
    uniqueFileName(file, fileName, fileType, filePath + QDir::separator());

    file.open(QIODevice::ReadWrite);
    file.close();

    name = fileName + fileType;
}

void NavigationProxyModel::createFolderHandler(QModelIndex &index,
                                               QString &name)
{
    QFileSystemModel *model =
        qobject_cast<QFileSystemModel *>(this->sourceModel());
    if (!model)
        return;

    QModelIndex sourceIndex = this->mapToSource(index);

    QString folderName = QStringLiteral("New Folder");
    QString folderPath;

    if (sourceIndex.isValid())
    {
        QFileInfo info = model->fileInfo(sourceIndex);
        folderPath = info.absoluteFilePath();
    }
    else
    {
        folderPath = model->rootPath();
    }

    QDir dir(folderPath);
    uniqueFolderName(dir, folderName, folderPath + QDir::separator());

    dir.mkdir(folderName);
    name = folderName;
}

void NavigationProxyModel::deleteFileFolderHandler(QModelIndex &index)
{
    QFileSystemModel *model =
        qobject_cast<QFileSystemModel *>(this->sourceModel());
    if (!model)
        return;

    QModelIndex sourceIndex = this->mapToSource(index);
    QFileInfo fileInfo = model->fileInfo(sourceIndex);

    if (!fileInfo.exists())
        return;

    // Cross-platform trash: Windows Recycle Bin / XDG trash / macOS Trash.
    QFile::moveToTrash(fileInfo.absoluteFilePath());
}

void NavigationProxyModel::openLocationHandler(QModelIndex &index)
{
    QFileSystemModel *model =
        qobject_cast<QFileSystemModel *>(this->sourceModel());
    if (!model)
        return;

    QModelIndex sourceIndex = this->mapToSource(index);

    QString folderPath;

    if (!sourceIndex.isValid())
    {
        folderPath = model->rootPath();
    }
    else
    {
        QFileInfo fileInfo = model->fileInfo(sourceIndex);
        if (!fileInfo.isDir())
            fileInfo.setFile(fileInfo.absoluteFilePath());
        folderPath = fileInfo.absolutePath();
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
}

void NavigationProxyModel::copyFileFolderHandler(QModelIndex &index)
{
    QFileSystemModel *model =
        qobject_cast<QFileSystemModel *>(this->sourceModel());
    if (!model)
        return;

    QModelIndex sourceIndex = this->mapToSource(index);

    QString path;
    if (!sourceIndex.isValid())
        path = model->rootPath();
    else
        path = model->fileInfo(sourceIndex).absoluteFilePath();

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(path);
}

void NavigationProxyModel::createAllFoldersList(QModelIndex index,
                                                QStringList &listPath)
{
    QFileSystemModel *model =
        qobject_cast<QFileSystemModel *>(this->sourceModel());
    if (!model)
        return;

    QModelIndex rootIndex = this->mapToSource(index);
    QFileInfo info = model->fileInfo(rootIndex);
    QString path = info.absoluteFilePath();

    if (info.isDir())
    {
        QDir dir(path);
        if (dir.isEmpty())
            return;

        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

        if (model->hasChildren(rootIndex))
        {
            QDirIterator di(path, QDir::Dirs, QDirIterator::Subdirectories);
            while (di.hasNext())
            {
                di.next();
                if (di.fileInfo().isDir())
                    listPath.append(di.fileInfo().absoluteFilePath());
            }
        }
    }
}

void NavigationProxyModel::createAllFilesList(QModelIndex index,
                                              QStringList &listPath)
{
    QFileSystemModel *model =
        qobject_cast<QFileSystemModel *>(this->sourceModel());
    if (!model)
        return;

    listPath.clear();

    QModelIndex rootIndex = this->mapToSource(index);
    QFileInfo info = model->fileInfo(rootIndex);
    QString path = info.absoluteFilePath();

    if (info.isDir())
    {
        QDir dir(path);
        if (dir.isEmpty())
            return;

        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

        if (model->hasChildren(rootIndex))
        {
            QDirIterator di(path, QDir::Files, QDirIterator::Subdirectories);
            while (di.hasNext())
            {
                di.next();
                if (di.fileInfo().isFile())
                    listPath.append(di.fileInfo().absoluteFilePath());
            }
        }
    }
}

void NavigationProxyModel::uniqueFileName(QFile &file, QString &name,
                                          QString &type, const QString &path)
{
    file.setFileName(path + name + type);
    if (file.exists())
    {
        name = name + QStringLiteral("_new");
        uniqueFileName(file, name, type, path);
    }
}

void NavigationProxyModel::uniqueFolderName(QDir &dir, QString &name,
                                            const QString &path)
{
    dir.setPath(path + name);
    if (dir.exists())
    {
        name = name + QStringLiteral("_new");
        uniqueFolderName(dir, name, path);
    }
}

bool NavigationProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex &source_parent) const
{
    QFileSystemModel *model =
        qobject_cast<QFileSystemModel *>(this->sourceModel());
    if (!model)
        return false;

    QModelIndex childIndex = model->index(source_row, 0, source_parent);

    if (filterRegularExpression().pattern().isEmpty())
        return true;

    QString rootPath = model->rootPath();
    QString infoPath = model->fileInfo(childIndex).absoluteFilePath();

    if (!isSubdirectory(infoPath, rootPath))
        return true;

    return filterChildIndex(model, source_row, childIndex);
}

bool NavigationProxyModel::filterChildIndex(
    QFileSystemModel *model, int source_row,
    const QModelIndex &source_parent) const
{
    Q_UNUSED(source_row);

    QFileInfo info = model->fileInfo(source_parent);
    QString fileName = info.fileName().toLower();

    const QString pattern = filterRegularExpression().pattern();

    if (!pattern.isEmpty() && pattern.at(0) == QLatin1Char('/'))
    {
        QString folderName = pattern.mid(1).toLower();

        if (info.isDir())
        {
            if (fileName.contains(folderName))
                return true;

            QDir dir(info.absoluteFilePath());
            dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
            int rowCount = dir.count();

            for (int row = 0; row < rowCount; row++)
            {
                QModelIndex childIndex = model->index(row, 0, source_parent);
                if (filterChildIndex(model, row, childIndex))
                    return true;
            }
        }

        if (info.absolutePath().toLower().contains(folderName))
            return true;
    }
    else
    {
        if (info.isFile())
        {
            if (fileName.contains(pattern))
                return true;
            return false;
        }

        if (info.isDir())
        {
            QDir dir(info.absoluteFilePath());
            if (dir.isEmpty())
                return false;

            dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
            int rowCount = dir.count();

            for (int row = 0; row < rowCount; row++)
            {
                QModelIndex childIndex = model->index(row, 0, source_parent);
                if (filterChildIndex(model, row, childIndex))
                    return true;
            }
        }
    }

    return false;
}

// -----------------------------------------------------------------------------
// Check whether subDirPath is located under parentDirPath.
//
// FIX: The naive `subDir.startsWith(parent)` check gives false positives.
// e.g. "/home/alice2/file" starts with "/home/al" even though alice2 is not
// inside /home/al. Append a trailing separator to the parent path so the
// comparison only succeeds on an actual path-component boundary.
// -----------------------------------------------------------------------------
bool NavigationProxyModel::isSubdirectory(const QString &subDirPath,
                                          const QString &parentDirPath) const
{
    const QString sub = QDir(subDirPath).absolutePath();
    QString parent = QDir(parentDirPath).absolutePath();

    if (sub == parent)
        return true;

    if (!parent.endsWith(QLatin1Char('/')))
        parent += QLatin1Char('/');

    return sub.startsWith(parent);
}
