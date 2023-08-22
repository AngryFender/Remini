#ifndef RECENTFILESDIALOG_H
#define RECENTFILESDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QFileIconProvider>
#include <QKeyEvent>

class RecentFilesDialog : public QDialog
{
    Q_OBJECT

    QVBoxLayout * layout;
    QListWidget *listWidget;
    QString currentPath;

protected:
    void keyPressEvent(QKeyEvent *event) override;

public:
    RecentFilesDialog(QWidget*parent = nullptr);
    void show();
    const QString getCurrentRelativeFile()const;
public slots:
    void updateRecentFileHandle(const QString &relativePath);
};

#endif // RECENTFILESDIALOG_H
