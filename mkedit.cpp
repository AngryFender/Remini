#include "mkedit.h"

void MkEdit::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());

    //draw Title
    painter.setFont(fTitle);
    painter.drawText(pTitle,title);

    //draw Texts
    painter.setFont(fText);
    rText.setHeight(this->size().height()-TEXT_Y-TITLE_Y-15);
    rText.setWidth(this->size().width()-TEXT_X);
    painter.drawText(rText,Qt::TextWrapAnywhere,this->toPlainText(),&rText);

}

void MkEdit::setTitle(QString title)
{
    this->title = title;
}
