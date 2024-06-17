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
        heading1.setFontPointSize(fontSize *1.5);
        heading2.setFontPointSize(fontSize *1.30);
        heading3.setFontPointSize(fontSize *1.15);
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
    int startBlock      = NO_SELECTION_POS;
    int endBlock        = NO_SELECTION_POS;
    bool hasSelection 	= false;
    bool oldSelection 	= false;
    bool isCheckBox 	= false;
    int currentposInBlock      = NO_SELECTION_POS;
    int currentBlockNo         = NO_SELECTION_POS;
    int oldBlockNo             = NO_SELECTION_POS;
    int selectionFirstStartBlock = NO_SELECTION_POS;
    int selectionFirstStartPosInBlock = NO_SELECTION_POS;
    int selectionEndBlock 			  = NO_SELECTION_POS;
    int selectionEndPosInBlock        = NO_SELECTION_POS;
    bool isCursorCaculated  = false;
    bool isFirstMousePress  = false;
    int scrollValue;
    int rawFirstBlock 		= NO_SELECTION_POS;
    int rawEndBlock 		= NO_SELECTION_POS;
    int oldRawFirstBlock 	= NO_SELECTION_POS;
    int oldRawEndBlock 		= NO_SELECTION_POS;
};

struct RawBlockInfo{
    bool hasSelection = false;
    int rawFirstBlock  = NO_SELECTION_POS;
    int rawEndBlock    = NO_SELECTION_POS;
};

class MkTextDocument : public QTextDocument
{
    Q_OBJECT
public:
    explicit MkTextDocument(QObject *parent = nullptr);
    void setPlainText(const QString &text);
    void setUndoRedoText(const QString &text);
    void setUndoRedoText(const int blockNo,const QString &text);
    void setUndoSelectRange(const SelectRange range);
    void setRedoSelectRange(const int blockNo, const int posInBlock, const bool isCheckBox, const int scrollValue);
    const SelectRange &getUndoSelectRange() const;
    const SelectRange &getRedoSelectRange() const;
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
    QTextDocument* getRawDocument();

public slots:
    void cursorPosChangedHandle(bool hasSelection, int blockNumber, SelectRange * range);
    void removeAllMkDataHandle(int blockNo);
    void applyAllMkDataHandle(int blockNumber, bool showAll, SelectRange*range);
    void applyMkSingleBlockHandle(int blockNumber, SelectRange*range);
    void enterKeyPressedHandle(int blockNumber);
    void quoteLeftKeyPressedHandle(int blockNumber,bool &success);
    void checkRightClockOnCodeBlockHandle(int blockNumber, bool &valid);
    void selectBlockCopyHandle(int blockNumber, int &startPos, int &endPos);
    void duplicateLineHandle(int blockNumber);
    void smartSelectionHandle(int blockNumber, QTextCursor &cursor);
    void saveRawDocumentHandler();
    void saveSingleRawBlockHandler(int blockNumber);

    void pushCheckBoxHandle(const int position);
    void pushLinkHandle(const int position);
    void autoInsertSymobolHandle(const int position);
    void setMarkdownHandle(bool state);
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

    QTextDocument rawDocument;

    QString filePath;
    QString fileName;
    int blockNo;
    int characterNo;
    SelectRange selectRange;

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
    QRegularExpression regexCheckBox;
    QRegularExpression regexBulletCheckBox;
    QRegularExpression regexBulletPoints;

    QUndoStack undoStack;

    QVector<int> checkMarkPositions;
    QVector<QPair<int, int>> linkPositions;

    QColor linkColor;
    bool disableMarkdownState;

    SelectRange undoSelectRange;
    SelectRange redoSelectRange;

    void resetFormatLocation();
    void identifyUserData();
    void identifyUserData(QTextBlock &block);
    void formatAllLines(const QTextDocument &original, MkTextDocument &formatted);

    void identifyFormatData(QTextBlock &block);
    void insertHeadingData( const QString &text, int &index1, FormatData *formatData);
    void insertFormatData(FormatLocation &loc, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test);
    void insertFormatCheckBoxData(FormatLocation &loc, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test);
    void insertFormatLinkData(FormatLocation &locTitle, FormatLocation &locLink, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test, QString * linkText);
    void incrementIndexes(int &index1, int &index2, int &index3,const int size =1);
    bool convertCharacterToSymbol(const QChar &single, QString &text);
    void convertCharacterToCheckboxSymbol(const QChar &single, QString &text);
    bool convertCharacterToLinkSymbol(const QChar &single, QString &text);
    void composeSymbolCombination(int length, const QString &text, int &index1, int &index2, int &index3, QString &result);
    void composeOnlyLinkSymbolCombination(int length, const QString &text, int &index1, int &index2, int &index3, QString &result);

    void setCodeBlockMargin(QTextBlock &block, int leftMargin=0,int rightMargin =0, int topMargin = 0);
    void stripUserData();

    void resetTextBlockFormat(int blockNumber);
    void resetTextBlockFormat(QTextBlock block);
    void applyMkFormat(QTextBlock &block, int start, int end, FragmentData::FormatSymbol status, QTextCursor &cursor,FormatCollection &formatCollection, bool onlyCheckboxAndLink);
    void hideSymbols(QTextBlock &block,const QString &symbol);
    void hideAllFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData);
    void hideSymbolsAtPos(QString &text, int pos, const QString &symbol);

    void showSymbols(QTextBlock &block,const QString &symbol);
    void showAllFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData, SelectRange * selectRange = nullptr);
    void showSymbolsAtPos(QString &text, int pos, const QString &symbol);

    void extractSymbolsInBlock(QTextBlock &block, QString &result);
    void showFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData, QString &result);

    void autoCompleteCodeBlock(int blockNumber,bool &success);
    BlockData* checkValidCodeBlock(QTextBlock &block);

    void numberListDetect(int blockNumber);
    int numberListGetSpaces(const QString &text);
    QString numberListGetNextNumber(const QString &text);

    void hideMKSymbolsFromDrawingRect(int blockNumber, bool showAll,SelectRange * const editSelectRange, const bool clearPushCheckBoxData = true);
    void hideMKSymbolsFromPreviousSelectedBlocks(SelectRange * const editSelectRange);
    void showMKSymbolsFromCurrentSelectedBlocks(int blockNumber, bool showAll,SelectRange * const editSelectRange, const bool clearPushCheckBoxData = true);

};

enum EditType{
    singleEdit = 0,
    checkbox,
    enterPressed,
    multiEdit,
};

struct UndoData{
    QTextEdit *view;
    QTextDocument *doc;
    QString text;
    int blockNo;
    int posInBlock;
    QString oldText;
    bool undoRedoSkip;
    bool selectAll;
    int scrollValue;
    SelectRange oldSelectRange;
    QString oldBlock;
    QString newBlock;
    EditType editType;
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
    QString oldBlock;
    QString newBlock;
    int blockNo;
    int posInBlock;
    int scrollValue;
    EditType editType;

    QString oldText;
    bool isConstructorRedo;
    SelectRange oldSelectRange;
};


#endif // MKTEXTDOCUMENT_H
