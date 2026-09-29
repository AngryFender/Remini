#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QAbstractButton>
#include <QBrush>

class ToggleButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit ToggleButton(QWidget *parent = nullptr);

    bool getState() const;

public slots:
    void setChecked(bool checked);

signals:
    void toggleState(bool state);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QBrush trackEdit;
    QBrush trackReadonly;
    QBrush circleEdit;
    QBrush circleReadonly;
};

#endif  // TOGGLEBUTTON_H
