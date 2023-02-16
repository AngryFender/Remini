#ifndef MKEDIT_H
#define MKEDIT_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <Highlighter.h>
#include <QRegularExpressionMatch>
#include <QTextDocument>
#include <QRubberBand>
#include <QMutex>
#include <mktextdocument.h>

#define TEXT_X 0
#define TEXT_Y 0
#define TEXT_SIZE 10
#define PADDING 10

#define BLOCKRADIUS 6
#define CODE_SIZE 10


class MkEdit : public QTextEdit
{
    Q_OBJECT
public:
    MkEdit(QWidget *parent = nullptr):QTextEdit(parent){

        setStyleSheet("QTextEdit { padding-left:10; padding-top:10; padding-bottom:10; padding-right:10}");
        fText.setPointSize(TEXT_SIZE);

        setTabStopDistance(20);
        regexCodeBlock.setPattern("^```+.*");
        regexStartBlock.setPattern("```[a-zA-Z0-9]+");
        widthCodeBlock = this->width() - this->width()-10-PADDING;
        heightCodeBlock = this->height();

        penCodeBlock.setWidthF(0.5);
        penCodeBlock.setStyle(Qt::SolidLine);
        penCodeBlock.setColor(QColor(194,201,207));

        QObject::connect(this,SIGNAL(cursorPositionChanged()),
                         this, SLOT(cursorPositionChangedHandle()));

    }
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QRegularExpression regexNumbering;
    QRegularExpression regexCodeBlock;
    QRegularExpression regexStartBlock;
    QFont fText;
    int widthCodeBlock;
    int heightCodeBlock;
    QPen penCodeBlock;
    int savedBlockNumber;

    void quoteLeftKey();

 public slots:
    void cursorPositionChangedHandle();

 public:
signals:
    void cursorPosChanged(bool hasSelection, int blockNumber );
    void fileSave();
    void enterKeyPressed(int blockNumber);
    void quoteLeftKeyPressed(int blockNumber,bool &success);

    void removeAllMkData();
    void applyAllMkData(bool hasSelection, int blockNumber);
};

#endif // MKEDIT_H
