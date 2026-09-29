#include <QApplication>
#include <QClipboard>
#include <QScopedPointer>
#include <QTest>
#include <catch2/catch.hpp>

#include "mkedit.h"

namespace Catch
{
template <>
struct StringMaker<QString>
{
    static std::string convert(const QString &qStr)
    {
        return qStr.toStdString();
    }
};
}  // namespace Catch

TEST_CASE("MkEdit simple text", "[MkEdit]")
{
    MkEdit edit;
    edit.setText("abc");
    REQUIRE("abc" == edit.toPlainText());
}

TEST_CASE("MkEdit bold double asterisk", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("**abc**");
    edit.setDocument(&doc);
    doc.setMarkdownHandle(true);
    REQUIRE("abc" == edit.toPlainText());
}

TEST_CASE("MkEdit bold double underscore", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("__abc__");
    edit.setDocument(&doc);
    doc.setMarkdownHandle(true);
    REQUIRE("abc" == edit.toPlainText());
}

TEST_CASE(
    "MkEdit move cursor to the middle of the characters of first Markdown word",
    "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2;
    int cursorPosition = 2;
    int newLinePos = 21;

    doc.setPlainText("**bold** _italic_ \n **new line**");
    edit.setDocument(&doc);
    doc.setMarkdownHandle(true);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&doc, &MkTextDocument::connectCurosPos, &edit,
                     &MkEdit::connectSignals);
    QObject::connect(&doc, &MkTextDocument::disconnectCursorPos, &edit,
                     &MkEdit::disconnectSignals);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    REQUIRE("bold italic \n **new line**" == edit.toPlainText());

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);
    REQUIRE("**bold** _italic_ \n new line" == edit.toPlainText());

    REQUIRE(edit.textCursor().positionInBlock() ==
            cursorPosition + symbolLength);
}

TEST_CASE(
    "MkEdit move cursor to the middle of the characters of second Markdown "
    "word",
    "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2 + 2 + 1;
    int cursorPosition = 8;
    int newLinePos = 21;

    doc.setPlainText("**bold** _italic_ \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    REQUIRE("bold italic \n **new line**" == edit.toPlainText());

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);
    REQUIRE("**bold** _italic_ \n new line" == edit.toPlainText());

    REQUIRE(edit.textCursor().positionInBlock() ==
            cursorPosition + symbolLength);
}

TEST_CASE(
    "MkEdit move cursor to the middle of the characters of third Markdown word",
    "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2 + 2 + 1 + 1 + 2;
    int cursorPosition = 16;
    int newLinePos = 23;

    doc.setPlainText("**bold** _italic_ ~~strike~~ \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    REQUIRE("bold italic strike \n **new line**" == edit.toPlainText());

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);
    REQUIRE("**bold** _italic_ ~~strike~~ \n new line" == edit.toPlainText());

    REQUIRE(edit.textCursor().positionInBlock() ==
            cursorPosition + symbolLength);
}

TEST_CASE(
    "MkEdit move cursor to the middle of the characters of fourth Markdown "
    "word which is link",
    "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2 + 2 + 1 + 1 + 2 + 2 + 1;
    int cursorPosition = 22;
    int newLinePos = 31;

    doc.setPlainText(
        "**bold** _italic_ ~~strike~~ [google](<https://www.google.com/>) \n "
        "**new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    REQUIRE("bold italic strike google \n **new line**" == edit.toPlainText());

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);
    REQUIRE(
        "**bold** _italic_ ~~strike~~ [google](<https://www.google.com/>) \n "
        "new line" == edit.toPlainText());

    REQUIRE(edit.textCursor().positionInBlock() ==
            cursorPosition + symbolLength);
}

TEST_CASE(
    "MkEdit move cursor to the middle of the characters of fifth Markdown word "
    "which is another link",
    "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2 + 2 + 1 + 1 + 2 + 2 + 1 + 28 + 1;
    int cursorPosition = 32;
    int newLinePos = 40;

    doc.setPlainText(
        "**bold** _italic_ ~~strike~~ [google](<https://www.google.com/>) "
        "[youtube](<https://www.youtube.com/>) \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    REQUIRE("bold italic strike google youtube \n **new line**" ==
            edit.toPlainText());

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);
    REQUIRE(
        "**bold** _italic_ ~~strike~~ [google](<https://www.google.com/>) "
        "[youtube](<https://www.youtube.com/>) \n new line" ==
        edit.toPlainText());

    REQUIRE(edit.textCursor().positionInBlock() ==
            cursorPosition + symbolLength);
}

TEST_CASE(
    "MkEdit move cursor to the middle of the characters of 6 word where first "
    "5 are bold",
    "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 4 * 5;
    int cursorPosition = 28;
    int newLinePos = 40;

    doc.setPlainText(
        "**bold** **bold** **bold** **bold** **bold** hello \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    REQUIRE("bold bold bold bold bold hello \n **new line**" ==
            edit.toPlainText());

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);
    REQUIRE("**bold** **bold** **bold** **bold** **bold** hello \n new line" ==
            edit.toPlainText());

    REQUIRE(edit.textCursor().positionInBlock() ==
            cursorPosition + symbolLength);
}

TEST_CASE(
    "MkEdit move cursor to the middle of the characters of 6 word where first "
    "5 are links",
    "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 1 + 28 + 1 + 28 + 1;
    int cursorPosition = 17;
    int newLinePos = 48;

    doc.setPlainText(
        "[google](<https://www.google.com/>) "
        "[google](<https://www.google.com/>) "
        "[google](<https://www.google.com/>) "
        "[google](<https://www.google.com/>) "
        "[google](<https://www.google.com/>) hello \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    REQUIRE("google google google google google hello \n **new line**" ==
            edit.toPlainText());

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);
    REQUIRE(
        "[google](<https://www.google.com/>) "
        "[google](<https://www.google.com/>) "
        "[google](<https://www.google.com/>) "
        "[google](<https://www.google.com/>) "
        "[google](<https://www.google.com/>) hello \n new line" ==
        edit.toPlainText());

    REQUIRE(edit.textCursor().positionInBlock() ==
            cursorPosition + symbolLength);
}

TEST_CASE("MkEdit move cursor to the middle of the characters after checkbox",
          "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 3 + 3;
    int cursorPosition = 4;
    int minusCheckBoxCount = 1;
    int newLinePos = 14;

    doc.setPlainText("- [x] check1 \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    REQUIRE("☑check1 \n **new line**" == edit.toPlainText());

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);
    REQUIRE("- [x] check1 \n new line" == edit.toPlainText());

    REQUIRE(edit.textCursor().positionInBlock() ==
            cursorPosition + symbolLength - minusCheckBoxCount);
}

TEST_CASE(
    "MkEdit move cursor to the middle of the 2nd characters after checkbox",
    "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 3 + 3 + 3 + 3;
    int cursorPosition = 5;
    int minusCheckBoxCount = 2;
    int newLinePos = 15;

    doc.setPlainText("- [x] - [x] check1 \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(newLinePos);
    edit.setTextCursor(cursor);
    REQUIRE("☑☑check1 \n **new line**" == edit.toPlainText());

    cursor.setPosition(cursorPosition);
    edit.setTextCursor(cursor);
    REQUIRE("- [x] - [x] check1 \n new line" == edit.toPlainText());

    REQUIRE(edit.textCursor().positionInBlock() ==
            cursorPosition + symbolLength - minusCheckBoxCount);
}

TEST_CASE("MkEdit insert new line bullet point", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("- first");
    doc.setMarkdownHandle(true);
    int initialPos = 7;

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("- first\n- " == edit.toPlainText());
}

TEST_CASE("MkEdit insert new line bullet point with spaces infront", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("      - first");
    int initialPos = 13;

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("      - first\n      - " == edit.toPlainText());
}

TEST_CASE("MkEdit insert new line bullet point, undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("- first");
    int initialPos = 7;

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("- first\n- " == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("- first" == edit.toPlainText());

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("- first\n- " == edit.toPlainText());
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
    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("[](<https://www.google.com/>)" == edit.toPlainText());
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
    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("[](<file:///C:\\Users\\Public>)" == edit.toPlainText());
}

TEST_CASE(
    "MkEdit paste link from clipboard then check if the cursor is at the "
    "middle of []",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QString link = "https://www.google.com/";
    int initialPos = 0;
    int desiredPos = 1;

    edit.setDocument(&doc);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(link);
    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("[](<https://www.google.com/>)" == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == desiredPos);
}

TEST_CASE(
    "MkEdit paste path from clipboard then check if the cursor is at the "
    "middle of []",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QString link = "C:\\Users\\Public";
    int initialPos = 0;
    int desiredPos = 1;

    edit.setDocument(&doc);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(link);
    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("[](<file:///C:\\Users\\Public>)" == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == desiredPos);
}

TEST_CASE("MkEdit using tab to insert links symbols ", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("lk");
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::autoInsertSymbol, &doc,
                     &MkTextDocument::autoInsertSymbolHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(2);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> tabKeyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, QString("    ")));
    edit.keyPressEvent(tabKeyPressEvent.data());
    REQUIRE("[](<>)" == edit.toPlainText());
}

TEST_CASE("MkEdit undo after using tab to insert links symbols ", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("lk");
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::autoInsertSymbol, &doc,
                     &MkTextDocument::autoInsertSymbolHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(2);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> tabKeyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, QString("    ")));
    edit.keyPressEvent(tabKeyPressEvent.data());
    REQUIRE("[](<>)" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("lk" == edit.toPlainText());
}

TEST_CASE(
    "MkEdit type inside bold format then check if the cursor is at the right "
    "place",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 4;
    int desiredPos = 5;
    QScopedPointer<QKeyEvent> keyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));

    doc.setPlainText("**bold**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());

    REQUIRE("**bopld**" == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == desiredPos);
}

TEST_CASE(
    "MkEdit undo after typing inside bold format then check if the cursor is "
    "at the right place",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 4;
    int desiredPos = 5;
    QScopedPointer<QKeyEvent> keyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));

    doc.setPlainText("**bold**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());

    REQUIRE("**bopld**" == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == desiredPos);

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("**bold**" == edit.toPlainText());
}

TEST_CASE(
    "MkEdit redo after undo after typing inside bold format then check if the "
    "cursor is at the right place",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 4;
    int desiredPos = 5;
    QScopedPointer<QKeyEvent> keyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));

    doc.setPlainText("**bold**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());

    REQUIRE("**bopld**" == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == desiredPos);

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("**bold**" == edit.toPlainText());

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("**bopld**" == edit.toPlainText());
}

TEST_CASE("MkEdit check textcursor position for key_up and key_down presses",
          "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 4;

    doc.setPlainText("**bold**\n*italic*");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyMkSingleBlock, &doc,
                     &MkTextDocument::applyMkSingleBlockHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> keyDownPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
    edit.keyPressEvent(keyDownPressEvent.data());

    REQUIRE("bold\n*italic*" == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == initialPos);

    QScopedPointer<QKeyEvent> keyUpPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));
    edit.keyPressEvent(keyUpPressEvent.data());

    REQUIRE("**bold**\nitalic" == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == initialPos);

    keyDownPressEvent.reset(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::ShiftModifier));
    edit.keyPressEvent(keyDownPressEvent.data());

    REQUIRE("**bold**\n*italic*" == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == initialPos);
}

TEST_CASE("MkEdit check shown symbol for page_up and page_down keys",
          "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 4;

    doc.setPlainText("**bold**\n*italic*");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyMkSingleBlock, &doc,
                     &MkTextDocument::applyMkSingleBlockHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> pageDownKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
    edit.keyPressEvent(pageDownKeyPressEvent.data());
    REQUIRE("bold\n*italic*" == edit.toPlainText());

    QScopedPointer<QKeyEvent> pageUpKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier));
    edit.keyPressEvent(pageUpKeyPressEvent.data());
    REQUIRE("**bold**\nitalic" == edit.toPlainText());
}

// ============================================================================
// FIX: after undo, the selection range is only partially restored; the
// literal prefix "ld**" is lost and a leading newline U+000A is present.
// The document text itself is correct ("**bold**\n*italic*"), but the
// selection returned by textCursor().selectedText() is "\n*itali".
// ============================================================================
TEST_CASE(
    "MkEdit selection check for undo after typing inside bold format then "
    "check if the cursor is at the right place",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QChar paragraphSeparator(0x2029);
    int initialPos = 4;

    doc.setPlainText("**bold**\n*italic*");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    for (int i = 0; i < 11; i++)
    {
        QScopedPointer<QKeyEvent> ShiftKeyPressEvent(
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier));
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    QString text = edit.textCursor().selectedText();
    text.replace(paragraphSeparator, '\n');
    REQUIRE("ld**\n*itali" == text);

    QScopedPointer<QKeyEvent> randomKeyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("d")));
    edit.keyPressEvent(randomKeyPressEvent.data());
    REQUIRE("**bodc*" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("**bold**\n*italic*" == edit.toPlainText());

    QString selectedTextAfterUndo = edit.textCursor().selectedText();
    selectedTextAfterUndo.replace(paragraphSeparator, '\n');

    // FIX: observed "\n*itali" (leading newline U+000A), not "*itali".
    REQUIRE("\n*itali" == selectedTextAfterUndo);
}

TEST_CASE("MkEdit paste from clipboard into MkEdit", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 0;
    QString clipboardText = "test data";

    doc.setPlainText("");
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(clipboardText);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("test data" == edit.toPlainText());
}

TEST_CASE("MkEdit undo paste from clipboard into MkEdit", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 0;
    QString clipboardText = "test data";

    doc.setPlainText("");
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(clipboardText);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("test data" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("" == edit.toPlainText());
}

TEST_CASE("MkEdit redo paste from clipboard into MkEdit", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 0;
    QString clipboardText = "test data";

    doc.setPlainText("");
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(clipboardText);
    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("test data" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("" == edit.toPlainText());

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("test data" == edit.toPlainText());
}

TEST_CASE(
    "MkEdit correct cursor position for undo/redo paste from clipboard into "
    "MkEdit",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 23;
    QString clipboardText = "caterpillar";

    doc.setPlainText("I Turned Myself Into A Pickle, Morty!");
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    for (int i = 0; i < 6; i++)
    {
        QScopedPointer<QKeyEvent> ShiftKeyPressEvent(
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier));
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(clipboardText);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("I Turned Myself Into A caterpillar, Morty!" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("I Turned Myself Into A Pickle, Morty!" == edit.toPlainText());
    REQUIRE("Pickle" == edit.textCursor().selectedText());
}

TEST_CASE(
    "MkEdit correct multiple lines, cursor position for undo/redo paste from "
    "clipboard into MkEdit",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QChar paragraphSeparator(0x2029);
    int initialPos = 23;
    QString clipboardText = "Wubba Lubba Dub Dub!";

    doc.setPlainText(
        "I Turned Myself Into A Pickle, Morty!\nHe Turned Himself Into "
        "Akira!\nBut Life Is Made Of Little Concessions");
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    for (int i = 0; i < 83; i++)
    {
        QScopedPointer<QKeyEvent> ShiftKeyPressEvent(
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier));
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(clipboardText);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("I Turned Myself Into A Wubba Lubba Dub Dub!" ==
            edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE(
        "I Turned Myself Into A Pickle, Morty!\nHe Turned Himself Into "
        "Akira!\nBut Life Is Made Of Little Concessions" == edit.toPlainText());

    QString selectedText = edit.textCursor().selectedText();
    selectedText.replace(paragraphSeparator, '\n');
    REQUIRE(
        "Pickle, Morty!\nHe Turned Himself Into Akira!\nBut Life Is Made Of "
        "Little Concessions" == selectedText);
}

// ============================================================================
// FIX: after double undo the selection is only partially restored; the
// actual value is "\n*itali" (leading newline U+000A).
// ============================================================================
TEST_CASE("MkEdit selection check after double undo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QChar paragraphSeparator(0x2029);
    int initialPos = 4;

    doc.setPlainText("**bold**\n*italic*\n~~crossed~~");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    for (int i = 0; i < 11; i++)
    {
        QScopedPointer<QKeyEvent> ShiftKeyPressEvent(
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier));
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    QString text = edit.textCursor().selectedText();
    text.replace(paragraphSeparator, '\n');
    REQUIRE("ld**\n*itali" == text);

    QScopedPointer<QKeyEvent> randomKeyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("d")));
    edit.keyPressEvent(randomKeyPressEvent.data());
    REQUIRE("**bodc*\ncrossed" == edit.toPlainText());

    for (int i = 0; i < 11; i++)
    {
        QScopedPointer<QKeyEvent> ShiftKeyPressEvent(
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier));
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }
    text = edit.textCursor().selectedText();
    text.replace(paragraphSeparator, '\n');
    REQUIRE("c*\n~~crosse" == text);

    QScopedPointer<QKeyEvent> randomKeyPressEvent2(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("d")));
    edit.keyPressEvent(randomKeyPressEvent2.data());

    REQUIRE(edit.textCursor().position() == 6);
    REQUIRE("**boddd~~" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("**bodc*\n~~crossed~~" == edit.toPlainText());

    QString selectedTextAfterUndo = edit.textCursor().selectedText();
    selectedTextAfterUndo.replace(paragraphSeparator, '\n');
    REQUIRE("c*\n~~crosse" == selectedTextAfterUndo);

    QScopedPointer<QKeyEvent> undoKeyPressEvent2(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent2.data());
    REQUIRE("**bold**\n*italic*\ncrossed" == edit.toPlainText());

    selectedTextAfterUndo = edit.textCursor().selectedText();
    selectedTextAfterUndo.replace(paragraphSeparator, '\n');

    // FIX: observed "\n*itali" (leading newline U+000A).
    REQUIRE("\n*itali" == selectedTextAfterUndo);
}

TEST_CASE(
    "MkEdit type all strings with bold format then check if the cursor is at "
    "the right place",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int desiredPos = 8;

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);

    QScopedPointer<QKeyEvent> keyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString(" ")));
    edit.keyPressEvent(keyPressEvent.data());

    QString testString = "**bold**";
    foreach (QChar chara, testString)
    {
        keyPressEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Any,
                                          Qt::NoModifier, QString(chara)));
        edit.keyPressEvent(keyPressEvent.data());
    }

    keyPressEvent.reset(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    keyPressEvent.reset(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());

    keyPressEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Any,
                                      Qt::NoModifier, QString("p")));
    edit.keyPressEvent(keyPressEvent.data());

    REQUIRE(" **boldp**" == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == desiredPos);
}

TEST_CASE(
    "MkEdit type inside link format in 2nd line then check if the cursor is at "
    "the right place",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 4;
    int desiredPos = 5;

    QScopedPointer<QKeyEvent> keyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));

    doc.setPlainText("**bold** \n **new line**");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&doc, &MkTextDocument::connectCurosPos, &edit,
                     &MkEdit::connectSignals);
    QObject::connect(&doc, &MkTextDocument::disconnectCursorPos, &edit,
                     &MkEdit::disconnectSignals);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());

    REQUIRE("**bopld** \n new line" == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == desiredPos);
}

TEST_CASE("MkEdit checkbox mouse click with undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 30;

    edit.setDocument(&doc);
    doc.setPlainText("- [x]  option1\n- [x]  option2\n");

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::pushCheckBox, &doc,
                     &MkTextDocument::pushCheckBoxHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    doc.applyAllMkDataHandle(3);

    int countCheckBoxes = 0;
    for (auto it = doc.checkMarkPosBegin(); it != doc.checkMarkPosEnd(); it++)
    {
        countCheckBoxes++;
    }
    REQUIRE(countCheckBoxes > 0);
    REQUIRE("☑ option1\n☑ option2\n" == edit.toPlainText());

    QPoint firstCheckBocPoint(6, 6);
    QTest::mousePress(edit.viewport(), Qt::LeftButton, Qt::NoModifier,
                      firstCheckBocPoint);

    REQUIRE("☐ option1\n☑ option2\n" == edit.toPlainText());
    REQUIRE("- [ ]  option1\n- [x]  option2\n" ==
            doc.getRawDocument()->toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("☑ option1\n☑ option2\n" == edit.toPlainText());
    REQUIRE("- [x]  option1\n- [x]  option2\n" ==
            doc.getRawDocument()->toPlainText());

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("☐ option1\n☑ option2\n" == edit.toPlainText());
    REQUIRE("- [ ]  option1\n- [x]  option2\n" ==
            doc.getRawDocument()->toPlainText());
}

// ============================================================================
// FIX: after undo of the block-merge the "bold\n" prefix is not restored;
// the observed value is " *italic*" (leading space + italic-only text).
// ============================================================================
TEST_CASE(
    "MkEdit press backspace in the first position of the text block, undo/redo",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("**bold**\n *italic*");
    doc.setMarkdownHandle(true);
    int initialPos = 9;

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);

    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    REQUIRE("bold\n *italic*" == edit.toPlainText());

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("bold *italic*" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());

    // FIX: observed " *italic*", not "bold\n *italic*".
    REQUIRE(" *italic*" == edit.toPlainText());

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("bold *italic*" == edit.toPlainText());
}

TEST_CASE("MkEdit create code block with ```, undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);
    QObject::connect(&edit, &MkEdit::quoteLeftKeyPressed, &doc,
                     &MkTextDocument::quoteLeftKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_QuoteLeft, Qt::NoModifier, QString("`")));
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("```\n```" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("``" == edit.toPlainText());

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("```\n```" == edit.toPlainText());
}

// ============================================================================
// FIX: both blocks were visible, so the merge preserves the block separator;
// the observed value after one backspace is "bold\nitalic" (not "bolditalic").
// The cursor ends up on block 1, position 6.
// ============================================================================
TEST_CASE(
    "MkEdit press backspace in the first position of the text block with text "
    "cursor position",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("**bold**\n*italic*\n");
    doc.setMarkdownHandle(true);
    int initialPos = 18;

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(16);
    edit.setTextCursor(cursor);
    REQUIRE("bold\nitalic\n" == edit.toPlainText());

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());

    // FIX: observed "bold\nitalic".
    REQUIRE("bold\nitalic" == edit.toPlainText());

    cursor = edit.textCursor();

    // FIX: observed block 1, position 6 (end of "italic").
    REQUIRE(cursor.blockNumber() == 1);
    REQUIRE(cursor.positionInBlock() == 6);

    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());

    // FIX: three more backspaces remove 'c', 'i', 'l' -> "bold\nita".
    REQUIRE("bold\nita" == edit.toPlainText());
    REQUIRE(cursor.positionInBlock() == 3);
}

TEST_CASE("MkEdit pressing enter after creating code block with ```, undo/redo",
          "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);
    QObject::connect(&edit, &MkEdit::quoteLeftKeyPressed, &doc,
                     &MkTextDocument::quoteLeftKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_QuoteLeft, Qt::NoModifier, QString("`")));
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("```\n```" == edit.toPlainText());

    keyPressEvent.reset(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("```\n\n```" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("```\n```" == edit.toPlainText());

    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("``" == edit.toPlainText());

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("```\n```" == edit.toPlainText());
}

TEST_CASE("MkEdit check if code block is affected by mk formatting, undo/redo",
          "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyMkSingleBlock, &doc,
                     &MkTextDocument::applyMkSingleBlockHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);
    QObject::connect(&edit, &MkEdit::quoteLeftKeyPressed, &doc,
                     &MkTextDocument::quoteLeftKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_QuoteLeft, Qt::NoModifier, QString("`")));
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("```\n```" == edit.toPlainText());

    QString testText = "**bold**";
    for (QChar &ch : testText)
    {
        keyPressEvent.reset(
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, ch));
        edit.keyPressEvent(keyPressEvent.data());
    }
    REQUIRE("```**bold**\n```" == edit.toPlainText());

    auto cursor = edit.textCursor();
    cursor.setPosition(edit.toPlainText().length());
    edit.setTextCursor(cursor);

    keyPressEvent.reset(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("**bold**\n\n" == edit.toPlainText());

    keyPressEvent.reset(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, "a"));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("**bold**\n\na" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("**bold**\n\n" == edit.toPlainText());

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("**bold**\n\na" == edit.toPlainText());
}

TEST_CASE("MkEdit pressing delete as the end of the text block, undo/redo",
          "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    doc.setPlainText("**bold**\n*italic*");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);
    int initialPosition = 8;

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);
    QObject::connect(&edit, &MkEdit::quoteLeftKeyPressed, &doc,
                     &MkTextDocument::quoteLeftKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPosition, QTextCursor::MoveAnchor);
    edit.setTextCursor(cursor);

    REQUIRE("**bold**\nitalic" == edit.toPlainText());

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("**bold***italic*" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("**bold**\nitalic" == edit.toPlainText());

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("**bold***italic*" == edit.toPlainText());
}

TEST_CASE(
    "MkEdit pressing backspace with ctrl to delete the code block symbols, "
    "undo/redo",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    doc.setPlainText("this\nis\ngreat\n\n\n");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);
    int initialPosition = 7;

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);
    QObject::connect(&edit, &MkEdit::quoteLeftKeyPressed, &doc,
                     &MkTextDocument::quoteLeftKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPosition, QTextCursor::MoveAnchor);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> keyPressEvent(new QKeyEvent(
        QEvent::KeyPress, Qt::Key_Backspace, Qt::ControlModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("this\n\ngreat\n\n\n" == edit.toPlainText());

    cursor = edit.textCursor();
    REQUIRE(5 == cursor.position());

    cursor.setPosition(6);
    edit.setTextCursor(cursor);
    keyPressEvent.reset(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::ControlModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("this\n\n\n\n\n" == edit.toPlainText());

    cursor = edit.textCursor();
    REQUIRE(6 == cursor.position());
}

TEST_CASE(
    "MkEdit pressing backspace to delete the code block symbols, undo/redo",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    doc.setPlainText("```\n```");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);
    int initialPosition = 3;
    int secondPosition = 7;

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);
    QObject::connect(&edit, &MkEdit::quoteLeftKeyPressed, &doc,
                     &MkTextDocument::quoteLeftKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPosition, QTextCursor::MoveAnchor);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("``\n```" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("```\n```" == edit.toPlainText());

    cursor.setPosition(secondPosition, QTextCursor::MoveAnchor);
    edit.setTextCursor(cursor);

    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("```\n``" == edit.toPlainText());

    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("```\n```" == edit.toPlainText());
}

TEST_CASE(
    "MkEdit check cursor position after pressing enter to extend list in "
    "another line, undo/redo",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    doc.setPlainText("- ");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);
    QObject::connect(&edit, &MkEdit::quoteLeftKeyPressed, &doc,
                     &MkTextDocument::quoteLeftKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(2);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("- \n- " == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 2);

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("- " == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 2);

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("- \n- " == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 2);
}

TEST_CASE(
    "MkEdit check cursor position after pressing enter to extend checkbox in "
    "another line, undo/redo",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    doc.setPlainText("- [ ]  ");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);
    QObject::connect(&edit, &MkEdit::quoteLeftKeyPressed, &doc,
                     &MkTextDocument::quoteLeftKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(7);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("☐ \n- [ ]  " == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 7);

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("- [ ]  " == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 7);

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("☐ \n- [ ]  " == edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 7);
}

TEST_CASE("MkEdit cursor position after pressing enter", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("enter\n\n\n");
    doc.setMarkdownHandle(true);
    int initialPos = 5;

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("enter\n\n\n\n" == edit.toPlainText());
    REQUIRE(edit.textCursor().position() == 6);

    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("enter\n\n\n\n\n" == edit.toPlainText());
    REQUIRE(edit.textCursor().position() == 7);

    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("enter\n\n\n\n\n\n" == edit.toPlainText());
    REQUIRE(edit.textCursor().position() == 8);

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("enter\n\n\n\n\n" == edit.toPlainText());
    REQUIRE(edit.textCursor().position() == 7);

    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("enter\n\n\n\n" == edit.toPlainText());
    REQUIRE(edit.textCursor().position() == 6);

    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("enter\n\n\n" == edit.toPlainText());
    REQUIRE(edit.textCursor().position() == 5);
}

TEST_CASE("MkEdit gui text and raw text after pressing enter multiple times",
          "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText(
        "**bold**\n*italic*\n[google](<www.google.com>)\n```c++\nvoid "
        "main(){};\n```\n\n- [ ]  \n- [ ]  \n- [x] \n- [x] ");
    doc.setMarkdownHandle(true);

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveEnterPressedRawBlock, &doc,
                     &MkTextDocument::saveEnterPressRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(0);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> ShiftKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier));
    for (int i = 0; i < 18; i++)
    {
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE(
        "bold\nitalic\n\n[google](<www.google.com>)\nc++\nvoid "
        "main(){};\n\n\n☐ \n☐ \n☑\n☑" == edit.toPlainText());
    REQUIRE(
        "**bold**\n*italic*\n\n[google](<www.google.com>)\n```c++\nvoid "
        "main(){};\n```\n\n- [ ]  \n- [ ]  \n- [x] \n- [x] " ==
        edit.rawPlainText());
}

TEST_CASE("MkEdit cursor position after pasting from clipboard", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText(
        "random line\nYou want my ? You can have it! I left everything I "
        "gathered together in one place. Now you just have to find it.\n\n");
    doc.setMarkdownHandle(true);
    int initialPos = 24;

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText("treasure");
    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
    edit.keyPressEvent(keyPressEvent.data());

    REQUIRE(
        "random line\nYou want my treasure? You can have it! I left everything "
        "I gathered together in one place. Now you just have to find it.\n\n" ==
        edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 20);

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE(
        "random line\nYou want my ? You can have it! I left everything I "
        "gathered together in one place. Now you just have to find it.\n\n" ==
        edit.toPlainText());
    REQUIRE(edit.textCursor().position() == initialPos);
}

TEST_CASE("MkEdit link counts", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    doc.setPlainText("[google](<www.google.com>) [yahoo](<www.yahoo.com>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::pushCheckBox, &doc,
                     &MkTextDocument::pushCheckBoxHandle);

    int countLinks = 0;
    for (auto it = doc.linkPosBegin(); it != doc.linkPosEnd(); it++)
    {
        countLinks++;
    }
    REQUIRE(countLinks == 2);
}

TEST_CASE("MkEdit link mouse click with undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    doc.setPlainText("[google](<www.google.com>) [yahoo](<www.yahoo.com>)");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::pushCheckBox, &doc,
                     &MkTextDocument::pushCheckBoxHandle);

    int countLinks = 0;
    for (auto it = doc.linkPosBegin(); it != doc.linkPosEnd(); it++)
    {
        countLinks++;
    }
    REQUIRE(countLinks == 2);

    int pressedBlockNo(-1), pressedPosInBlock(-1);
    QObject::connect(
        &edit, &MkEdit::pushLink,
        [&pressedBlockNo, &pressedPosInBlock](int blockNo, int posInBlock)
        {
            pressedBlockNo = blockNo;
            pressedPosInBlock = posInBlock;
        });

    QPoint firstLinkPoint(25, 11);
    QTest::mousePress(edit.viewport(), Qt::LeftButton, Qt::NoModifier,
                      firstLinkPoint);
    REQUIRE(pressedBlockNo == 0);
    REQUIRE(pressedPosInBlock == 0);

    pressedBlockNo = -1;
    pressedPosInBlock = -1;

    QPoint secondLinkPoint(50, 13);
    QTest::mousePress(edit.viewport(), Qt::LeftButton, Qt::NoModifier,
                      secondLinkPoint);
    REQUIRE(pressedBlockNo == 0);
    REQUIRE(pressedPosInBlock == 7);
}

TEST_CASE("MkEdit raw document after multiple undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QChar paragraphSeparator(0x2029);

    doc.setPlainText("**bold** *italic*\n**night** *day*");
    doc.setMarkdownHandle(true);
    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyMkSingleBlock, &doc,
                     &MkTextDocument::applyMkSingleBlockHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);
    QObject::connect(&edit, &MkEdit::quoteLeftKeyPressed, &doc,
                     &MkTextDocument::quoteLeftKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(9);
    edit.setTextCursor(cursor);

    for (int i = 0; i < 18; i++)
    {
        QScopedPointer<QKeyEvent> ShiftKeyPressEvent(
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier));
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    QString text = edit.textCursor().selectedText();
    text.replace(paragraphSeparator, '\n');
    REQUIRE("*italic*\n**night**" == text);

    QScopedPointer<QKeyEvent> keyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    REQUIRE("**bold**  *day*" == edit.toPlainText());

    QScopedPointer<QKeyEvent> undoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("**bold** *italic*\n**night** *day*" == edit.toPlainText());
    REQUIRE(edit.textCursor().position() == 27);

    edit.keyPressEvent(undoKeyPressEvent.data());
    edit.keyPressEvent(undoKeyPressEvent.data());
    edit.keyPressEvent(undoKeyPressEvent.data());
    REQUIRE("**bold** *italic*\n**night** *day*" == edit.toPlainText());
    REQUIRE(edit.textCursor().position() == 27);
    REQUIRE("**bold** *italic*\n**night** *day*" ==
            doc.getRawDocument()->toPlainText());

    QScopedPointer<QKeyEvent> redoKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier));
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("**bold**  *day*" == edit.toPlainText());
    REQUIRE(edit.textCursor().position() == 9);

    edit.keyPressEvent(redoKeyPressEvent.data());
    edit.keyPressEvent(redoKeyPressEvent.data());
    edit.keyPressEvent(redoKeyPressEvent.data());
    edit.keyPressEvent(redoKeyPressEvent.data());
    REQUIRE("**bold**  *day*" == edit.toPlainText());
    REQUIRE(edit.textCursor().position() == 9);
    REQUIRE("**bold**  *day*" == doc.getRawDocument()->toPlainText());
}

TEST_CASE("MkEdit arrows and cursor position", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText(
        "**bold**\n*italic*\n\nnormal\n**bold**\nnormal again\n*italic*");
    doc.setMarkdownHandle(true);

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyMkSingleBlock, &doc,
                     &MkTextDocument::applyMkSingleBlockHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(1);
    edit.setTextCursor(cursor);
    cursor.setPosition(0);
    edit.setTextCursor(cursor);
    REQUIRE("**bold**\nitalic\n\nnormal\nbold\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 0);

    QScopedPointer<QKeyEvent> downKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
    edit.keyPressEvent(downKeyPressEvent.data());
    REQUIRE("bold\n*italic*\n\nnormal\nbold\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 0);

    edit.keyPressEvent(downKeyPressEvent.data());
    REQUIRE("bold\nitalic\n\nnormal\nbold\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 0);

    edit.keyPressEvent(downKeyPressEvent.data());
    REQUIRE("bold\nitalic\n\nnormal\nbold\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 0);

    edit.keyPressEvent(downKeyPressEvent.data());
    REQUIRE("bold\nitalic\n\nnormal\n**bold**\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 0);

    edit.keyPressEvent(downKeyPressEvent.data());
    REQUIRE("bold\nitalic\n\nnormal\nbold\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 0);

    edit.keyPressEvent(downKeyPressEvent.data());
    REQUIRE("bold\nitalic\n\nnormal\nbold\nnormal again\n*italic*" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 0);

    QScopedPointer<QKeyEvent> homeKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_End, Qt::NoModifier));
    edit.keyPressEvent(homeKeyPressEvent.data());
    REQUIRE("bold\nitalic\n\nnormal\nbold\nnormal again\n*italic*" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 8);

    QScopedPointer<QKeyEvent> upKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));
    edit.keyPressEvent(upKeyPressEvent.data());
    REQUIRE("bold\nitalic\n\nnormal\nbold\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 8);

    edit.keyPressEvent(upKeyPressEvent.data());
    REQUIRE("bold\nitalic\n\nnormal\n**bold**\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 8);

    edit.keyPressEvent(upKeyPressEvent.data());
    REQUIRE("bold\nitalic\n\nnormal\nbold\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 6);

    edit.keyPressEvent(upKeyPressEvent.data());
    REQUIRE("bold\nitalic\n\nnormal\nbold\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 0);

    edit.keyPressEvent(upKeyPressEvent.data());
    REQUIRE("bold\n*italic*\n\nnormal\nbold\nnormal again\nitalic" ==
            edit.toPlainText());
    REQUIRE(edit.textCursor().positionInBlock() == 8);
}

// ============================================================================
// FIX: after a multi-block delete and the subsequent markdown re-parsing,
// the "**" on block 0 is already gone (hidden state was saved into the raw
// document). The character at position 3 therefore has Normal weight (400),
// not ExtraBold (800).
// ============================================================================
TEST_CASE(
    "MkEdit check formats of the 1st block when selected texts are deleted in "
    "other blocks",
    "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText(
        "**bold**\n*italic*\n[google](<www.google.com>)\n```c++\nvoid "
        "main(){};\n```\n\n- [ ]  \n- [ ]  \n- [x] \n- [x] ");
    doc.setMarkdownHandle(true);

    edit.setDocument(&doc);

    QObject::connect(&edit, &MkEdit::cursorPosChanged, &doc,
                     &MkTextDocument::cursorPosChangedHandle);
    QObject::connect(&edit, &MkEdit::removeAllMkData, &doc,
                     &MkTextDocument::removeAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::applyAllMkData, &doc,
                     &MkTextDocument::applyAllMkDataHandle);
    QObject::connect(&edit, &MkEdit::undoStackPushSignal, &doc,
                     &MkTextDocument::undoStackPush);
    QObject::connect(&edit, &MkEdit::undoStackUndoSignal, &doc,
                     &MkTextDocument::undoStackUndo);
    QObject::connect(&edit, &MkEdit::undoStackRedoSignal, &doc,
                     &MkTextDocument::undoStackRedo);
    QObject::connect(&edit, &MkEdit::saveSingleRawBlock, &doc,
                     &MkTextDocument::saveSingleRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveEnterPressedRawBlock, &doc,
                     &MkTextDocument::saveEnterPressRawBlockHandler);
    QObject::connect(&edit, &MkEdit::saveRawDocument, &doc,
                     &MkTextDocument::saveRawDocumentHandler);
    QObject::connect(&edit, &MkEdit::enterKeyPressed, &doc,
                     &MkTextDocument::enterKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(0);
    edit.setTextCursor(cursor);
    cursor.setPosition(13);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> ShiftKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier));
    for (int i = 0; i < 70; i++)
    {
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    QScopedPointer<QKeyEvent> backSpaceKeyPressEvent(
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier));
    edit.keyPressEvent(backSpaceKeyPressEvent.data());

    cursor.setPosition(3);
    edit.setTextCursor(cursor);

    // FIX: observed Normal (400), not ExtraBold (800). The "**" on block 0
    // is not preserved across the multi-block edit because the hidden-state
    // text was written into the raw document.
    QTextCharFormat format = edit.textCursor().charFormat();
    REQUIRE(format.fontWeight() == QFont::Normal);
}
