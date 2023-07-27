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
#include <QDesktopServices>
#include <blockdata.h>
#include <linedata.h>
#include <formatdata.h>
#include <QAbstractTextDocumentLayout>
#include <QQueue>
#include <QMutex>


class FormatCollection{
public:
    FormatCollection(int fontSize){

        linkColor.setRgb(51,102,204);

        bold.setFontWeight(QFont::ExtraBold);
        italic.setFontItalic(true);
        strikethrough.setFontStrikeOut(true);
        heading1.setFontPointSize(fontSize *2);
        heading2.setFontPointSize(fontSize *1.5);
        heading3.setFontPointSize(fontSize *1.25);
        link.setFontUnderline(true);
        link.setUnderlineColor(linkColor);
        link.setForeground(linkColor);
    }

    QTextCharFormat * getBold(){return &bold;};
    QTextCharFormat * getItalic (){return &italic;};
    QTextCharFormat * getStrikethrough(){return &strikethrough;};
    QTextCharFormat * getHeading1(){return &heading1;};
    QTextCharFormat * getHeading2(){return &heading2;};
    QTextCharFormat * getHeading3(){return &heading3;};
    QTextCharFormat * getLink(){return &link;};
    QColor linkColor;

private:
    QTextCharFormat bold;
    QTextCharFormat italic;
    QTextCharFormat strikethrough;
    QTextCharFormat heading1;
    QTextCharFormat heading2;
    QTextCharFormat heading3;
    QTextCharFormat link;
};


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

    QVector<QPair<int, int>>::const_iterator linkPosBegin(){return linkPositions.cbegin();};
    QVector<QPair<int, int>>::const_iterator linkPosEnd(){return linkPositions.cend();};

public slots:
    void cursorPosChangedHandle(bool hasSelection, int blockNumber,QRect rect, int selectionStart, int selectionEnd);
    void removeAllMkDataHandle(int blockNo);
    void applyAllMkDataHandle(bool hasSelection, int blockNumber, bool showAll, QRect rect);
    void enterKeyPressedHandle(int blockNumber);
    void quoteLeftKeyPressedHandle(int blockNumber,bool &success);
    void checkRightClockOnCodeBlockHandle(int blockNumber, bool &valid);
    void selectBlockCopyHandle(int blockNumber, int &startPos, int &endPos);
    void duplicateLineHandle(int blockNumber);
    void smartSelectionHandle(int blockNumber, QTextCursor &cursor);
    void drawTextBlocksHandler(bool hasSelection, int blockNumber, bool showAll, QRect rect);

    void showMKSymbolsFromSavedBlocks(QRect *rect = nullptr, int cursorBlockNo = 0);


    void pushCheckBoxHandle(const int position);
    void pushLinkHandle(const int position);
    void autoInsertSymobolHandle(const int position);
    void setMarkdownHandle(bool state, QRect rect);

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

    struct SelectRange{
        int start = -1;
        int end = -1;
    };

    QTextDocument originalTexts;
    QTextDocument formattedTexts;

    SelectRange selectRange;
    int selectStart;
    int selectEnd;
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
    QVector<QPair<int, int>> linkPositions;

    QColor linkColor;
    bool disableMarkdownState;


    void resetFormatLocation();
    void identifyUserData(bool showAll, bool hasSelection = false);
    void formatAllLines(const QTextDocument &original, MkTextDocument &formatted);

    void identifyFormatData(QTextBlock &block, bool showAll, bool hasSelection = false);
    void insertHeadingData( const QString &text, int &index1, FormatData *formatData);
    void insertFormatData(FormatLocation &loc, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test);
    void insertFormatCheckBoxData(FormatLocation &loc, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test);
    void insertFormatLinkData(FormatLocation &locTitle, FormatLocation &locLink, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test, QString * linkText);
    void incrementIndexes(int &index1, int &index2, int &index3,const int size =1);
    bool convertCharacterToSymbol(const QChar &single, QString &text);
    void convertCharacterToCheckboxSymbol(const QChar &single, QString &text);
    bool convertCharacterToLinkSymbol(const QChar &single, QString &text);
    void composeSymbolCombination(int length, const QString &text, int &index1, int &index2, int &index3, QString &result);

    void setCodeBlockMargin(QTextBlock &block, int leftMargin=0,int rightMargin =0, int topMargin = 0);
    void stripUserData();

    void resetTextBlockFormat(int blockNumber);
    void resetTextBlockFormat(QTextBlock block);
    void applyMkFormat(QTextBlock &block, int start, int end, FragmentData::FormatSymbol status, QTextCursor &cursor,FormatCollection &formatCollection);
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
