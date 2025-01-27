#include "togglebutton.h"
#include <QPainter>

ToggleButton::ToggleButton(QWidget *parent):QAbstractButton(parent), animation(new QPropertyAnimation(this,"pos",this))
{
    animation->setDuration(200);
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColorConstants::Blue);
}

void ToggleButton::keyPressEvent(QKeyEvent *e)
{

}

void ToggleButton::keyReleaseEvent(QKeyEvent *e)
{

}

void ToggleButton::mouseMoveEvent(QMouseEvent *e)
{

}

void ToggleButton::mousePressEvent(QMouseEvent *e)
{

}

void ToggleButton::mouseReleaseEvent(QMouseEvent *e)
{

}

void ToggleButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setBrush(state? brush:Qt::gray);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawRoundedRect(QRect(5,height()/2,width()-2* 3, height()/3), 4,5);

    p.setBrush(circle);
    p.setBrush(state? brush:Qt::gray);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawEllipse(QPoint(12,height()/3*2),7,7);

}
