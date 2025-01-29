#include "togglebutton.h"
#include <QPainter>

ToggleButton::ToggleButton(QWidget *parent):QAbstractButton(parent), animation(new QPropertyAnimation(this,"pos",this))
{
    state = false;
    animation->setDuration(200);
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor("#2492ff"));
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
    state = !state;
}

void ToggleButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    state = !state;
    QWidget::mouseDoubleClickEvent(event);
}

void ToggleButton::mouseReleaseEvent(QMouseEvent *e)
{

}

void ToggleButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(state? brush:Qt::gray);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawRoundedRect(QRect(5,height()/2,width()-2* 3, height()/3), 4,5);

    p.setBrush(circle);
    p.setBrush(state? brush:Qt::gray);
    p.setRenderHint(QPainter::Antialiasing, true);

    if(state){
        p.drawEllipse(QPoint(23,height()/3*2),7,7);
    }else{
        p.drawEllipse(QPoint(12,height()/3*2),7,7);
    }



}
