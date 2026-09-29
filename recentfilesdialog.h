#ifndef RECENTFILESDIALOG_H
#define RECENTFILESDIALOG_H

#include <QDialog>
#include <QString>

class QListWidget;
class QListWidgetItem;
class QVBoxLayout;
class QKeyEvent;

class RecentFilesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RecentFilesDialog(QWidget *parent = nullptr,
                               QListWidget *listWidget = nullptr);
    ~RecentFilesDialog() override = default;

    const QString getCurrentRelativeFile() const;

public slots:
    void updateRecentFileHandle(const QString &relativePath);
    void removeRecentDeletedFileHandle(const QString &relativePath);

protected:
    void keyPressEvent(QKeyEvent *event) override;

public:
    void show();

private:
    QVBoxLayout *layout = nullptr;
    QListWidget *listWidget = nullptr;
    QString currentPath;
};

#endif  // RECENTFILESDIALOG_H
