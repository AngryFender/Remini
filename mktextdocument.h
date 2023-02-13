#ifndef MKTEXTDOCUMENT_H
#define MKTEXTDOCUMENT_H

#include <QTextDocument>
#include <QTextBlock>
#include <QObject>
#include <QRegularExpressionMatchIterator>
#include <QRegularExpressionMatch>
#include <QPageRanges>
#include <blockdata.h>

class MkTextDocument : public QTextDocument
{
    Q_OBJECT
public:
    explicit MkTextDocument(QObject *parent = nullptr);
    void setPlainText(const QString &text);
public slots:
    void  cursorPosChangedHandle(QTextDocument *doc, int blockNumber);
    void  KeyEnterPressedHandle(int blockNumber);

 private:
    struct CheckBlock{
        int start = 0;
        int end = 0;
    };

    QRegularExpression regexCodeBlock;
    QList<QPageRanges> codeBlockPosList;

    void showCursoredBlock(int blockNumber, int start, int end, const QString &symbol);
    void hideSymbols(QTextBlock block,const QString &symbol);
    void showSymbols(QTextBlock block,const QString &symbol);


};

#endif // MKTEXTDOCUMENT_H
