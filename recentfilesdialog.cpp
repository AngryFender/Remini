#include "recentfilesdialog.h"

RecentFilesDialog::RecentFilesDialog(QWidget * parent):QDialog(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint |Qt::ToolTip);
}
