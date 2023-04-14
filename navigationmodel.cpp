#include "navigationmodel.h"

#include <QDirIterator>


NavigationProxyModel::NavigationProxyModel(QObject *parent):QSortFilterProxyModel(parent)
{

}

QModelIndex NavigationProxyModel::setRootIndexFromPath(QString path)
{
    QFileSystemModel * model =  dynamic_cast<QFileSystemModel*>(this->sourceModel());
    if(model) {
        QModelIndex result = this->mapFromSource(model->index(path));
        return result;
    }
    return QModelIndex();
}

QFileInfo NavigationProxyModel::getFileInfoMappedToSource(const QModelIndex &index)
{
    QFileSystemModel * model =  qobject_cast<QFileSystemModel*>(this->sourceModel());
    if(!model)
        return QFileInfo();

    QModelIndex sourceIndex = this->mapToSource(index);
    return model->fileInfo(sourceIndex);
}

QFileInfo NavigationProxyModel::getFileInfo(const QModelIndex &index)
{
    QFileSystemModel * model =  qobject_cast<QFileSystemModel*>(this->sourceModel());
    if(!model)
        return QFileInfo();

    return model->fileInfo(index);
}

void NavigationProxyModel::createFileHandler(QModelIndex &index, QString &name)
{
    QFileSystemModel * model =  qobject_cast<QFileSystemModel*>(this->sourceModel());
    if(!model)
        return ;


    QModelIndex sourceIndex = this->mapToSource(index);

    QString fileName = "Untitled";
    QString fileType = ".txt";
    QString filePath;
    if(sourceIndex.isValid()){
        filePath = model->filePath(sourceIndex)+"/";
    }else{
        filePath = model->rootPath()+"/";
    }

    QFile file;
    uniqueFileName(file,fileName,fileType,filePath);
    file.open(QIODevice::ReadWrite);
    file.close();
    name = fileName+fileType;
}

void NavigationProxyModel::createFolderHandler(QModelIndex &index, QString &name)
{
    QFileSystemModel * model =  qobject_cast<QFileSystemModel*>(this->sourceModel());
    if(!model)
        return ;


    QModelIndex sourceIndex = this->mapToSource(index);

    QString folderName = "New Folder";
    QString folderPath;
    if(sourceIndex.isValid()){
        QFileInfo info = model->fileInfo(sourceIndex);
        folderPath = info.absoluteFilePath()+"//";
    }else{
        folderPath = model->rootPath()+"//";
    }

    QDir dir;
    uniqueFolderName(dir,folderName, folderPath);
    dir.mkdir(".");
    name = folderName;
    emit model->directoryLoaded(model->rootPath());
}

void NavigationProxyModel::deleteFileFolderHandler(QModelIndex &index)
{
    QFileSystemModel * model =  qobject_cast<QFileSystemModel*>(this->sourceModel());
    if(!model)
        return ;

    QModelIndex sourceIndex = this->mapToSource(index);
    QFileInfo fileInfo = model->fileInfo(sourceIndex);

    QFile file(fileInfo.absoluteFilePath());
    if(file.exists())
        file.moveToTrash();
}

void NavigationProxyModel::openLocationHandler(QModelIndex &index)
{
    QFileSystemModel * model =  qobject_cast<QFileSystemModel*>(this->sourceModel());
    if(!model)
        return ;

    QModelIndex sourceIndex = this->mapToSource(index);

    QString folderPath;

    if(!sourceIndex.isValid()){
        folderPath = model->rootPath();
    }else{
        QFileInfo fileInfo = model->fileInfo(sourceIndex);
        if (!fileInfo.isDir()) {
            fileInfo.setFile(fileInfo.absoluteFilePath());
        }
        folderPath = fileInfo.absolutePath();
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
}
void NavigationProxyModel::createAllFoldersList(QModelIndex index, QStringList &listPath)
{
    QFileSystemModel * model =  qobject_cast<QFileSystemModel*>(this->sourceModel());
    if(!model) {
        return;
    }

    QModelIndex rootIndex = this->mapToSource(index);
    QFileInfo info = model->fileInfo(rootIndex);
    QString path = info.absoluteFilePath();
    if(info.isDir())
    {
        QDir dir(path);
        if(dir.isEmpty()){
            return;
        }else{
            dir.setFilter( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot );
            if(model->hasChildren(rootIndex)){
                QDirIterator di(path, QDir::Dirs, QDirIterator::Subdirectories);
                while(di.hasNext()){
                    di.next();
                    if(di.fileInfo().isDir()){
                        listPath.append(di.fileInfo().absoluteFilePath());
                    }
                }
            }
        }
    }
}

void NavigationProxyModel::createAllFilesList(QModelIndex index, QStringList &listPath)
{
    QFileSystemModel * model =  qobject_cast<QFileSystemModel*>(this->sourceModel());
    if(!model) {
        return;
    }

    listPath.clear();
    QModelIndex rootIndex = this->mapToSource(index);
    QFileInfo info = model->fileInfo(rootIndex);
    QString path = info.absoluteFilePath();
    if(info.isDir())
    {
        QDir dir(path);
        if(dir.isEmpty()){
            return;
        }else{
            dir.setFilter( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot );
            if(model->hasChildren(rootIndex)){
                QDirIterator di(path, QDir::Files, QDirIterator::Subdirectories);
                while(di.hasNext()){
                    di.next();
                    if(di.fileInfo().isFile()){
                        listPath.append(di.fileInfo().absoluteFilePath());
                    }
                }
            }
        }
    }
}

void NavigationProxyModel::uniqueFileName(QFile &file, QString &name, QString &type, const QString &path)
{
    file.setFileName(path+name+type);
    if (file.exists()){
        name = name+"_new";
        uniqueFileName(file,name,type,path);
    }
}

void NavigationProxyModel::uniqueFolderName(QDir &dir, QString &name, const QString &path)
{
    dir.setPath(path+name);
    if (dir.exists()){
        name = name+"_new";
        uniqueFolderName(dir,name,path);
    }
}

bool NavigationProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QFileSystemModel * model =  qobject_cast<QFileSystemModel*>(this->sourceModel());
    QModelIndex childIndex = model->index(source_row,0,source_parent);

    if(filterRegularExpression().pattern() == "")
        return true;

    if(!model)
        return false;

    QString rootPath = model->rootPath()+"//";
    QString infoPath = model->fileInfo(childIndex).absoluteFilePath();

    if(!isSubdirectory(infoPath,rootPath))
        return true;

    return filterChildIndex( model, source_row, childIndex);
}

bool NavigationProxyModel::filterChildIndex( QFileSystemModel* model, int source_row, const QModelIndex &source_parent) const
{
    QFileInfo info = model->fileInfo(source_parent);
    QString fileName = info.fileName().toLower();

    if(info.isFile()){
        if(fileName.contains(filterRegularExpression().pattern())){
            return true;
        }else{
            return false;
        }
    }

    if(info.isDir())
    {
        QDir dir(info.absoluteFilePath());
        if(dir.isEmpty()){
            return false;
        }else{
            dir.setFilter( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot );
            int rowCount = dir.count();
            bool childResult = false;

            for(int row = 0; row<rowCount; row++){
                QModelIndex childIndex = model->index(row,0,source_parent);
                if(filterChildIndex( model, row, childIndex)){
                    return true;
                }
            }
        }
    }
    return false;
}

bool NavigationProxyModel::isSubdirectory(const QString &subDirPath, const QString &parentDirPath) const
{
    QDir subDir(subDirPath);
    QDir parentDir(parentDirPath);

    QString absoluteSubDirPath = subDir.absolutePath();
    QString absoluteParentDirPath = parentDir.absolutePath();

    return absoluteSubDirPath.startsWith(absoluteParentDirPath);
}
