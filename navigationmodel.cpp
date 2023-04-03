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
    QModelIndex subParent = model->index(source_row,0,source_parent);

    if(filterRegularExpression().pattern() == ""){
        return true;
    }

    if(model) {
        QFileInfo info = model->fileInfo(subParent);
        QString fileName = info.fileName();
        qDebug()<<"<<<<<<checking "<< fileName;
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
            if(dir.dirName().contains(filterRegularExpression().pattern())){
                return true;
            }

            if(dir.isEmpty()){
                return false;
            }else{
                dir.setFilter( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot );
                int rowCount = dir.count();
                qDebug()<<"row count = "<<rowCount;
                for(int row = 0; row<rowCount; row++){
                    QModelIndex childIndex = model->index(row,0,subParent);
//                    if(!fileName.isEmpty()){
                        qDebug()<<">>>>>> "<<row<<" child name"<< model->fileInfo(childIndex).fileName();
//                    }
                }

                return true;
            }
        }
    }
    return false;


}

bool NavivationProxyModel::processChildIndex( QFileSystemModel* model, int source_row, const QModelIndex &source_parent) const
{
    QFileInfo info = model->fileInfo(source_parent);
    if(info.isDir()){
        int rowCount = model->rowCount(source_parent);
        bool show = false;
        qDebug()<<info.filePath()<<rowCount;
        for(int row = 0; row < rowCount ; row ++){

            QModelIndex childIndex = model->index(source_row,0,source_parent);
            show = this->processChildIndex( model, row, childIndex);
        }
        return true;
    }else if(info.isFile()){
        if(model->fileInfo(source_parent).fileName().contains(filterRegularExpression())){
            return true;
        }
    }


    return true;
}
