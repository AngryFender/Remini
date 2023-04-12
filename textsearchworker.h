
#ifndef TEXTSEARCHWORKER_H
#define TEXTSEARCHWORKER_H


#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QTextCursor>
#include <QStandardItemModel>


class TextSearchWorker : public QObject
{
    Q_OBJECT
public:
    explicit TextSearchWorker(QObject *parent = nullptr);
    QStringList &getListPaths();
    void setText(QString &text);

public slots:
    void doWork();
private:
    QStringList listPaths;
    QString text;
    QList<int> listPositions;
    QStandardItemModel model;

    void findAllMatches(bool &appendItem,QStandardItem *item, int &row, QList<int> &list, QTextDocument &document, int startPosition, QString &text);
    QString extractNeighbourWords(QTextDocument &document, int position);

signals:
    void finished();
    void updateTextSearchView(QStandardItemModel *model);
};

#endif // TEXTSEARCHWORKER_H
