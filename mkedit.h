#ifndef MKEDIT_H
#define MKEDIT_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <Highlighter.h>
#include <QRegularExpressionMatch>
#include <QTextDocument>
#include <mktextdocument.h>
#include <QScrollBar>
#include <QUndoStack>
#include <editcommand.h>

#define TEXT_X 0
#define TEXT_Y 0
#define PADDING 10
#define BLOCKRADIUS 3

class MkEdit : public QTextEdit
{
    Q_OBJECT
    Q_PROPERTY(QColor blockColor READ blockColor WRITE blockColor NOTIFY blockColorChanged)

public:
    MkEdit(QWidget *parent = nullptr):QTextEdit(parent){

        setTabStopDistance(20);
        regexCodeBlock.setPattern("^```+.*");
        regexStartBlock.setPattern("```[a-zA-Z0-9]+");

        penCodeBlock.setWidthF(1);
        penCodeBlock.setStyle(Qt::SolidLine);

        QObject::connect(this,SIGNAL(cursorPositionChanged()),
                         this, SLOT(cursorPositionChangedHandle()));

        this->setUndoRedoEnabled(false);

    }
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event)override;
    void resizeEvent(QResizeEvent *event)override;
    void wheelEvent(QWheelEvent *e)override;
    QColor blockColor() const;
    void blockColor(const QColor& color);

 private:
    QColor codeBlockColor;
    QRegularExpression regexNumbering;
    QRegularExpression regexCodeBlock;
    QRegularExpression regexStartBlock;
    int widthCodeBlock;
    QPen penCodeBlock;
    int savedBlockNumber;
    QUndoStack undoStack;

    void quoteLeftKey();

 public slots:
    void cursorPositionChangedHandle();
    void undo();
    void redo();
//    void blockColorChangedHandle(const QColor& color);

signals:
    void cursorPosChanged(bool hasSelection, int blockNumber );
    void fileSave();
    void enterKeyPressed(int blockNumber);
    void quoteLeftKeyPressed(int blockNumber,bool &success);

    void removeAllMkData();
    void applyAllMkData(bool hasSelection, int blockNumber);
    void blockColorChanged(const QColor& color);
};

#endif // MKEDIT_H
