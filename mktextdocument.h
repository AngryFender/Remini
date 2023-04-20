#ifndef MKTEXTDOCUMENT_H
#define MKTEXTDOCUMENT_H

#include <QTextDocument>
#include <QTextBlock>
#include <QObject>
#include <QRegularExpressionMatchIterator>
#include <QRegularExpressionMatch>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <blockdata.h>
#include <linedata.h>
#include <formatdata.h>
#include <QAbstractTextDocumentLayout>
#include <QQueue>

class MkTextDocument : public QTextDocument
{
    Q_OBJECT
public:
    explicit MkTextDocument(QObject *parent = nullptr);
    void setPlainText(const QString &text);
    void setUndoRedoText(const QString &text);
    void clear() override;

public slots:
    void cursorPosChangedHandle(bool hasSelection, int blockNumber,QRect rect);
    void removeAllMkDataHandle();
    void applyAllMkDataHandle(bool hasSelection, int blockNumber, bool showAll, QRect rect);
    void enterKeyPressedHandle(int blockNumber);
    void quoteLeftKeyPressedHandle(int blockNumber,bool &success);
    void checkRightClockOnCodeBlockHandle(int blockNumber, bool &valid);
    void selectBlockCopyHandle(int blockNumber, int &startPos, int &endPos);
    void duplicateLineHandle(int blockNumber);
    void smartSelectionHandle(int blockNumber, QTextCursor &cursor);
    void drawTextBlocksHandler(bool hasSelection, int blockNumber, bool showAll, QRect rect);

    void showMKSymbolsFromSavedBlocks(QRect *rect = nullptr);
    void hideMKSymbolsFromDrawingRect(QRect rect,bool hasSelection, int blockNumber, bool showAll);

 private:
    struct CheckBlock{
        int start = 0;
        int end = 0;
    };

    struct CheckingBlock{
        QTextBlock start;
        QTextBlock end;
    };

    struct FormatLocation{
        int start = -1;
        int end =-1;
        QString symbol;

        void reset(){
            start = -1;
            end =-1;
        }
    };
    FormatLocation locBoldA;
    FormatLocation locBoldU;
    FormatLocation locItalicA;
    FormatLocation locItalicU;
    FormatLocation locStrike;
    FormatLocation locCheck;

    QRegularExpression regexCodeBlock;
    QRegularExpression regexHorizontalLine;
    QRegularExpression regexNumbering;
    QQueue<QTextBlock> savedBlocks;

    void resetFormatLocation();
    void identifyUserData(bool showAll, int blockNumber=0, bool hasSelection = false);

    void identifyFormatData(QTextBlock &block, bool showAll, int blockNumber=0, bool hasSelection = false);
    QString convertCharacterToSymbol(QChar single);
    QString convertCharacterToCheckboxSymbol(QChar single);
    QString composeSymbol(QString &text, int &index1, int &index2, int &index3);
    void setCodeBlockMargin(QTextBlock &block, QTextBlockFormat &blockFormat, int leftMargin,int rightMargin, int topMargin = 0);
    void stripUserData();

    void updateSingleBlockMkFormat(int blockNumber);
    void applyMkFormat(QTextBlock &block, int start, int end, FragmentData::FormatSymbol status);
    void hideSymbols(QTextBlock block,const QString &symbol);
    void hideSymbolsAtPos(QTextBlock &block,int pos, const QString &symbol);
    void hideSymbolsAtPos(QString &text, int pos, const QString &symbol);

    void showSymbols(QTextBlock block,const QString &symbol);
    void showSymbolsAtPos(QTextBlock &block,int pos, const QString &symbol);
    void showSymbolsAtPos(QString &text, int pos, const QString &symbol);

    void autoCompleteCodeBlock(int blockNumber,bool &success);
    BlockData* checkValidCodeBlock(QTextBlock &block);

    void numberListDetect(int blockNumber);
    int numberListGetSpaces(const QString &text);
    QString numberListGetNextNumber(const QString &text);

signals:
    void clearUndoStack();


};

#endif // MKTEXTDOCUMENT_H
