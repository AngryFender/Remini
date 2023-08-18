#include "recentfilesdialog.h"

RecentFilesDialog::RecentFilesDialog(QWidget * parent):QDialog(parent)
{
    layout = new QVBoxLayout(this);
    this->setWindowFlags(Qt::FramelessWindowHint |Qt::ToolTip);

    listWidget = new QListWidget(this);
    layout->addWidget(listWidget);

    for(int i = 0; i<7; i++){
    QListWidgetItem *newItem = new QListWidgetItem;
    QFileIconProvider iconProvider;
    newItem->setIcon(iconProvider.icon(QFileIconProvider::File));
    newItem->setText("File"+QString::number(i));
    listWidget->addItem(newItem);
    }
}

void RecentFilesDialog::show()
{

    QDialog::show();
    listWidget->setFocusPolicy(Qt::StrongFocus);
    listWidget->setFocus(Qt::MouseFocusReason);
    listWidget->setCurrentRow(0,QItemSelectionModel::Select);

}
