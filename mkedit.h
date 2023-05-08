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
#include <QUndoStack>
#include <editcommand.h>
#include <theme.h>

#define FILE_SAVE_TIMEOUT 300
#define BLOCKRADIUS 3
#define MAXIMUM_FONT_SIZE 40
#define MINIMUM_FONT_SIZE 8

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

public:
    MkEdit(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event)override;
    void resizeEvent(QResizeEvent *event)override;
    void wheelEvent(QWheelEvent *e)override;
    QColor blockColor() const;
    void blockColor(const QColor& color);

    QColor getTypeColor() const;
    QColor getMethodColor() const;
    QColor getArgumentColor() const;
    QColor getCommentColor() const;
    QColor getQuoteColor() const;
    QColor getKeywordColor() const;
    void setTypeColor(const QColor& color);
    void setMethodColor(const QColor& color);
    void setArgumentColor(const QColor& color);
    void setCommentColor(const QColor& color);
    void setQuoteColor(const QColor& color);
    void setKeywordColor(const QColor& color);

protected:
    void insertFromMimeData(const QMimeData *source) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;

 private:
    QColor codeBlockColor;
    int widthCodeBlock;
    QPen penCodeBlock;
    int savedBlockNumber;
    QUndoStack undoStack;
    UndoData undoData;
    HighlightColor syntaxColor;
    QTimer fileSaveTimer;

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
    QPoint contextMenuPos;

    QRegularExpression regexUrl;
    QRegularExpressionMatch match;

    QRect getVisibleRect();
    void clearMkEffects();
    void applyMkEffects(const bool scroll = true);
    void fileSaveNow();
    void fileSaveWithScroll(const bool scroll = true);

    bool isMouseOnCheckBox(QMouseEvent *e);

 public slots:
    void contextMenuHandler(QPoint pos);
    void undoContextMenu();
    void redoContextMenu();
    void deleteContextMenu();
    void selectBlock();
    void cursorPositionChangedHandle();
    void undo();
    void redo();
    void clearUndoStackHandle();
    void scrollValueUpdateHandle(int value);

private slots:
    void fileSaveHandle();

signals:
    void cursorPosChanged(bool hasSelection, int blockNumber, QRect rect);
    void fileSave();
    void enterKeyPressed(int blockNumber);
    void quoteLeftKeyPressed(int blockNumber,bool &success);
    void checkRightClockOnCodeBlock(int blockNumber, bool &valid);
    void selectBlockCopy(int blockNumber, int &startPos, int &endPos);
    void duplicateLine(int blockNumber);
    void smartSelection(int blockNumber, QTextCursor &cursor);
    void drawTextBlocks(bool hasSelection, int blockNumber, bool showAll, QRect rect);

    void removeAllMkData();
    void applyAllMkData(bool hasSelection, int blockNumber, bool showAll, QRect rect);
    void blockColorChanged(const QColor& color);
    void syntaxColorUpdate(HighlightColor& colors);

    void typeColorChanged(const QColor &color);
    void methodColorChanged(const QColor &color);
    void argumentColorChanged(const QColor &color);
    void commentColorChanged(const QColor &color);
    void quoteColorChanged(const QColor &color);
    void keywordColorChanged(const QColor &color);

    void pushCheckBox(int position);
    void pushLink(int position);
    void autoInsertSymbol(const int position);
};

#endif // MKEDIT_H
