#include "searchalldialog.h"

SearchAllDialog::SearchAllDialog(QWidget *parent):QDialog(parent)
{
    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    textView = new QLineEdit(this);
    textView->setPlaceholderText("Search Texts");
    matchCountView = new QLabel(this);
    matchCountView->setAlignment(Qt::AlignRight);
    searchView = new NavigationView(this);


    layout->addWidget(textView);
    layout->addWidget(matchCountView,Qt::AlignRight);
    layout->addWidget(searchView);

    keyPressTimer.setInterval(KEY_PRESS_TIMEOUT);

    connect(searchView,SIGNAL(pressed(QModelIndex)),
            this,SLOT(textSearchPositionSelected(QModelIndex)));

    connect(&keyPressTimer,SIGNAL(timeout()),
            this,SLOT(keyPressTimeoutHandle()));

    connect(textView,SIGNAL(textChanged(QString)),
            this,SLOT(textSearchChanged(QString)));


}

SearchAllDialog::~SearchAllDialog(){
    delete layout;
    delete textView;
}

void SearchAllDialog::show()
{
    textView->setFocus();
    QWidget::show();
}

void SearchAllDialog::updateTextSearchViewHandle(QStandardItemModel *model,int matchCount)
{
    matchCountView->setText(QString::number(matchCount)+" matches found");

    searchView->setModel(model);
    if(model->rowCount()<=2)
        searchView->expandAll();
}

void SearchAllDialog::textSearchPositionSelected(const QModelIndex &index)
{
    QModelIndex parentIndex = searchView->model()->parent(index);
    QString filePath = parentIndex.data(Qt::UserRole).toString();
    int searchTextLength = index.data(Qt::UserRole).toInt();
    int blockNumber = index.data(Qt::UserRole+1).toInt();
    int positionInBlock = index.data(Qt::UserRole+2).toInt();


    emit showSearchedTextInFile(filePath, searchTextLength, blockNumber,positionInBlock);
}

void SearchAllDialog::textSearchChanged(const QString &text)
{
    keyPressTimer.start();
}

void SearchAllDialog::keyPressTimeoutHandle()
{
    keyPressTimer.stop();
    QString text = textView->text();
    emit startSearch(text);
}
