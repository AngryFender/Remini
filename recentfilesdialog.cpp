#include "recentfilesdialog.h"

RecentFilesDialog::RecentFilesDialog(QWidget*parent, QListWidget * listWidget):QDialog(parent)
{
    listWidget->setParent(this);
    layout = new QVBoxLayout(this);
    this->setWindowFlags(Qt::FramelessWindowHint |Qt::ToolTip);

    this->listWidget = listWidget;
    layout->addWidget(listWidget);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

void RecentFilesDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Tab){
        QListWidgetItem *item;
        int row = listWidget->currentRow();

        if(++row < listWidget->count()){
            item = listWidget->item(row);
        }else{
            item = listWidget->item(0);
        }

        listWidget->setCurrentItem(item);
        currentPath = item->text();
    }
    QDialog::keyPressEvent(event);
}

void RecentFilesDialog::show()
{
    QDialog::show();
    listWidget->clearSelection();
    listWidget->setFocusPolicy(Qt::StrongFocus);
    if(listWidget->count() ==0){

    }else if(listWidget->count()==1){
        listWidget->setCurrentRow(0,QItemSelectionModel::Select);
    }else{
        listWidget->setCurrentRow(1,QItemSelectionModel::Select);
    }
    listWidget->activateWindow();
    listWidget->setFocus();

}

const QString RecentFilesDialog::getCurrentRelativeFile() const
{
    if(listWidget->count() ==0){
        return "";
    }
    return listWidget->currentItem()->text();
}

void RecentFilesDialog::updateRecentFileHandle(const QString &relativePath)
{
    QListWidgetItem *newItem = new QListWidgetItem;
//    QListWidgetItem *nextItem;
    QFileIconProvider iconProvider;
    newItem->setIcon(iconProvider.icon(QFileIconProvider::File));
    newItem->setText(relativePath);

    for(int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);

        if(item->text() == relativePath){
            delete listWidget->takeItem(i);
        }
    }
    listWidget->clearSelection();
    listWidget->insertItem(0,newItem);
//    if(listWidget->count() <= 1){
//        listWidget->setCurrentItem(newItem);
//        listWidget->setCurrentRow(1);
//    }else{
//        listWidget->setCurrentRow(1);
//    }
}
