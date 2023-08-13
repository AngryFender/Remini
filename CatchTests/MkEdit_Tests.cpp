#include <catch2/catch.hpp>
#include "mkedit.h"
#include <QApplication>
#include <QClipboard>
#include <QScopedPointer>

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

TEST_CASE("MkEdit move cursor to the middle of the characters of first Markdown word", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2;   // '**'
    int cursorPosition = 2; // bo
    int newLinePos = 26;

    doc.setPlainText("**bold** _italic_ \n **new line**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    QString text = edit.toPlainText();
    REQUIRE("bold italic \n **new line**" == text);

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);

    QString text1 = edit.toPlainText();
    REQUIRE("**bold** _italic_ \n new line" == text1);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == cursorPosition + symbolLength);
}

TEST_CASE("MkEdit move cursor to the middle of the characters of second Markdown word", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2+2+1;   // '**' + '**' + '_'
    int cursorPosition = 8;     // bold ital
    int newLinePos = 26;

    doc.setPlainText("**bold** _italic_ \n **new line**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    QString text = edit.toPlainText();
    REQUIRE("bold italic \n **new line**" == text);

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);

    QString text1 = edit.toPlainText();
    REQUIRE("**bold** _italic_ \n new line" == text1);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == cursorPosition + symbolLength);
}

TEST_CASE("MkEdit move cursor to the middle of the characters of third Markdown word", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2+2+1+1+2;   // '**' + '**' + '_' + '_' + '~~'
    int cursorPosition = 16;     // bold italic stri
    int newLinePos = 36;

    doc.setPlainText("**bold** _italic_ ~~strike~~ \n **new line**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    QString text = edit.toPlainText();
    REQUIRE("bold italic strike \n **new line**" == text);

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);

    QString text1 = edit.toPlainText();
    REQUIRE("**bold** _italic_ ~~strike~~ \n new line" == text1);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == cursorPosition + symbolLength);
}

TEST_CASE("MkEdit move cursor to the middle of the characters of fourth Markdown word which is link", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2+2+1+1+2+2+1;   // '**' + '**' + '_' + '_' + '~~' + '~~' + '['
    int cursorPosition = 22;     // bold italic strike goo
    int newLinePos = 71;

    doc.setPlainText("**bold** _italic_ ~~strike~~ [google](https://www.google.com/) \n **new line**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    QString text = edit.toPlainText();
    REQUIRE("bold italic strike google \n **new line**" == text);

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);

    QString text1 = edit.toPlainText();
    REQUIRE("**bold** _italic_ ~~strike~~ [google](https://www.google.com/) \n new line" == text1);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == cursorPosition + symbolLength);
}

TEST_CASE("MkEdit move cursor to the middle of the characters of fifth Markdown word which is another link", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2+2+1+1+2+2+1+26+1;   // '**' + '**' + '_' + '_' + '~~' + '~~' + '[' + '](https://www.google.com/)' + '['
    int cursorPosition = 32;     // bold italic strike google youtub
    int newLinePos = 106;

    doc.setPlainText("**bold** _italic_ ~~strike~~ [google](https://www.google.com/) [youtube](https://www.youtube.com/) \n **new line**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    QString text = edit.toPlainText();
    REQUIRE("bold italic strike google youtube \n **new line**" == text);

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);

    QString text1 = edit.toPlainText();
    REQUIRE("**bold** _italic_ ~~strike~~ [google](https://www.google.com/) [youtube](https://www.youtube.com/) \n new line" == text1);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == cursorPosition + symbolLength);
}

TEST_CASE("MkEdit move cursor to the middle of the characters of 6 word where first 5 are bold", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 4*5;         // '**' + '**' *5
    int cursorPosition = 28;        // google google goo
    int newLinePos = 59;

    doc.setPlainText("**bold** **bold** **bold** **bold** **bold** hello \n **new line**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    QString text = edit.toPlainText();
    REQUIRE("bold bold bold bold bold hello \n **new line**" == text);

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);

    QString text1 = edit.toPlainText();
    REQUIRE("**bold** **bold** **bold** **bold** **bold** hello \n new line" == text1);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == cursorPosition + symbolLength);
}

TEST_CASE("MkEdit move cursor to the middle of the characters of 6 word where first 5 are links", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 1+26+1+26+1;   // '[' + '](https://www.google.com/)' + '[' + '](https://www.google.com/)' + '['
    int cursorPosition = 17;     // google google goo
    int newLinePos = 185;

    doc.setPlainText("[google](https://www.google.com/) [google](https://www.google.com/) [google](https://www.google.com/) [google](https://www.google.com/) [google](https://www.google.com/) hello \n **new line**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    QString text = edit.toPlainText();
    REQUIRE("google google google google google hello \n **new line**" == text);

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);

    QString text1 = edit.toPlainText();
    REQUIRE("[google](https://www.google.com/) [google](https://www.google.com/) [google](https://www.google.com/) [google](https://www.google.com/) [google](https://www.google.com/) hello \n new line" == text1);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == cursorPosition + symbolLength);
}

TEST_CASE("MkEdit move cursor to the middle of the characters after checkbox", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 3+3;   // '- [' + 'x] '
    int cursorPosition = 4;     // ☑che
    int minusCheckBoxCount = 1; // ☑
    int newLinePos = 21;

    doc.setPlainText("- [x] check1 \n **new line**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    QString text = edit.toPlainText();
    REQUIRE("☑check1 \n **new line**" == text);

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);

    QString text1 = edit.toPlainText();
    REQUIRE("- [x] check1 \n new line" == text1);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == cursorPosition + symbolLength - minusCheckBoxCount);

}

TEST_CASE("MkEdit move cursor to the middle of the 2nd characters after checkbox", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 3+3+3+3;   // '- [' + 'x] ' + '- [' + 'x] '
    int cursorPosition = 5;     // ☑☑che
    int minusCheckBoxCount = 2; // ☑☑
    int newLinePos = 27;

    doc.setPlainText("- [x] - [x] check1 \n **new line**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    QString text = edit.toPlainText();
    REQUIRE("☑☑check1 \n **new line**" == text);

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);

    QString text1 = edit.toPlainText();
    REQUIRE("- [x] - [x] check1 \n new line" == text1);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == cursorPosition + symbolLength - minusCheckBoxCount);
}

TEST_CASE("MkEdit paste link from clipboard", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QString link = "https://www.google.com/";
    int initialPos = 0;

    edit.setDocument(&doc);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(link);
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("[](https://www.google.com/)" == text);
}

TEST_CASE("MkEdit paste path from clipboard", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QString link = "C:\\Users\\Public";
    int initialPos = 0;

    edit.setDocument(&doc);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(link);
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("[](file:///C:\\Users\\Public)" == text);
}

TEST_CASE("MkEdit paste link from clipboard then check if the cursor is at the middle of []", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QString link = "https://www.google.com/";
    int initialPos = 0;
    int desiredPos = 1;

    edit.setDocument(&doc);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(link);
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("[](https://www.google.com/)" == text);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == desiredPos);
}

TEST_CASE("MkEdit paste path from clipboard then check if the cursor is at the middle of []", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QString link = "C:\\Users\\Public";
    int initialPos = 0;
    int desiredPos = 1;

    edit.setDocument(&doc);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(link);
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("[](file:///C:\\Users\\Public)" == text);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == desiredPos);
}

TEST_CASE("MkEdit type inside bold format then check if the cursor is at the right place", "[MkEdit]")
{

    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 4; //**bo
    int desiredPos = 5; //**bop
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));

    doc.setPlainText("**bold**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::drawTextBlocks,
                     &doc,&MkTextDocument::drawTextBlocksHandler);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());

    QString text = edit.toPlainText();
    REQUIRE("**bopld**" == text);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == desiredPos);

}

TEST_CASE("MkEdit type all strings with bold format then check if the cursor is at the right place", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int desiredPos = 8; //**bop

    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::drawTextBlocks,
                     &doc,&MkTextDocument::drawTextBlocksHandler);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);


    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString(" ")));
    edit.keyPressEvent(keyPressEvent.data());

    QString testString = "**bold**";
    foreach (QChar chara,testString) {
        keyPressEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString(chara)));
        edit.keyPressEvent(keyPressEvent.data());
    }

    keyPressEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());

    keyPressEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());

    keyPressEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));
    edit.keyPressEvent(keyPressEvent.data());

    QString text = edit.toPlainText();
    REQUIRE(" **boldp**" == text);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == desiredPos);
}


TEST_CASE("MkEdit type inside link format in 2nd line then check if the cursor is at the right place", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 4; //**bo
    int desiredPos = 5; //**bop

    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));


    doc.setPlainText("**bold** \n **new line**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::drawTextBlocks,
                     &doc,&MkTextDocument::drawTextBlocksHandler);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());

    QString text = edit.toPlainText();
    REQUIRE("**bopld** \n new line" == text);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == desiredPos);
}

