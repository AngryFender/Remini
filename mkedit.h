#ifndef MKEDIT_H
#define MKEDIT_H

#include <QTextEdit>
#include <QObject>
#include <QWidget>
#include <QPainter>
#include <Highlighter.h>
#include <QRegularExpressionMatch>
#include <QtGui>

#define TEXT_X 0
#define TEXT_Y 0
#define TEXT_SIZE 10


class MkEdit : public QTextEdit
{
    Q_OBJECT
public:
    MkEdit(QWidget *parent = nullptr):QTextEdit(parent){

        rText.setTop(TEXT_Y);
        rText.setLeft(TEXT_X);
        rText.setSize(this->size());
        fText.setPointSize(TEXT_SIZE);
        highlighter.setDocument(this->document());

        setTabStopDistance(20);
        regexCodeBlock.setPattern("^```$");
        widthCodeBlock = this->width() - this->width()-10;
        heightCodeBlock = this->height();
        penCodeBlock.setColor(Qt::gray);
        penCodeBlock.setWidth(1);
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
    Highlighter highlighter;
    QRect rText;
    QFont fText;
    int widthCodeBlock;
    int heightCodeBlock;
    QPen penCodeBlock;
};

#endif // MKEDIT_H
