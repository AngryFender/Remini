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
public:
    RecentFilesDialog(QWidget*parent = nullptr);

    QVBoxLayout * layout;
};

#endif // RECENTFILESDIALOG_H
