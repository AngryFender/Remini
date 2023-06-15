#include <catch2/catch.hpp>
#include "mkedit.h"
#include <QApplication>

TEST_CASE("MkEdit simple text", "[MkEdit]")
{
    MkEdit edit;

    edit.setText("abc");
    QString text =edit.toPlainText();

    REQUIRE("abc" == text);

}

TEST_CASE("MkEdit bold double asterisk", "[MkEdit]")
{
    MkEdit edit;

    edit.setText("**abc**");
    QString text =edit.toPlainText();

    REQUIRE("**abc**" == text);
}

TEST_CASE("MkEdit bold double underscore", "[MkEdit]")
{
    MkEdit edit;

    edit.setText("__abc__");
    QString text =edit.toPlainText();

    REQUIRE("__abc__" == text);
}

