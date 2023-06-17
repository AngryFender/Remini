#include <catch2/catch.hpp>
#include "mktextdocument.h"
#include "mkedit.h"
#include <QApplication>

TEST_CASE("MkTextDocument simple text", "[MkTextDocument]")
{
    MkTextDocument doc;
    doc.setPlainText("abc");
    QString text =doc.toPlainText();
    REQUIRE("abc" == text);
}

TEST_CASE("MkTextDocument bold text", "[MkTextDocument]")
{
    MkTextDocument doc;
    doc.setPlainText("**abc**");
    QString text =doc.toPlainText();
    REQUIRE("**abc**" == text);
}

TEST_CASE("MkTextDocument italic text", "[MkTextDocument]")
{
    MkTextDocument doc;
    doc.setPlainText("*abc*");
    QString text =doc.toPlainText();
    REQUIRE("*abc*" == text);
}

TEST_CASE("MkTextDocument single word bold, hide symbols", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("**abc**");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("abc" == text);
}

TEST_CASE("MkTextDocument single word italic, hide symbols", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("*abc*");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("abc" == text);
}

TEST_CASE("MkTextDocument single word bold plus false bold sign, hide only bold symbols", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("**abc** **");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("abc **" == text);
}

TEST_CASE("MkTextDocument single word italic plus false italic sign, hide only italic sign", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("*abc* *");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("abc *" == text);
}

TEST_CASE("MkTextDocument link texts", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("[youtube](www.youtube.com)");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("youtube" == text);
}

TEST_CASE("MkTextDocument link texts with underscore", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("[lang_explain](https://sqlite.org/lang_explain.html)");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("lang_explain" == text);
}

TEST_CASE("MkTextDocument link texts with empty title", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("[](www.google.com)");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("" == text);
}
