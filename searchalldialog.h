#ifndef SEARCHALLDIALOG_H
#define SEARCHALLDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTreeView>
#include <QObject>
#include <QVBoxLayout>
#include <QWidget>

class SearchAllDialog : public QDialog
{
    Q_OBJECT
public:
    SearchAllDialog(QWidget*parent);
    ~SearchAllDialog();
private:
    QVBoxLayout * layout;
    QLineEdit * textView;
    QTreeView* searchView;
};

#endif // SEARCHALLDIALOG_H
