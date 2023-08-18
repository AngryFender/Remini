#ifndef RECENTFILESDIALOG_H
#define RECENTFILESDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QFileIconProvider>

class RecentFilesDialog : public QDialog
{
    Q_OBJECT

    QVBoxLayout * layout;
    QListWidget *listWidget;
public:
    RecentFilesDialog(QWidget*parent = nullptr);
    void show();
};

#endif // RECENTFILESDIALOG_H
