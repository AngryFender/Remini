// Tests for RecentFilesDialog.
//
// Behaviour of RecentFilesDialog that these tests depend on:
//
//   * The constructor re-parents the supplied QListWidget to itself
//     (`listWidget->setParent(this)`), so the dialog owns the widget.
//     Tests must NOT delete the list widget manually.
//
//   * show() clears the selection and then sets the current row to
//     0 when the list has exactly 1 item, or to 1 when it has more
//     than 1 item. An empty list leaves the current row at -1.
//
//   * keyPressEvent() handles Tab only: it advances the current row
//     by one, wrapping back to row 0. It does not check modifiers.
//
//   * updateRecentFileHandle() removes any pre-existing item with the
//     same text, then inserts the new item at row 0 and selects it.
//     So calling it N times leaves the list ordered newest-first.
//
// Note on QTest::keyPress and Tab:
//   QWidget::event() performs focus traversal for a bare Tab press
//   BEFORE keyPressEvent() is reached. Whether it consumes the event
//   depends on the focus state of the dialog (which is itself
//   platform-dependent because of the Qt::ToolTip window flag).
//   Passing Qt::ControlModifier skips that focus-traversal branch
//   (see QWidget::event in Qt sources) and guarantees the event
//   reaches RecentFilesDialog::keyPressEvent. The dialog does not
//   inspect modifiers, so the semantic behaviour is unchanged.

#include <QListWidget>
#include <QListWidgetItem>
#include <QScopedPointer>
#include <QtTest/QtTest>
#include <catch2/catch.hpp>

#include "recentfilesdialog.h"

TEST_CASE("selection when list is empty", "[RecentFilesDialog]")
{
    const int expectedRow = -1;

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog(
        new RecentFilesDialog(nullptr, listPtr));

    dialog->show();

    // Empty list: show() takes neither branch, so the current row stays
    // at the QListWidget default of -1.
    REQUIRE(listPtr->currentRow() == expectedRow);

    // No delete: the dialog owns listPtr from construction onwards.
}

TEST_CASE("selection in 2nd item test", "[RecentFilesDialog]")
{
    const int expectedRow = 1;

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog(
        new RecentFilesDialog(nullptr, listPtr));

    listPtr->addItem("text1.txt");
    listPtr->addItem("test2.txt");
    dialog->show();

    // More than 1 item -> show() selects row 1.
    REQUIRE(listPtr->currentRow() == expectedRow);
}

TEST_CASE("selection in 2nd item and correct path test", "[RecentFilesDialog]")
{
    const QString expectedPath("path.txt");

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog(
        new RecentFilesDialog(nullptr, listPtr));

    listPtr->addItem("text1.txt");
    listPtr->addItem(expectedPath);
    dialog->show();

    REQUIRE(dialog->getCurrentRelativeFile() == expectedPath);
}

TEST_CASE("selection in 1st item and correct path test", "[RecentFilesDialog]")
{
    const QString expectedPath("path.txt");

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog(
        new RecentFilesDialog(nullptr, listPtr));

    listPtr->addItem(expectedPath);  // row 0
    listPtr->addItem("text2.txt");   // row 1
    dialog->show();                  // selects row 1

    // Ctrl+Tab guarantees delivery to keyPressEvent. Row 1 -> 2 wraps to
    // row 0, which is expectedPath.
    QTest::keyPress(dialog.data(), Qt::Key_Tab, Qt::ControlModifier);

    REQUIRE(dialog->getCurrentRelativeFile() == expectedPath);
}

TEST_CASE("adding paths to the list", "[RecentFilesDialog]")
{
    const QString expectedPath("path.txt");

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog(
        new RecentFilesDialog(nullptr, listPtr));

    // updateRecentFileHandle inserts at row 0.
    dialog->updateRecentFileHandle(expectedPath);  // row 0
    listPtr->addItem("text2.txt");                 // row 1
    dialog->show();                                // selects row 1

    QTest::keyPress(dialog.data(), Qt::Key_Tab, Qt::ControlModifier);
    // Row 1 -> 2 wraps to row 0, which is expectedPath.

    REQUIRE(dialog->getCurrentRelativeFile() == expectedPath);
}

TEST_CASE("adding path to the dialog using slots", "[RecentFilesDialog]")
{
    const QString item1 = "test1.txt";

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog(
        new RecentFilesDialog(nullptr, listPtr));

    dialog->updateRecentFileHandle(item1);
    dialog->show();

    REQUIRE(dialog->getCurrentRelativeFile() == item1);
}

TEST_CASE("adding 2 paths to the dialog using slots with show()",
          "[RecentFilesDialog]")
{
    const QString item1 = "test1.txt";
    const QString item2 = "test2.txt";

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog(
        new RecentFilesDialog(nullptr, listPtr));

    // FIX: the original test declared item2 but never inserted it, so it
    // did not match its own name. Add both paths.
    // After the two inserts the list is [test2.txt, test1.txt].
    dialog->updateRecentFileHandle(item1);
    dialog->updateRecentFileHandle(item2);

    dialog->show();  // 2 items -> selects row 1, i.e. item1

    REQUIRE(dialog->getCurrentRelativeFile() == item1);
}

TEST_CASE("adding 2 paths to the dialog using slots without show()",
          "[RecentFilesDialog]")
{
    const QString item1 = "test1.txt";
    const QString item2 = "test2.txt";

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog(
        new RecentFilesDialog(nullptr, listPtr));

    // updateRecentFileHandle selects row 0 itself. After both inserts the
    // list is [test2.txt, test1.txt], so row 0 is item2.
    dialog->updateRecentFileHandle(item1);
    dialog->updateRecentFileHandle(item2);

    REQUIRE(dialog->getCurrentRelativeFile() == item2);
}

TEST_CASE("adding more paths to the dialog using slots", "[RecentFilesDialog]")
{
    const QString item1 = "test1.txt";
    const QString item2 = "test2.txt";
    const QString item3 = "test3.txt";

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog(
        new RecentFilesDialog(nullptr, listPtr));

    // List after all three inserts: [test3.txt, test2.txt, test1.txt].
    dialog->updateRecentFileHandle(item1);
    dialog->updateRecentFileHandle(item2);
    dialog->updateRecentFileHandle(item3);

    dialog->show();  // selects row 1 = item2

    // Ctrl+Tab: row 1 -> row 2 = item1.
    QTest::keyPress(dialog.data(), Qt::Key_Tab, Qt::ControlModifier);

    REQUIRE(dialog->getCurrentRelativeFile() == item1);
}

TEST_CASE("cycling back to the last file", "[RecentFilesDialog]")
{
    const QString item1 = "test1.txt";
    const QString item2 = "test2.txt";
    const QString item3 = "test3.txt";

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog(
        new RecentFilesDialog(nullptr, listPtr));

    // List after all three inserts: [test3.txt, test2.txt, test1.txt].
    dialog->updateRecentFileHandle(item1);
    dialog->updateRecentFileHandle(item2);
    dialog->updateRecentFileHandle(item3);

    dialog->show();  // selects row 1 = item2

    QTest::keyPress(dialog.data(), Qt::Key_Tab,
                    Qt::ControlModifier);  // row 1 -> row 2 = item1
    QTest::keyPress(dialog.data(), Qt::Key_Tab,
                    Qt::ControlModifier);  // row 2 -> wraps to row 0 = item3

    REQUIRE(dialog->getCurrentRelativeFile() == item3);
}
