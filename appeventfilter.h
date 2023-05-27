
#ifndef APPEVENTFILTER_H
#define APPEVENTFILTER_H


#include <QObject>
#include <QKeyEvent>


class AppEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit AppEventFilter(QObject *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *event)override;

signals:
    void KeyPressAlt(bool press);       //true for press, false for release
    void viewChosen(Qt::Key key);

private:
    bool altPressed;

};

#endif // APPEVENTFILTER_H
