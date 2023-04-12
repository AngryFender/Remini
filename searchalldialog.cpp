#include "searchalldialog.h"

SearchAllDialog::SearchAllDialog(QWidget *parent):QDialog(parent)
{
    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    textView = new QLineEdit(this);
    textView->setPlaceholderText("Search Texts");
    searchView = new QTreeView(this);

    layout->addWidget(textView);
    layout->addWidget(searchView);

}

SearchAllDialog::~SearchAllDialog(){
    delete layout;
    delete textView;
}

void SearchAllDialog::updateTextSearchViewHandle(QStandardItemModel *model)
{
    searchView->setModel(model);
}

void SearchAllDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter||
        event->key() == Qt::Key_Return){

        QString text = textView->text();
        emit startSearch(text);
    }
    QDialog::keyPressEvent(event);
}
