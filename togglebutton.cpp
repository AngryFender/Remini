#include "togglebutton.h"
#include <QPainter>

ToggleButton::ToggleButton(QWidget *parent):QAbstractButton(parent), animation(new QPropertyAnimation(this,"pos",this))
{
    state = true;
    animation->setDuration(200);

    trackEdit.setStyle(Qt::SolidPattern);
    trackEdit.setColor("#b3d9ff");

    trackReadonly.setStyle(Qt::SolidPattern);
    trackReadonly.setColor("#a0a0a4");

    circleEdit.setStyle(Qt::SolidPattern);
    circleEdit.setColor("#2492ff");

    circleReadonly.setStyle(Qt::SolidPattern);
    circleReadonly.setColor("#eaeaf0");

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
    emit toggleState(state);
    this->update();
}

void ToggleButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    state = !state;
    emit toggleState(state);
    this->update();
}

void ToggleButton::mouseReleaseEvent(QMouseEvent *e)
{

}

void ToggleButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(state? trackEdit: trackReadonly);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawRoundedRect(QRect(5,height()/2,width()-2* 3, height()/3), 4,5);

    p.setBrush(state? circleEdit: circleReadonly);
    p.setRenderHint(QPainter::Antialiasing, true);

    if(state){
        p.drawEllipse(QPoint(23,height()/3*2),7,7);
    }else{
        p.drawEllipse(QPoint(12,height()/3*2),7,7);
    }



}
