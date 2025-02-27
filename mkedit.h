#ifndef MKEDIT_H
#define MKEDIT_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <Highlighter.h>
#include <QRegularExpressionMatch>
#include <QTextDocument>
#include <QMenu>
#include <QMimeData>
#include <QTimer>
#include <mktextdocument.h>
#include <QScrollBar>
#include <theme.h>

#define FILE_SAVE_TIMEOUT 300
#define BLOCKRADIUS 4

class Connector{
public:
    Connector(std::function<void(bool)> disconnectSignals, std::function<void(bool)> connectSignals):connectSignals(connectSignals){
        disconnectSignals(true);
    }
    ~Connector(){
        connectSignals(true);
    }

private:
    std::function<void(bool)> connectSignals;

};

class MkEdit : public QTextEdit
{
    Q_OBJECT
    Q_PROPERTY(QColor blockColor READ blockColor WRITE blockColor NOTIFY blockColorChanged)
    Q_PROPERTY(QColor typeColor READ getTypeColor WRITE setTypeColor NOTIFY typeColorChanged)
    Q_PROPERTY(QColor methodColor READ getMethodColor WRITE setMethodColor NOTIFY methodColorChanged)
    Q_PROPERTY(QColor argumentColor READ getArgumentColor WRITE setArgumentColor NOTIFY argumentColorChanged)
    Q_PROPERTY(QColor commentColor READ getCommentColor WRITE setCommentColor NOTIFY commentColorChanged)
    Q_PROPERTY(QColor quoteColor READ getQuoteColor WRITE setQuoteColor NOTIFY quoteColorChanged)
    Q_PROPERTY(QColor keywordColor READ getKeywordColor WRITE setKeywordColor NOTIFY keywordColorChanged)
    Q_PROPERTY(QColor searchMatchColor READ getSearchMatchColor WRITE setSearchMatchColor NOTIFY searchMatchColorChanged)

public:
    explicit MkEdit(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event)override;
    void keyReleaseEvent(QKeyEvent *event)override;
    void resizeEvent(QResizeEvent *event)override;
    void wheelEvent(QWheelEvent *e)override;
    QColor blockColor() const;
    void blockColor(const QColor& color);

    QColor getTypeColor() const;
    QColor getMethodColor() const;
    QColor getArgumentColor() const;
    QColor getCommentColor() const;
    QColor getQuoteColor() const;
    QColor getSearchMatchColor() const;
    QColor getKeywordColor() const;
    void setTypeColor(const QColor& color);
    void setMethodColor(const QColor& color);
    void setArgumentColor(const QColor& color);
    void setCommentColor(const QColor& color);
    void setQuoteColor(const QColor& color);
    void setSearchMatchColor(const QColor& color);
    void setKeywordColor(const QColor& color);

    void setDocument(QTextDocument *document);
    QString rawPlainText() const;

    void setFont(const QFont &font);
    void setMkState(bool enable);
protected:
    void insertFromMimeData(const QMimeData *source) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
 private:
    QColor codeBlockColor;
    int widthCodeBlock;
    QBrush brushDefault;
    QPen penCodeBlock;
    QPen whitePen;
    int savedCharacterNumber;
    UndoData undoData;
    EditType undoRedoEditType;

    HighlightColor syntaxColor;
    QTimer fileSaveTimer;
    SelectRange selectRange;
    SelectRange undoRedoSelectRange;
    bool isCalcuatedForStartPos;
    bool isCursorChangedHandleTriggered;
    bool isShiftKeyPressed;

    bool isDisconnectedViaHighPriority;

    void quoteLeftKey();
    void smartSelectionSetup();
    void tabKeyPressed();
    void preUndoSetup();
    void postUndoSetup();

    QAction undoAction;
    QAction redoAction;
    QAction copyTextAction;
    QAction pasteTextAction;
    QAction deleteTextAction;
    QAction selectAllAction;
    QAction selectBlockAction;
    QAction disableMarkdown;
    QAction lineWrapAction;
    QPoint contextMenuPos;

    QRegularExpression regexUrl;
    QRegularExpression regexFolderFile;
    QRegularExpression regexCodeBlock;
    QRegularExpressionMatch matchUrl;
    QRegularExpressionMatch matchFolderFile;
    QRegularExpressionMatch matchCodeBlockRegex;

    QRect getVisibleRect();
    void clearMkEffects(EditType editType = EditType::singleEdit);
    void applyMkEffects(const int blockNumber);

    void updateRawDocument();
    void fileSaveNow();
    void fileSaveWithScroll();
    bool isTextCursorVisible();

    bool isMouseOnCheckBox(QMouseEvent *e);
    void showSelectionAfterUndo();
    void showSelectionAfterRedo();
    void setPreArrowKeys(const bool isShiftPressed, const bool isUpOrDownArrowPressed);
    void setPostArrowKeys(const bool isShiftPressed, const bool isLeftArrowPressed, const bool isUpOrDownArrowPressed);

    void restoreTextCursor(int blockNo, int posInBlock, bool hasSelection);
    void postCursorPosChangedSignal();

public slots:
    void contextMenuHandler(QPoint pos);
    void undoContextMenu();
    void redoContextMenu();
    void deleteContextMenu();
    void selectBlock();
    void cursorPositionChangedHandle();
    void connectSignals(bool override = false);
    void disconnectSignals(bool override = false);
    void setEditState(bool edit = false);

private slots:
    void fileSaveHandle();
    void diableMarkdown_internal();
    void lineWrapHandler();
signals:
    void cursorPosChanged(SelectRange * const selectRange, const bool readOnly);
    void fileSaveRaw();
    void enterKeyPressed(int blockNumber, int &newCursorPos);
    void quoteLeftKeyPressed(int blockNumber,bool &success);
    void checkRightClockOnCodeBlock(int blockNumber, bool &valid);
    void selectBlockCopy(int blockNumber, int &startPos, int &endPos);
    void duplicateLine(int blockNumber);
    void smartSelection(int blockNumber, QTextCursor &cursor);
    void saveRawDocument();
    void saveSingleRawBlock(int blockNumber);
    void saveDoubleRawBlock(int firstBlockNumber, int secondBlockNumber);
    void saveEnterPressedRawBlock(int blockNumber, QString text);

    void removeAllMkData(int currentBlockNo);
    void applyAllMkData(int blockNumber);
    void applyMkSingleBlock(int blockNumber);
    void blockColorChanged(const QColor& color);
    void syntaxColorUpdate(HighlightColor& colors);

    void typeColorChanged(const QColor &color);
    void methodColorChanged(const QColor &color);
    void argumentColorChanged(const QColor &color);
    void commentColorChanged(const QColor &color);
    void quoteColorChanged(const QColor &color);
    void searchMatchColorChanged(const QColor &color);
    void keywordColorChanged(const QColor &color);

    void pushCheckBox(int position);
    void pushLink(int blockNo, int posInBlock);
    void autoInsertSymbol(const int position, int &newPosition);
    void setMarkdownStatus(bool state);
    void cursorUpdate(const int blockNo, const int characterPos);
    void checkIfCursorInBlock(bool &isBlock, QTextCursor &cursor);

    void undoStackPushSignal(QUndoCommand *);
    void undoStackUndoSignal(bool &success);
    void undoStackRedoSignal(bool &success);
    void undoStackClear();
    void escapeFocus(QWidget*view);
};

#endif // MKEDIT_H
