#include "recentfilesdialog.h"

#include <QFileIconProvider>
#include <QKeyEvent>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTimer>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Constructor
//
// Window flags:
//   X11   : ToolTip + FramelessWindowHint gives an override-redirect popup
//           with no decoration and no task-bar entry.
//   Wayland: the protocol has no override-redirect concept; the compositor
//           decides the final presentation. ToolTip still maps to a popup
//           surface, which is the closest available behaviour.
// ---------------------------------------------------------------------------
RecentFilesDialog::RecentFilesDialog(QWidget *parent, QListWidget *listWidget)
    : QDialog(parent)
{
    if (!listWidget)
        listWidget = new QListWidget(this);

    listWidget->setParent(this);

    layout = new QVBoxLayout(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);

    this->listWidget = listWidget;
    layout->addWidget(listWidget);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

// ---------------------------------------------------------------------------
// Key handling: Tab cycles through the recent files.
// Guard against an empty list to avoid dereferencing a null item.
// ---------------------------------------------------------------------------
void RecentFilesDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Tab && listWidget->count() > 0)
    {
        QListWidgetItem *item = nullptr;
        int row = listWidget->currentRow();

        if (++row < listWidget->count())
            item = listWidget->item(row);
        else
            item = listWidget->item(0);

        if (item)
        {
            listWidget->setCurrentItem(item);
            currentPath = item->text();
        }
    }
    QDialog::keyPressEvent(event);
}

// ---------------------------------------------------------------------------
// Show the dialog.
//
// activateWindow() is a no-op on Wayland (focus stealing is forbidden by the
// protocol), so we only rely on setFocus() and defer it to the next event
// loop iteration to make sure the compositor has already granted keyboard
// focus to the new surface.
// ---------------------------------------------------------------------------
void RecentFilesDialog::show()
{
    QDialog::show();

    listWidget->clearSelection();
    listWidget->setFocusPolicy(Qt::StrongFocus);

    if (listWidget->count() == 1)
        listWidget->setCurrentRow(0, QItemSelectionModel::Select);
    else if (listWidget->count() > 1)
        listWidget->setCurrentRow(1, QItemSelectionModel::Select);

    // Defer focus assignment so the compositor has time to map the surface
    // and route keyboard input to it. On X11 this is harmless; on Wayland
    // it makes the difference between focus working and silently failing.
    QTimer::singleShot(0, listWidget,
                       [this]()
                       { listWidget->setFocus(Qt::OtherFocusReason); });
}

const QString RecentFilesDialog::getCurrentRelativeFile() const
{
    if (listWidget->count() == 0)
        return QString();

    QListWidgetItem *item = listWidget->currentItem();
    if (!item)
        return QString();

    return item->text();
}

void RecentFilesDialog::updateRecentFileHandle(const QString &relativePath)
{
    QListWidgetItem *newItem = new QListWidgetItem;
    QFileIconProvider iconProvider;
    newItem->setIcon(iconProvider.icon(QFileIconProvider::File));
    newItem->setText(relativePath);

    for (int i = 0; i < listWidget->count(); ++i)
    {
        QListWidgetItem *item = listWidget->item(i);
        if (item->text() == relativePath)
            delete listWidget->takeItem(i);
    }

    listWidget->clearSelection();
    listWidget->insertItem(0, newItem);
    listWidget->setCurrentRow(0, QItemSelectionModel::Select);
}

void RecentFilesDialog::removeRecentDeletedFileHandle(
    const QString &relativePath)
{
    for (int i = 0; i < listWidget->count(); ++i)
    {
        QListWidgetItem *item = listWidget->item(i);
        if (item->text() == relativePath)
            delete listWidget->takeItem(i);
    }
}
