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

#define KEY_PRESS_TIMEOUT 400
#define PADDING 10

class SearchAllDialog : public QDialog
{
    Q_OBJECT
public:
    SearchAllDialog(QWidget*parent);
    ~SearchAllDialog();
    void setGeometry(const QRect &rect);
    void show();
    void setFocusAtSearch();
    void setFocusAtResults();
public slots:
    void updateTextSearchViewHandle(QStandardItemModel *model, int matchCount);
    void textSearchPositionSelected(const QModelIndex &index);
    void textSearchChanged(const QString &text);
    void keyPressTimeoutHandle();

private:
    QVBoxLayout * layout;
    QLineEdit * textView;
    QLabel *matchCountView;
    NavigationView * searchView;
    QTimer keyPressTimer;
signals:
    void startSearch(QString &text);
    void showSearchedTextInFile(QString &filePath,int searchTextLength, int blockNumber, int positionInBlock);
};

#endif // SEARCHALLDIALOG_H
