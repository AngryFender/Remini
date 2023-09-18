#include <catch2/catch.hpp>
#include "recentfilesdialog.h"
#include <QtTest/QTestKeyEvent>

TEST_CASE("selection when list is empty", "[RecentFilesDialog]")
{
    const int expectedRow = -1;

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog ( new RecentFilesDialog(nullptr,listPtr));

    dialog->show();

    int currentRow = listPtr->currentRow();
    REQUIRE( currentRow== expectedRow);

    delete listPtr;
}

TEST_CASE("selection in 2nd item test", "[RecentFilesDialog]")
{
    const int expectedRow = 1;

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog ( new RecentFilesDialog(nullptr,listPtr));

    listPtr->addItem("text1.txt");
    listPtr->addItem("test2.txt");
    dialog->show();

    int currentRow = listPtr->currentRow();
    REQUIRE( currentRow== expectedRow);

    delete listPtr;
}

TEST_CASE("selection in 2nd item and correct path test", "[RecentFilesDialog]")
{
    const QString expectedPath("path.txt");

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog ( new RecentFilesDialog(nullptr,listPtr));

    listPtr->addItem("text1.txt");
    listPtr->addItem(expectedPath);
    dialog->show();

    QString path = dialog->getCurrentRelativeFile();

    REQUIRE( path == expectedPath);
    delete listPtr;
}

TEST_CASE("selection in 1st item and correct path test", "[RecentFilesDialog]")
{
    const QString expectedPath("path.txt");

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog ( new RecentFilesDialog(nullptr,listPtr));

    listPtr->addItem(expectedPath);
    listPtr->addItem("text2.txt");
    dialog->show();

    QTest::keyPress(dialog.data(), Qt::Key_Tab,Qt::ControlModifier);
    QString path = dialog->getCurrentRelativeFile();

    REQUIRE( path == expectedPath);
    delete listPtr;
}

TEST_CASE("adding paths to the list", "[RecentFilesDialog]")
{
    const QString expectedPath("path.txt");

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog ( new RecentFilesDialog(nullptr,listPtr));

    dialog->updateRecentFileHandle(expectedPath);
    listPtr->addItem("text2.txt");
    dialog->show();

    QTest::keyPress(dialog.data(), Qt::Key_Tab,Qt::ControlModifier);
    QString path = dialog->getCurrentRelativeFile();

    REQUIRE( path == expectedPath);
    delete listPtr;
}

TEST_CASE("adding path to the dialog using slots", "[RecentFilesDialog]"){
    QString item1 = "test1.txt";

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog ( new RecentFilesDialog(nullptr,listPtr));

    dialog->updateRecentFileHandle(item1);
    dialog->show();

    QString result = dialog->getCurrentRelativeFile();
    REQUIRE( result == item1);

    delete listPtr;
}

TEST_CASE("adding 2 paths to the dialog using slots with show()", "[RecentFilesDialog]"){
    QString item1 = "test1.txt";
    QString item2 = "test2.txt";

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog ( new RecentFilesDialog(nullptr,listPtr));

    dialog->updateRecentFileHandle(item1);
    dialog->show();

    QString result = dialog->getCurrentRelativeFile();
    REQUIRE( result == item1);

    delete listPtr;
}

TEST_CASE("adding 2 paths to the dialog using slots without show()", "[RecentFilesDialog]"){
    QString item1 = "test1.txt";
    QString item2 = "test2.txt";

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog ( new RecentFilesDialog(nullptr,listPtr));

    dialog->updateRecentFileHandle(item1);
    dialog->updateRecentFileHandle(item2);

    QString result = dialog->getCurrentRelativeFile();
    REQUIRE( result == item2);

    delete listPtr;
}

TEST_CASE("adding more paths to the dialog using slots", "[RecentFilesDialog]"){
    QString item1 = "test1.txt";
    QString item2 = "test2.txt";
    QString item3 = "test3.txt";

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog ( new RecentFilesDialog(nullptr,listPtr));

    dialog->updateRecentFileHandle(item1);
    dialog->updateRecentFileHandle(item2);
    dialog->updateRecentFileHandle(item3);
    dialog->show();

    QTest::keyPress(dialog.data(), Qt::Key_Tab);
    QString result = dialog->getCurrentRelativeFile();
    REQUIRE( result == item1);	// its item1 and not item2 because dialog->show() will also increase the selection index

    delete listPtr;
}

TEST_CASE("cycling back to the last file", "[RecentFilesDialog]"){
    QString item1 = "test1.txt";
    QString item2 = "test2.txt";
    QString item3 = "test3.txt";

    QListWidget *listPtr = new QListWidget;
    QScopedPointer<RecentFilesDialog> dialog ( new RecentFilesDialog(nullptr,listPtr));

    dialog->updateRecentFileHandle(item1);
    dialog->updateRecentFileHandle(item2);
    dialog->updateRecentFileHandle(item3);
    dialog->show();

    QTest::keyPress(dialog.data(), Qt::Key_Tab);
    QTest::keyPress(dialog.data(), Qt::Key_Tab);
    QString result = dialog->getCurrentRelativeFile();
    REQUIRE( result == item3);

    delete listPtr;
}
