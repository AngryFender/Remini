#ifndef TRANSPARENTDIALOG_H
#define TRANSPARENTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QObject>
#include <QPushButton>

class TransparentDialog : public QDialog
{
public:
    TransparentDialog(QWidget *parent, const QString &text);
private:
    QLabel lbl;
};

#endif // TRANSPARENTDIALOG_H
