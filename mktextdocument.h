#ifndef MKTEXTDOCUMENT_H
#define MKTEXTDOCUMENT_H

#include <QTextDocument>
#include <QTextBlock>
#include <QObject>
#include <QRegularExpressionMatchIterator>
#include <QRegularExpressionMatch>
#include <QPageRanges>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <blockdata.h>
#include <linedata.h>
#include <formatdata.h>

class MkTextDocument : public QTextDocument
{
    Q_OBJECT
public:
    explicit MkTextDocument(QObject *parent = nullptr);
    void setPlainText(const QString &text);
    void setUndoRedoText(const QString &text);


public slots:
    void cursorPosChangedHandle(bool hasSelection, int blockNumber);
    void removeAllMkDataHandle();
    void applyAllMkDataHandle(bool hasSelection, int blockNumber, bool showAll);
    void enterKeyPressedHandle(int blockNumber);
    void quoteLeftKeyPressedHandle(int blockNumber,bool &success);
    void checkRightClockOnCodeBlockHandle(int blockNumber, bool &valid);
    void selectBlockCopyHandle(int blockNumber, int &startPos, int &endPos);
    void duplicateLineHandle(int blockNumber);
    void smartSelectionHandle(int blockNumber, QTextCursor &cursor);

 private:
    struct CheckBlock{
        int start = 0;
        int end = 0;
    };

    QRegularExpression regexCodeBlock;
    QRegularExpression regexHorizontalLine;
    QRegularExpression regexNumbering;
    QRegularExpression regexBoldA;
    QRegularExpression regexBoldU;
    QList<QPageRanges> codeBlockPosList;
    int cursorPosition;

    void identifyUserData(bool showAll);
    void identifyFormatData(QTextBlock &block, bool showAll);
    void setCodeBlockMargin(QTextBlock &block, QTextBlockFormat &blockFormat, int leftMargin,int rightMargin, int topMargin = 0);
    void stripUserData();

    void hideSymbols(QTextBlock block,const QString &symbol);
    void hideSymbolsAtPos(QTextBlock &block,int pos, const QString &symbol);
    void showAllSymbols();
    void showSymbols(QTextBlock block,const QString &symbol);
    void showSymbolsAtPos(QTextBlock &block,int pos, const QString &symbol);

    void autoCompleteCodeBlock(int blockNumber,bool &success);
    BlockData* checkValidCodeBlock(QTextBlock &block);

    void numberListDetect(int blockNumber);
    int numberListGetSpaces(const QString &text);
    QString numberListGetNextNumber(const QString &text);

signals:
    emit void clearUndoStack();


};

#endif // MKTEXTDOCUMENT_H
