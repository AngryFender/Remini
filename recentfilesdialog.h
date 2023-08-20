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

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public:
    RecentFilesDialog(QWidget*parent = nullptr);
    void show();
public slots:
    void updateRecentFileHandle(const QString &relativePath);

signals:
    void openFile(const QString &relativePath);
};

#endif // RECENTFILESDIALOG_H
