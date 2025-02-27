#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QAbstractButton>
#include <QObject>
#include <QPropertyAnimation>
#include <QWidget>

class ToggleButton : public QAbstractButton
{
    Q_OBJECT
private:
    bool state;
    QPropertyAnimation *animation = nullptr;

    QBrush trackEdit, circleEdit, trackReadonly, circleReadonly;
    QPoint cirlePoint;
public:
    ToggleButton(QWidget * parent);
    bool getState() const;

    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

signals:
    void toggleState(bool state);

public slots:
    void setChecked(bool state);
};

#endif // TOGGLEBUTTON_H
