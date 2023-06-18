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

TEST_CASE("MkTextDocument single word bold, hide symbols, multiple words", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("**abc** **qwerty**");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("abc qwerty" == text);
}

TEST_CASE("MkTextDocument single word bold, hide symbols, multiple words, multiple lines", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("**abc** **qwerty**\n **new** **line**");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 2,rec);

    QString text = edit.toPlainText();
    REQUIRE("abc qwerty\n new line" == text);
}

TEST_CASE("MkTextDocument single word bold, hide symbols, multiple words, multiple lines, cursor on the last line", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("**abc** **qwerty**\n **new** **line**");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("abc qwerty\n **new** **line**" == text);
}

TEST_CASE("MkTextDocument single word bold, hide symbols, underscore", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("__abc__");
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

TEST_CASE("MkTextDocument single word italic, hide symbols, underscore", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("_abc_");
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

TEST_CASE("MkTextDocument single word bold plus false bold sign, hide only bold symbols, underscore", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("__abc__ __");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("abc __" == text);
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

TEST_CASE("MkTextDocument single word italic plus false italic sign, hide only italic sign, underscore", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("_abc_ _");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("abc _" == text);
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

TEST_CASE("MkTextDocument link texts false positive", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("abc](123");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false, 1,rec);

    QString text = edit.toPlainText();
    REQUIRE("abc](123" == text);
}


TEST_CASE("MkTextDocument strikethrough", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("~~strike~~");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false,1,rec);

    QString text = edit.toPlainText();
    REQUIRE("strike" == text);
}

TEST_CASE("MkTextDocument strikethrough, false positive at the back", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("strike~~");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false,1,rec);

    QString text = edit.toPlainText();
    REQUIRE("strike~~" == text);
}

TEST_CASE("MkTextDocument strikethrough, false positive at the front", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("~~strike");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false,1,rec);

    QString text = edit.toPlainText();
    REQUIRE("~~strike" == text);
}

TEST_CASE("MkTextDocument strikethrough with false positive at the back", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("~~strike~~ ~~");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false,1,rec);

    QString text = edit.toPlainText();
    REQUIRE("strike ~~" == text);
}

TEST_CASE("MkTextDocument strikethrough with false positive at the front", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    QRect rec;

    doc.setPlainText("~~ ~~strike~~");
    edit.setDocument(&doc);
    doc.cursorPosChangedHandle(false,1,rec);

    QString text = edit.toPlainText();
    REQUIRE(" strike~~" == text);
}
