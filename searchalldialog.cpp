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
