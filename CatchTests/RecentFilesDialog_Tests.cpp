#include <catch2/catch.hpp>
#include "recentfilesdialog.h"

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

