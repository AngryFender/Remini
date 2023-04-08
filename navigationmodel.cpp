#include "navigationmodel.h"

NavigationModel::NavigationModel(QObject *parent)
    : QFileSystemModel{parent}
{
}

void NavigationModel::createFileHandler(QModelIndex &index, QString &name)
{
    QString fileName = "Untitled";
    QString fileType = ".txt";
    QString filePath;
    if(index.isValid()){
        filePath = this->filePath(index)+"/";
    }else{
        filePath = this->rootPath()+"/";
    }

    QFile file;
    uniqueFileName(file,fileName,fileType,filePath);
    file.open(QIODevice::ReadWrite);
    file.close();
    name = fileName+fileType;
}

void NavigationModel::createFolderHandler(QModelIndex &index, QString &name)
{
    QString folderName = "New Folder";
    QString folderPath;
    if(index.isValid()){
        folderPath = this->filePath(index)+"/";
    }else{
        folderPath = this->rootPath()+"/";
    }

    QDir dir;
    uniqueFolderName(dir,folderName, folderPath);
    dir.mkdir(".");
    name = folderName;
}

void NavigationModel::deleteFileFolderHandler(QModelIndex &index)
{
    QString fullFileName = QFileSystemModel::filePath(index);
    QFile file(fullFileName);
    if(file.exists()){
        file.moveToTrash();
    }
}

void NavigationModel::openLocationHandler(QModelIndex &index)
{
    QString folderPath;

    if(!index.isValid()){
        folderPath = this->rootPath();
    }else{
        QFileInfo fileInfo = this->fileInfo(index);
        if (!fileInfo.isDir()) {
            fileInfo.setFile(fileInfo.absoluteFilePath());
        }
        folderPath = fileInfo.absolutePath();
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
}

void NavigationModel::uniqueFileName(QFile &file, QString &name, QString &type, const QString &path)
{
    file.setFileName(path+name+type);
    if (file.exists()){
        name = name+"_new";
        uniqueFileName(file,name,type,path);
    }
}

void NavigationModel::uniqueFolderName(QDir &dir, QString &name, const QString &path)
{
    dir.setPath(path+name);
    if (dir.exists()){
        name = name+"_new";
        uniqueFolderName(dir,name,path);
    }
}

QModelIndex NavivationProxyModel::setRootIndexFromPath(QString path)
{
    QFileSystemModel * model =  dynamic_cast<QFileSystemModel*>(this->sourceModel());
    if(model) {
        QModelIndex result = this->mapFromSource(model->index(path));
        return result;
    }
    return QModelIndex();
}

bool NavivationProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QFileSystemModel * model =  qobject_cast<QFileSystemModel*>(this->sourceModel());
    QModelIndex childIndex = model->index(source_row,0,source_parent);

    if(filterRegularExpression().pattern() == ""){
        return true;
    }

    if(!model) {
        return false;
    }

    QString rootPath = model->rootPath()+"//";
    QString infoPath = model->fileInfo(childIndex).absoluteFilePath();

    if(!isSubdirectory(infoPath,rootPath)){
        return true;
    }

    return processChildIndex( model, source_row, childIndex);
}

bool NavivationProxyModel::processChildIndex( QFileSystemModel* model, int source_row, const QModelIndex &source_parent) const
{
    QFileInfo info = model->fileInfo(source_parent);
    QString fileName = info.fileName();

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
                if(processChildIndex( model, row, childIndex)){
                    return true;
                }
            }
        }
    }
    return false;
}

bool NavivationProxyModel::isSubdirectory(const QString &subDirPath, const QString &parentDirPath) const
{
    QDir subDir(subDirPath);
    QDir parentDir(parentDirPath);

    QString absoluteSubDirPath = subDir.absolutePath();
    QString absoluteParentDirPath = parentDir.absolutePath();

    return absoluteSubDirPath.startsWith(absoluteParentDirPath);
}
