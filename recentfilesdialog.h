#ifndef RECENTFILESDIALOG_H
#define RECENTFILESDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>

class RecentFilesDialog : public QDialog
{
    Q_OBJECT
public:
    RecentFilesDialog(QWidget*parent = nullptr);
};

#endif // RECENTFILESDIALOG_H
