#ifndef MKEDIT_H
#define MKEDIT_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <Highlighter.h>
#include <QRegularExpressionMatch>
#include <QTextDocument>
#include <QRubberBand>
#include <codebox.h>
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

        //highlighter.setDocument(this->document());

        setTabStopDistance(20);
        regexCodeBlock.setPattern("^```+.*");
        regexStartBlock.setPattern("```[a-zA-Z0-9]+");
        widthCodeBlock = this->width() - this->width()-10-PADDING;
        heightCodeBlock = this->height();

        penCodeBlock.setWidthF(0.5);
        penCodeBlock.setStyle(Qt::SolidLine);
        penCodeBlock.setColor(QColor(194,201,207));

        QObject::connect(this,SIGNAL(cursorPositionChanged()),
                         this, SLOT(cursorPositionChanged()));

    }
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);


    void numberListDetect();
    int numberListGetSpaces(const QString &text);
    QString numberListGetNextNumber(const QString &text);

    void codeBockDetect();

private:
    QRegularExpression regexNumbering;
    QRegularExpression regexCodeBlock;
    QRegularExpression regexStartBlock;
    //Highlighter highlighter;
    QFont fText;
    int widthCodeBlock;
    int heightCodeBlock;
    QPen penCodeBlock;
    int positionStartBlock;
    int positionEndBlock;
    QRubberBand *rb;

    int savedBlockNumber;
    MkTextDocument defaultDoc;
    QMutex mutex;
 public slots:
    void cursorPositionChanged();

public:
signals:

    void keyEnterPressed(int blockNumber);
    void sendUpdateMkGui( QTextDocument *doc, int blockNumber);


};

#endif // MKEDIT_H
