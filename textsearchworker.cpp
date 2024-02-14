#include "textsearchworker.h"

TextSearchWorker::TextSearchWorker(QObject *parent)
    : QObject{parent}
{

}

QStringList &TextSearchWorker::getListPaths()
{
    return listPaths;
}

void TextSearchWorker::setText(const QString &text)
{
    this->text = text;
}

void TextSearchWorker::setRootPath(QString rootPath)
{
    this->rootPath = rootPath;
}

void TextSearchWorker::doWork(){
    model.clear();
    QDir dir(rootPath);
    int matchCount = 0;

    for(QString& path: listPaths){

        QFileInfo fileInfo(path);
        QIcon fileIcon = iconProvider.icon(fileInfo);
        QString displayPath = dir.relativeFilePath(path);
        QStandardItem *fileItem = new QStandardItem();
        fileItem->setIcon(fileIcon);
        fileItem->setData(path,Qt::UserRole);

        QFile file(path);
        QTextDocument document;
        int childRow = 0;

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            QString content = stream.readAll();
            document.setPlainText(content);
        }
        file.close();

        findAllMatches(matchCount, fileItem, childRow, document, 0, this->text);
        fileItem->setData(displayPath+" ("+QString::number(childRow)+")",Qt::DisplayRole);

        if(!fileItem->hasChildren()){
            delete fileItem;
        }else{
            model.appendRow(fileItem);
        }
    }
    emit updateTextSearchView(&model, matchCount);
    emit finished();

}

void TextSearchWorker::findAllMatches(int &matchCount, QStandardItem *parentItem, int &row, QTextDocument &document, int startPosition, QString &text){
    QTextCursor foundCursor = document.find(text,startPosition);
    if(foundCursor != QTextCursor()){
        int positionInFile = foundCursor.position();
        int blockNumber = foundCursor.blockNumber();
        int positionInBlock = foundCursor.positionInBlock();

        QStandardItem * positionItem = new QStandardItem(positionInFile);
        positionItem->setFlags(Qt::ItemNeverHasChildren | Qt::ItemIsSelectable |Qt::ItemIsEnabled);
        positionItem->setData(extractNeighbourWords(document, positionInFile),Qt::DisplayRole);
        positionItem->setData(text.length(),Qt::UserRole);
        positionItem->setData(blockNumber,Qt::UserRole+1);
        positionItem->setData(positionInBlock,Qt::UserRole+2);
        parentItem->setChild(row,0,positionItem);
        matchCount++;row++;
        findAllMatches(matchCount, parentItem,row, document, positionInFile+1, text);
    }
}

QString TextSearchWorker::extractNeighbourWords(QTextDocument &document, int position)
{
    QTextCursor cursor(&document);
    cursor.setPosition(position, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    QString matchingWords = cursor.selectedText();
    return matchingWords;
}

