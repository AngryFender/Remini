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
    SelectRange range;
    doc.setPlainText("**abc**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc" == text);
}

TEST_CASE("MkTextDocument bold, hide symbols, multiple words", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**abc** **qwerty**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc qwerty" == text);
}

TEST_CASE("MkTextDocument bold, hide symbols on multiple lines", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**abc** **qwerty**\n **new** **line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc qwerty\n new line" == text);
}

TEST_CASE("MkTextDocument bold, hide symbols only on 1st line", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**abc** **qwerty**\n **new** **line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc qwerty\n **new** **line**" == text);
}

TEST_CASE("MkTextDocument bold, hide symbols, underscore", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("__abc__");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc" == text);
}

TEST_CASE("MkTextDocument bold, hide underscore symbols only on 1st line", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("__abc__\n __123__");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc\n __123__" == text);
}


TEST_CASE("MkTextDocument bold, hide underscore symbols only on both lines", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("__abc__\n __123__");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc\n 123" == text);
}

TEST_CASE("MkTextDocument single word italic, hide symbols", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("*abc*");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc" == text);
}

TEST_CASE("MkTextDocument italic, hide symbol on 1st line", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("*abc*\n*hello*");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc\n*hello*" == text);
}

TEST_CASE("MkTextDocument italic, hide symbols on both lines", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("*abc*\n*hello*");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc\nhello" == text);
}

TEST_CASE("MkTextDocument single word italic, hide symbols, underscore", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("_abc_");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc" == text);
}

TEST_CASE("MkTextDocument italic, hide underscore symbols on 1st line", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("_abc_\n_hello_");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc\n_hello_" == text);
}

TEST_CASE("MkTextDocument italic, hide underscore symbols on both lines", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("_abc_\n_hello_");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc\nhello" == text);
}

TEST_CASE("MkTextDocument single word bold plus false bold sign, hide only bold symbols", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**abc** **");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc **" == text);
}

TEST_CASE("MkTextDocument single word bold plus false bold sign, hide only bold symbols, underscore", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("__abc__ __");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc __" == text);
}

TEST_CASE("MkTextDocument single word italic plus false italic sign, hide only italic sign", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("*abc* *");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc *" == text);
}

TEST_CASE("MkTextDocument single word italic plus false italic sign, hide only italic sign, underscore", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("_abc_ _");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc _" == text);
}

TEST_CASE("MkTextDocument link texts", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("[youtube](<www.youtube.com>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("youtube" == text);
}

TEST_CASE("MkTextDocument link texts, hide symbols only in 1st line", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("[youtube](<www.youtube.com>)\n[google](<www.google.com>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("youtube\n[google](<www.google.com>)" == text);
}

TEST_CASE("MkTextDocument link texts, hide symbols only in both lines", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("[youtube](<www.youtube.com>)\n[google](<www.google.com>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("youtube\ngoogle" == text);
}

TEST_CASE("MkTextDocument link texts with underscore", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("[lang_explain](<https://sqlite.org/lang_explain.html>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("lang_explain" == text);
}

TEST_CASE("MkTextDocument link texts with empty title", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("[](<www.google.com>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("" == text);
}

TEST_CASE("MkTextDocument link texts false positive", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("abc](<123");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc](<123" == text);
}

TEST_CASE("MkTextDocument local link texts, with () round brackets in the path", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("[microsoft](<file:///C:\\Program Files (x86)\\Microsoft>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("microsoft" == text);
}

TEST_CASE("MkTextDocument local link texts, with ()() round brackets in the path", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("[folder](<file:///C:\\New folder(folder)\\New folder(folder)>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("folder" == text);
}

TEST_CASE("MkTextDocument local link texts, with [,],(,),$,%,. symbols in the path", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("[folder](<file:///C:\\New folder(folder)\\New folder(folder)\\New folder([h]folder&&$%).))))))))\\New (this) folder>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("folder" == text);
}

TEST_CASE("MkTextDocument strikethrough", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("~~strike~~");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("strike" == text);
}

TEST_CASE("MkTextDocument strikethrough, hide symbols only in 1st line", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("~~strike~~\n~~new line~~");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("strike\n~~new line~~" == text);
}

TEST_CASE("MkTextDocument strikethrough, hide symbols in both lines", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("~~strike~~\n~~new line~~");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("strike\nnew line" == text);
}

TEST_CASE("MkTextDocument strikethrough, false positive at the back", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("strike~~");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("strike~~" == text);
}

TEST_CASE("MkTextDocument strikethrough, false positive at the front", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("~~strike");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("~~strike" == text);
}

TEST_CASE("MkTextDocument strikethrough with false positive at the back", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("~~strike~~ ~~");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("strike ~~" == text);
}

TEST_CASE("MkTextDocument strikethrough with false positive at the front", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("~~ ~~strike~~");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE(" strike~~" == text);
}

TEST_CASE("MkTestDocument single checkbox unchecked","[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("- [ ] ");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("☐"==text);
}

TEST_CASE("MkTestDocument double checkbox unchecked","[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("- [ ] - [ ] ");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("☐☐"==text);
}

TEST_CASE("MkTestDocument checkbox unchecked, hidden only in 1st line","[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("- [ ] \n- [ ] ");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("☐\n☐"==text);
}

TEST_CASE("MkTestDocument single checkbox checked","[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("- [x] ");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("☑"==text);
}

TEST_CASE("MkTestDocument double checkbox checked","[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("- [x] - [x] ");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("☑☑"==text);
}

TEST_CASE("MkTestDocument checkbox checked, hidden only in 1st line","[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("- [x] \n- [x] ");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 2;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("☑\n☑"==text);
}

TEST_CASE("MkTextDocument single word bold, setMarkdown = false", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**abc**");
    doc.setMarkdownHandle(false);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = doc.toPlainText();
    REQUIRE("**abc**" == text);
}

TEST_CASE("MkTextDocument single word bold, setMarkdown = true", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**abc**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = doc.toPlainText();
    REQUIRE("abc" == text);
}

TEST_CASE("MkTextDocument single word italic, underscore, setMarkdown = false", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("_abc_");
    doc.setMarkdownHandle(false);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("_abc_" == text);
}

TEST_CASE("MkTextDocument single word italic, underscore, setMarkdown = true", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("_abc_");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("abc" == text);
}

TEST_CASE("MkTextDocument strikethrough, setMarkdown = false", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("~~strike~~");
    doc.setMarkdownHandle(false);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("~~strike~~" == text);
}

TEST_CASE("MkTextDocument strikethrough, setMarkdown = true", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("~~strike~~");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("strike" == text);
}

TEST_CASE("MkTextDocument link texts with underscore, setMarkdown = false", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("[lang_explain](https://sqlite.org/lang_explain.html)");
    doc.setMarkdownHandle(false);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("[lang_explain](https://sqlite.org/lang_explain.html)" == text);
}

TEST_CASE("MkTextDocument link texts with underscore, setMarkdown = true", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("[lang_explain](<https://sqlite.org/lang_explain.html>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("lang_explain" == text);
}

TEST_CASE("MkTestDocument single checkbox unchecked, setMarkdown = false","[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("- [ ] ");
    doc.setMarkdownHandle(false);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("- [ ] "==text);
}

TEST_CASE("MkTestDocument single checkbox unchecked, setMarkdown = true","[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("- [ ] ");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("☐"==text);
}

TEST_CASE("MkTestDocument single checkbox checked, setMarkdown = false","[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("- [x] ");
    doc.setMarkdownHandle(false);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("- [x] "==text);
}

TEST_CASE("MkTestDocument single checkbox checked, setMarkdown = true","[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("- [x] ");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = edit.toPlainText();
    REQUIRE("☑"==text);
}

TEST_CASE("MkTextDocument bold and italic, setMarkdown = false", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**bold** _italic_");
    doc.setMarkdownHandle(false);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = doc.toPlainText();
    REQUIRE("**bold** _italic_" == text);
}

TEST_CASE("MkTextDocument bold and italic, setMarkdown = true", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**bold** _italic_");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = doc.toPlainText();
    REQUIRE("bold italic" == text);
}

TEST_CASE("MkTextDocument bold and italic in two lines, setMarkdown = false", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**bold** _italic_ \n **new line**");
    doc.setMarkdownHandle(false);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = doc.toPlainText();
    REQUIRE("**bold** _italic_ \n **new line**" == text);
}

TEST_CASE("MkTextDocument bold and italic in two lines, setMarkdown = true", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**bold** _italic_ \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = doc.toPlainText();
    REQUIRE("bold italic \n **new line**" == text);
}

TEST_CASE("MkTextDocument bold and italic in two lines, setMarkdown = true, focus on 1st line", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**bold** _italic_ \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 0;
    doc.cursorPosChangedHandle(&range);

    QString text = doc.toPlainText();
    REQUIRE("**bold** _italic_ \n new line" == text);
}

TEST_CASE("MkTextDocument bold and italic in two lines, setMarkdown = true, focus on 2nd line", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    SelectRange range;

    doc.setPlainText("**bold** _italic_ \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    range.hasSelection = false;
    range.currentBlockNo = 1;
    doc.cursorPosChangedHandle(&range);

    QString text = doc.toPlainText();
    REQUIRE("bold italic \n **new line**" == text);
}
