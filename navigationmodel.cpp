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
