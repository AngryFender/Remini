
#ifndef TEXTSEARCHWORKER_H
#define TEXTSEARCHWORKER_H

#include <QDir>
#include <QFile>
#include <QFileIconProvider>
#include <QObject>
#include <QStandardItemModel>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>

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

    void findAllMatches(int &matchCount, QStandardItem *parentItem, int &row,
                        QTextDocument &document, int startPosition,
                        const QString &searchText);
    QString extractNeighbourWords(QTextDocument &document, int position);

signals:
    void finished();
    void updateTextSearchView(QStandardItemModel *model, int matchCount);
};

#endif  // TEXTSEARCHWORKER_H
