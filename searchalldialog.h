#ifndef SEARCHALLDIALOG_H
#define SEARCHALLDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTreeView>
#include <QObject>
#include <QVBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QStandardItemModel>

class SearchAllDialog : public QDialog
{
    Q_OBJECT
public:
    SearchAllDialog(QWidget*parent);
    ~SearchAllDialog();
public slots:
    void updateTextSearchViewHandle(QStandardItemModel *model);
protected:
    void keyPressEvent(QKeyEvent *event)override;

private:
    QVBoxLayout * layout;
    QLineEdit * textView;
    QTreeView* searchView;
signals:
    void startSearch(QString &text);
};

#endif // SEARCHALLDIALOG_H
