#ifndef SEARCHALLDIALOG_H
#define SEARCHALLDIALOG_H

#include "navigationview.h"

#include <QDialog>
#include <QLineEdit>
#include <QTreeView>
#include <QObject>
#include <QVBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QStandardItemModel>
#include <QTreeView>
#include <QLabel>

class SearchAllDialog : public QDialog
{
    Q_OBJECT
public:
    SearchAllDialog(QWidget*parent);
    ~SearchAllDialog();
    void show();
public slots:
    void updateTextSearchViewHandle(QStandardItemModel *model, int matchCount);
    void textSearchPositionSelected(const QModelIndex &index);
protected:
    void keyPressEvent(QKeyEvent *event)override;

private:
    QVBoxLayout * layout;
    QLineEdit * textView;
    QLabel *matchCountView;
    NavigationView * searchView;
signals:
    void startSearch(QString &text);
    void showSearchedTextInFile(QString &filePath,int searchTextLength, int blockNumber, int positionInBlock);
};

#endif // SEARCHALLDIALOG_H
