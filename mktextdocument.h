#ifndef MKTEXTDOCUMENT_H
#define MKTEXTDOCUMENT_H

#include <QTextDocument>
#include <QTextBlock>
#include <QObject>
#include <QRegularExpressionMatchIterator>
#include <QRegularExpressionMatch>
#include <QPageRanges>
#include <blockdata.h>
#include <linedata.h>

class MkTextDocument : public QTextDocument
{
    Q_OBJECT
public:
    explicit MkTextDocument(QObject *parent = nullptr);
    void setPlainText(const QString &text);

public slots:
    void cursorPosChangedHandle(bool hasSelection, int blockNumber);
    void removeAllMkDataHandle();
    void applyAllMkDataHandle(bool hasSelection, int blockNumber);
    void enterKeyPressedHandle(int blockNumber);
    void quoteLeftKeyPressedHandle(int blockNumber,bool &success);
    void isUserDataStillValidHandle(int blockNumber);
 private:
    struct CheckBlock{
        int start = 0;
        int end = 0;
    };

    QRegularExpression regexCodeBlock;
    QRegularExpression regexHorizontalLine;
    QRegularExpression regexNumbering;
    QList<QPageRanges> codeBlockPosList;
    int cursorPosition;

    void identifyUserData();
    void stripUserData();

    void showCursoredBlock(int blockNumber, int start, int end, const QString &symbol);
    void hideSymbols(QTextBlock block,const QString &symbol);
    void showAllSymbols();
    void showSymbols(QTextBlock block,const QString &symbol);

    void autoCompleteCodeBlock(int blockNumber,bool &success);

    void numberListDetect(int blockNumber);
    int numberListGetSpaces(const QString &text);
    QString numberListGetNextNumber(const QString &text);

};

#endif // MKTEXTDOCUMENT_H
