#include "transparentdialog.h"

#include <QVBoxLayout>

TransparentDialog::TransparentDialog(QWidget *parent, const QString &text)
{
    this->setWindowFlags(Qt::FramelessWindowHint |Qt::ToolTip);
    this->setAttribute(Qt::WA_TranslucentBackground);


    lbl.setAlignment(Qt::AlignCenter);
    lbl.setText(text);
    lbl.setStyleSheet(" background-color: rgba(255, 255, 255, 0.2);"
                       " border-radius: 3px;"
                       " border: 3px solid white;"
                       " font-size: 16px;");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(&lbl);
}

