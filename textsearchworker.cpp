
#include "textsearchworker.h"

#include <QElapsedTimer>

TextSearchWorker::TextSearchWorker(QObject *parent)
    : QObject{parent}
{

}

QStringList &TextSearchWorker::getListPaths()
{
    return listPaths;
}

void TextSearchWorker::setText(QString &text)
{
    this->text = text;
}

void TextSearchWorker::doWork(){
    QElapsedTimer timer;
    timer.start();
    listPositions.clear();
    model.clear();
    for(QString& path: listPaths){
        QStandardItem *fileItem = new QStandardItem(path);
//        fileItem->setFlags(Qt::ItemIsSelectable);
        bool appendItem = false;
        fileItem->setData(path,Qt::DisplayRole);
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

        findAllMatches(appendItem, fileItem, childRow, listPositions, document, 0, this->text);

        if(!fileItem->hasChildren()){
            delete fileItem;
        }else{
            model.appendRow(fileItem);
        }
    }
    qDebug()<< ">>>time to search all = "<<timer.elapsed();
    emit updateTextSearchView(&model);
    emit finished();

}

void TextSearchWorker::findAllMatches(bool &appendItem, QStandardItem *parentItem, int &row, QList<int> &list, QTextDocument &document, int startPosition, QString &text){
    QTextCursor foundCursor = document.find(text,startPosition);
    if(foundCursor != QTextCursor()){
        int foundPosition = foundCursor.position();
        list.push_back(foundPosition);
        QString words = "..."+extractNeighbourWords(document, foundPosition)+"...";
        QStandardItem * positionItem = new QStandardItem(foundPosition);
        positionItem->setFlags(Qt::ItemNeverHasChildren | Qt::ItemIsSelectable |Qt::ItemIsEnabled);
        positionItem->setData(words,Qt::DisplayRole);
        parentItem->setChild(row,0,positionItem);
        row++;
        findAllMatches(appendItem, parentItem,row, list, document, foundPosition+1, text);
    }
}

QString TextSearchWorker::extractNeighbourWords(QTextDocument &document, int position)
{
    QTextCursor cursor(&document);
    if(position+10< document.characterCount())
    {
        cursor.setPosition(position+10);
    }else
        cursor.setPosition(position);

    if(position-text.length()-10 >=0){
        cursor.setPosition(position-text.length()-10,QTextCursor::KeepAnchor);
    }
    else
        cursor.setPosition(position-text.length(),QTextCursor::KeepAnchor);

    return cursor.selectedText();
}

