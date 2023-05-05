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
#include <QMutex>

class MkTextDocument : public QTextDocument
{
    Q_OBJECT
public:
    explicit MkTextDocument(QObject *parent = nullptr);
    void setPlainText(const QString &text);
    void setUndoRedoText(const QString &text);
    void clear() override;

    QVector<int>::const_iterator checkMarkPosBegin(){return checkMarkPositions.cbegin();};
    QVector<int>::const_iterator checkMarkPosEnd(){return checkMarkPositions.cend();};

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


    void pushCheckBoxHandle(const int position);
    void autoInsertSymobolHandle(const int position);

 private:
    struct CheckingBlock{
        QTextBlock start;
        QTextBlock end;
    };

    struct FormatLocation{
        int start = -1;
        int end =-1;

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
    FormatLocation locHeading1;
    FormatLocation locHeading2;
    FormatLocation locHeading3;
    FormatLocation locLink;
    FormatLocation locLinkTitle;

    QRegularExpression regexCodeBlock;
    QRegularExpression regexHorizontalLine;
    QRegularExpression regexNumbering;
    QQueue<QTextBlock> savedBlocks;

    QVector<int> checkMarkPositions;

    void resetFormatLocation();
    void identifyUserData(bool showAll, bool hasSelection = false);

    void identifyFormatData(QTextBlock &block, bool showAll, bool hasSelection = false);
    void insertHeadingData( const QString &text, int &index1, FormatData *formatData);
    void insertFormatData(FormatLocation &loc, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test);
    void insertFormatCheckBoxData(FormatLocation &loc, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test);
    void incrementIndexes(int &index1, int &index2, int &index3,const int size =1);
    bool convertCharacterToSymbol(const QChar &single, QString &text);
    void convertCharacterToCheckboxSymbol(const QChar &single, QString &text);
    void convertCharacterToLinkSymbol(const QChar &single, QString &text);
    void composeSymbolCombination(int length, const QString &text, int &index1, int &index2, int &index3, QString &result);

    void setCodeBlockMargin(QTextBlock &block, int leftMargin=0,int rightMargin =0, int topMargin = 0);
    void stripUserData();

    void resetTextBlockFormat(int blockNumber);
    void resetTextBlockFormat(QTextBlock block);
    void applyMkFormat(QTextBlock &block, int start, int end, FragmentData::FormatSymbol status);
    void hideSymbols(QTextBlock &block,const QString &symbol);
    void hideAllFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData);
    void hideSymbolsAtPos(QString &text, int pos, const QString &symbol);

    void showSymbols(QTextBlock &block,const QString &symbol);
    void showAllFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData);
    void showSymbolsAtPos(QString &text, int pos, const QString &symbol);

    void autoCompleteCodeBlock(int blockNumber,bool &success);
    BlockData* checkValidCodeBlock(QTextBlock &block);

    void numberListDetect(int blockNumber);
    int numberListGetSpaces(const QString &text);
    QString numberListGetNextNumber(const QString &text);

    void hideMKSymbolsFromDrawingRect(QRect rect,bool hasSelection, int blockNumber, bool showAll, const bool clearPushCheckBoxData = true);
signals:
    void clearUndoStack();


};

#endif // MKTEXTDOCUMENT_H
