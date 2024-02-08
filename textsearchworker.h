
#ifndef TEXTSEARCHWORKER_H
#define TEXTSEARCHWORKER_H


#include <QObject>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QTextDocument>
#include <QTextCursor>
#include <QStandardItemModel>
#include <QFileIconProvider>


class TextSearchWorker : public QObject
{
    Q_OBJECT
public:
    explicit TextSearchWorker(QObject *parent = nullptr);
    QStringList &getListPaths();
    void setText(const QString &text);
    void setRootPath(QString text);

public slots:
    void doWork();
private:
    QStringList listPaths;
    QString text;
    QString rootPath;
    QStandardItemModel model;
    QFileIconProvider iconProvider;

    void findAllMatches(int &matchCount,QStandardItem *item, int &row, QTextDocument &document, int startPosition, QString &text);
    QString extractNeighbourWords(QTextDocument &document, int position);

signals:
    void finished();
    void updateTextSearchView(QStandardItemModel *model, int matchCount);
};

#endif // TEXTSEARCHWORKER_H
