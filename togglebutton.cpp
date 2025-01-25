#include "togglebutton.h"

ToggleButton::ToggleButton(QWidget *parent):QAbstractButton(parent), animation(new QPropertyAnimation(this,"pos",this))
{
    animation->setDuration(200);
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

}
