
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
    void openRecentFiles(bool show);

private:
    bool altPressed;

};

#endif // APPEVENTFILTER_H
