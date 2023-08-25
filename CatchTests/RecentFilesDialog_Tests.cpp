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
