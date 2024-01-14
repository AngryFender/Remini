#include "searchalldialog.h"
#include "qapplication.h"

SearchAllDialog::SearchAllDialog(QWidget *parent):QDialog(parent)
{
    layout = new QVBoxLayout(this);
    textView = new QLineEdit(this);
    textView->setPlaceholderText("Search Texts");
    matchCountView = new QLabel(this);
    matchCountView->setAlignment(Qt::AlignRight);
    searchView = new NavigationView(this,false);

    layout->addWidget(textView);
    layout->addWidget(matchCountView,Qt::AlignRight);
    layout->addWidget(searchView);

    keyPressTimer.setInterval(KEY_PRESS_TIMEOUT);

    connect(searchView,&NavigationView::pressed,
            this,&SearchAllDialog::textSearchPositionSelected);

    connect(&keyPressTimer,&QTimer::timeout,
            this,&SearchAllDialog::keyPressTimeoutHandle);

    connect(textView,&QLineEdit::textChanged,
            this,&SearchAllDialog::textSearchChanged);


}

SearchAllDialog::~SearchAllDialog(){
    delete layout;
    delete textView;
}

void SearchAllDialog::setGeometry(const QRect &rect)
{
    searchView->setFixedHeight(rect.height()-QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight)-textView->height()-matchCountView->height()-PADDING);
    QDialog::setGeometry(rect);
}

void SearchAllDialog::show()
{
    textView->setFocus();
    QWidget::show();
}

void SearchAllDialog::setFocusAtSearch()
{
    textView->setFocus();
}

void SearchAllDialog::setFocusAtResults()
{
    searchView->setFocus();
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
