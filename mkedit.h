#ifndef MKEDIT_H
#define MKEDIT_H

#include <QTextEdit>
#include <QObject>
#include <QWidget>
#include <QPainter>


#define TITLE_X 15
#define TITLE_Y 15
#define TITLE_SIZE 14

#define TEXT_X 25
#define TEXT_Y 30
#define TEXT_SIZE 10


class MkEdit : public QTextEdit
{
    Q_OBJECT
public:
    MkEdit(QWidget *parent = nullptr):QTextEdit(parent){
        pTitle.setX(TITLE_X);
        pTitle.setY(TITLE_Y);
        fTitle.setPointSize(TITLE_SIZE);

        rText.setTop(TEXT_Y);
        rText.setLeft(TEXT_X);
        rText.setSize(this->size());
        fText.setPointSize(TEXT_SIZE);
    }

    void paintEvent(QPaintEvent *event);
    void setTitle(QString title);
private:
    QString title;
    QPoint pTitle;
    QRect rTitle;
    QFont fTitle;
    QRect rText;
    QFont fText;
};

#endif // MKEDIT_H
