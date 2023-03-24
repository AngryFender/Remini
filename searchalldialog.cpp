#include "searchalldialog.h"

SearchAllDialog::SearchAllDialog(QWidget *parent):QDialog(parent)
{
    layout = new QVBoxLayout(this);
    textView = new QLineEdit(this);
    searchView = new QTreeView(this);

    layout->addWidget(textView);
    layout->addWidget(searchView);
}

SearchAllDialog::~SearchAllDialog(){
    delete layout;
    delete textView;
}
