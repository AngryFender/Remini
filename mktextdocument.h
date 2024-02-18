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
#include <QTextEdit>
#include <QUndoCommand>
#include <QUndoStack>


#define MAXIMUM_FONT_SIZE 30
#define MINIMUM_FONT_SIZE 7

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

#define NO_SELECTION_POS -1
struct SelectRange{
    int start           = NO_SELECTION_POS;
    int end             = NO_SELECTION_POS;
    bool hasSelection 	= false;
    int currentposInBlock      = NO_SELECTION_POS;
    int currentBlockPos        = NO_SELECTION_POS;
    int currentBlockNo         = NO_SELECTION_POS;
    int blockStart      = NO_SELECTION_POS;
    int posInBlockStart = NO_SELECTION_POS;
    int blockEnd        = NO_SELECTION_POS;
    int posInBlockEnd   = NO_SELECTION_POS;
    int selectionFirstStartPos = NO_SELECTION_POS;
    int selectionFirstStartBlock = NO_SELECTION_POS;
    int selectionFirstStartPosInBlock = NO_SELECTION_POS;
    bool isCursorCaculated  = false;
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

    void setFilePath(const QString &filePath);
    void setFileName(const QString &fileName);
    void setCursorPos(const int blockNo, const int characterNo);
    int getBlockNo()const;
    int getCharacterNo()const;
    QString getFilePath() const;
    QString getFileName() const;

public slots:
    void cursorPosChangedHandle(bool hasSelection, int blockNumber,QRect rect, SelectRange * editSelectRange);
    void removeAllMkDataHandle(int blockNo);
    void applyAllMkDataHandle(bool hasSelection, int blockNumber, bool showAll, QRect rect);
    void enterKeyPressedHandle(int blockNumber);
    void quoteLeftKeyPressedHandle(int blockNumber,bool &success);
    void checkRightClockOnCodeBlockHandle(int blockNumber, bool &valid);
    void selectBlockCopyHandle(int blockNumber, int &startPos, int &endPos);
    void duplicateLineHandle(int blockNumber);
    void smartSelectionHandle(int blockNumber, QTextCursor &cursor);
    void drawTextBlocksHandler(bool hasSelection, int blockNumber, bool showAll, QRect rect, SelectRange* selectRange);

    void showMKSymbolsFromSavedBlocks(QRect *rect = nullptr, int cursorBlockNo = 0);

    void pushCheckBoxHandle(const int position);
    void pushLinkHandle(const int position);
    void autoInsertSymobolHandle(const int position);
    void setMarkdownHandle(bool state, QRect rect);
    void cursorUpdateHandle(const int blockNo, const int characterNo);

    void undoStackPush(QUndoCommand *edit);
    void undoStackUndo();
    void undoStackRedo();

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

    QTextDocument originalTexts;
    QTextDocument formattedTexts;

    QString filePath;
    QString fileName;
    int blockNo;
    int characterNo;
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

    QUndoStack undoStack;

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
    void showAllFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData, SelectRange * selectRange = nullptr);
    void showSymbolsAtPos(QString &text, int pos, const QString &symbol);

    void autoCompleteCodeBlock(int blockNumber,bool &success);
    BlockData* checkValidCodeBlock(QTextBlock &block);

    void numberListDetect(int blockNumber);
    int numberListGetSpaces(const QString &text);
    QString numberListGetNextNumber(const QString &text);

    void hideMKSymbolsFromDrawingRect(QRect rect,bool hasSelection, int blockNumber, bool showAll,SelectRange * const editSelectRange, const bool clearPushCheckBoxData = true);

};

struct UndoData{
    QTextEdit *view;
    QTextDocument *doc;
    QString text;
    int cursorPos;
    QString oldText;
    int oldCursorPos;
    int oldStartSelection;
    int oldEndSelection;
    bool undoRedoSkip;
    bool selectAll;
    int scrollValue;
};

class EditCommand : public QUndoCommand
{
public:
    EditCommand(UndoData &data);

    void undo() override;
    void redo() override;

private:
    QTextEdit *view;
    MkTextDocument *doc;
    QString text;
    int cursorPos;
    int scrollValue;

    QString oldText;
    int oldCursorPos;
    int oldStartSelection;
    int oldEndSelection;
    bool isConstructorRedo;
};


#endif // MKTEXTDOCUMENT_H
