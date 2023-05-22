#ifndef TRANSPARENTDIALOG_H
#define TRANSPARENTDIALOG_H

#include <QDialog>
#include <QObject>


class TransparentDialog : public QDialog
{
public:
    TransparentDialog(QWidget *parent);

    void setDimension(int x, int y, int width, int height);
};

#endif // TRANSPARENTDIALOG_H
