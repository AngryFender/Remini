#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QAbstractButton>
#include <QObject>
#include <QWidget>

class ToggleButton : public QAbstractButton
{
public:
    ToggleButton(QWidget * parent);

    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

};

#endif // TOGGLEBUTTON_H
