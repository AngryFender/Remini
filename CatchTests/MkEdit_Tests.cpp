#include <catch2/catch.hpp>
#include "mkedit.h"
#include <QApplication>
#include <QClipboard>
#include <QScopedPointer>
#include <QTest>

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

    doc.setPlainText("**bold** _italic_ ~~strike~~ [google](<https://www.google.com/>) \n **new line**");
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
    REQUIRE("**bold** _italic_ ~~strike~~ [google](<https://www.google.com/>) \n new line" == text1);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == cursorPosition + symbolLength);
}

TEST_CASE("MkEdit move cursor to the middle of the characters of fifth Markdown word which is another link", "[MkTextDocument]")
{
    MkTextDocument doc;
    MkEdit edit;
    int symbolLength = 2+2+1+1+2+2+1+28+1;   // '**' + '**' + '_' + '_' + '~~' + '~~' + '[' + '](<https://www.google.com/>)' + '['
    int cursorPosition = 32;     // bold italic strike google youtub
    int newLinePos = 106;

    doc.setPlainText("**bold** _italic_ ~~strike~~ [google](<https://www.google.com/>) [youtube](<https://www.youtube.com/>) \n **new line**");
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
    REQUIRE("**bold** _italic_ ~~strike~~ [google](<https://www.google.com/>) [youtube](<https://www.youtube.com/>) \n new line" == text1);

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
    int symbolLength = 1+28+1+28+1;   // '[' + '](<https://www.google.com/>)' + '[' + '](<https://www.google.com/>)' + '['
    int cursorPosition = 17;     // google google goo
    int newLinePos = 194;

    doc.setPlainText("[google](<https://www.google.com/>) [google](<https://www.google.com/>) [google](<https://www.google.com/>) [google](<https://www.google.com/>) [google](<https://www.google.com/>) hello \n **new line**");
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
    REQUIRE("[google](<https://www.google.com/>) [google](<https://www.google.com/>) [google](<https://www.google.com/>) [google](<https://www.google.com/>) [google](<https://www.google.com/>) hello \n new line" == text1);

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

TEST_CASE("MkEdit insert new line bullet point", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("- first");
    int initialPos = 7;

    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::enterKeyPressed,
                     &doc,&MkTextDocument::enterKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("- first\n- " == text);
}

TEST_CASE("MkEdit insert new line bullet point with spaces infront", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("      - first");
    int initialPos = 13;

    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::enterKeyPressed,
                     &doc,&MkTextDocument::enterKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("      - first\n      - " == text);
}

TEST_CASE("MkEdit insert new line bullet point, undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("- first");
    int initialPos = 7;

    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveSingleRawBlock,
                     &doc,&MkTextDocument::saveSingleRawBlockHandler);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QObject::connect(&edit,&MkEdit::enterKeyPressed,
                     &doc,&MkTextDocument::enterKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("- first\n- " == text);

    QScopedPointer<QKeyEvent> undoKeyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("- first" == text);


    QScopedPointer<QKeyEvent>  redoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier)) ;
    edit.keyPressEvent(redoKeyPressEvent.data());
    edit.keyPressEvent(redoKeyPressEvent.data());

    text = edit.toPlainText();
    REQUIRE("- first\n- " == text);
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
    REQUIRE("[](<https://www.google.com/>)" == text);
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
    REQUIRE("[](<file:///C:\\Users\\Public>)" == text);
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
    REQUIRE("[](<https://www.google.com/>)" == text);

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
    REQUIRE("[](<file:///C:\\Users\\Public>)" == text);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == desiredPos);
}

TEST_CASE("MkEdit using tab to insert links symbols ", "[MkEdit]")
{

    MkTextDocument doc;
    MkEdit edit;
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));

    doc.setPlainText("lk");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::autoInsertSymbol,
                     &doc,&MkTextDocument::autoInsertSymobolHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(2);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> tabKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, QString("    ")));
    edit.keyPressEvent(tabKeyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("[](<>)" == text);
}

TEST_CASE("MkEdit undo after using tab to insert links symbols ", "[MkEdit]")
{

    MkTextDocument doc;
    MkEdit edit;
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));

    doc.setPlainText("lk");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::autoInsertSymbol,
                     &doc,&MkTextDocument::autoInsertSymobolHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(2);
    edit.setTextCursor(cursor);

    QScopedPointer<QKeyEvent> tabKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, QString("    ")));
    edit.keyPressEvent(tabKeyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("[](<>)" == text);

    QScopedPointer<QKeyEvent>  undoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("lk" == text);
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

TEST_CASE("MkEdit undo after typing inside bold format then check if the cursor is at the right place", "[MkEdit]")
{

    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 4; //**bo
    int desiredPos = 5; //**bop
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));

    doc.setPlainText("**bold**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());

    QString text = edit.toPlainText();
    REQUIRE("**bopld**" == text);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == desiredPos);

    QScopedPointer<QKeyEvent>  undoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("**bold**" == text);
}

TEST_CASE("MkEdit redo after undo after typing inside bold format then check if the cursor is at the right place", "[MkEdit]")
{

    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 4; //**bo
    int desiredPos = 5; //**bop
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("p")));

    doc.setPlainText("**bold**");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    edit.keyPressEvent(keyPressEvent.data());

    QString text = edit.toPlainText();
    REQUIRE("**bopld**" == text);

    int currentPositionOfTextCursorInBlock = edit.textCursor().positionInBlock();
    REQUIRE(currentPositionOfTextCursorInBlock == desiredPos);

    QScopedPointer<QKeyEvent>  undoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("**bold**" == text);

    QScopedPointer<QKeyEvent>  redoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier)) ;
    edit.keyPressEvent(redoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("**bopld**" == text);
}

TEST_CASE("MkEdit selection check for undo after typing inside bold format then check if the cursor is at the right place", "[MkEdit]")
{

    MkTextDocument doc;
    MkEdit edit;
    QChar paragraphSeparator(0x2029);
    int initialPos = 4; //**bo

    doc.setPlainText("**bold**\n*italic*");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    for(int i = 0; i < 11; i ++){
        QScopedPointer<QKeyEvent>  ShiftKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier)) ;
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    QString text = edit.textCursor().selectedText();
    text.replace(paragraphSeparator, '\n');
    REQUIRE("ld**\n*itali" == text);

    QScopedPointer<QKeyEvent> randomKeyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("d")));
    edit.keyPressEvent(randomKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("**bodc*" == text);

    QScopedPointer<QKeyEvent>  undoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("**bold**\n*italic*" == text);

    QString selectedTextAfterUndo = edit.textCursor().selectedText();
    selectedTextAfterUndo.replace(paragraphSeparator, '\n');
    REQUIRE("ld**\n*itali" == selectedTextAfterUndo);
}

TEST_CASE("MkEdit paste from clipboard into MkEdit", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 0;
    QString clipboardText = "test data";

    doc.setPlainText("");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    // Set text to the clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(clipboardText);

    // Simulate paste from clipboard
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("test data" == text);
}

TEST_CASE("MkEdit undo paste from clipboard into MkEdit", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 0;
    QString clipboardText = "test data";

    doc.setPlainText("");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    // Set text to the clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(clipboardText);

    // Simulate paste from clipboard
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("test data" == text);

    QScopedPointer<QKeyEvent>  undoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("" == text);
}

TEST_CASE("MkEdit redo paste from clipboard into MkEdit", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 0;
    QString clipboardText= "test data";

    doc.setPlainText("");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(clipboardText);
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("test data" == text);

    QScopedPointer<QKeyEvent>  undoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("" == text);

    QScopedPointer<QKeyEvent>  redoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier)) ;
    edit.keyPressEvent(redoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("test data" == text);
}

TEST_CASE("MkEdit correct cursor position for undo/redo paste from clipboard into MkEdit", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 23;
    QString clipboardText = "caterpillar";

    doc.setPlainText("I Turned Myself Into A Pickle, Morty!");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    //select the word "Pickle"
    for(int i = 0; i < 6; i ++){
        QScopedPointer<QKeyEvent>  ShiftKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier)) ;
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    // Set text to the clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(clipboardText);

    // Simulate paste from clipboard
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("I Turned Myself Into A caterpillar, Morty!" == text);

    QScopedPointer<QKeyEvent>  undoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("I Turned Myself Into A Pickle, Morty!" == text);

    QString selectedText = edit.textCursor().selectedText();
    REQUIRE("Pickle" == selectedText);
}

TEST_CASE("MkEdit correct multiple lines, cursor position for undo/redo paste from clipboard into MkEdit", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    QChar paragraphSeparator(0x2029);
    int initialPos = 23;
    QString clipboardText = "Wubba Lubba Dub Dub!";

    doc.setPlainText("I Turned Myself Into A Pickle, Morty!\nHe Turned Himself Into Akira!\nBut Life Is Made Of Little Concessions");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    //select the word "Pickle"
    for(int i = 0; i < 83; i ++){
        QScopedPointer<QKeyEvent>  ShiftKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier)) ;
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    // Set text to the clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(clipboardText);

    // Simulate paste from clipboard
    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));

    edit.keyPressEvent(keyPressEvent.data());
    QString text = edit.toPlainText();
    REQUIRE("I Turned Myself Into A Wubba Lubba Dub Dub!" == text);

    QScopedPointer<QKeyEvent>  undoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("I Turned Myself Into A Pickle, Morty!\nHe Turned Himself Into Akira!\nBut Life Is Made Of Little Concessions" == text);

    QString selectedText = edit.textCursor().selectedText();
    selectedText.replace(paragraphSeparator, '\n');
    REQUIRE("Pickle, Morty!\nHe Turned Himself Into Akira!\nBut Life Is Made Of Little Concessions" == selectedText);
}


TEST_CASE("MkEdit selection check after double undo", "[MkEdit]")
{

    MkTextDocument doc;
    MkEdit edit;
    QChar paragraphSeparator(0x2029);
    int initialPos = 4; //**bo

    doc.setPlainText("**bold**\n*italic*\n~~crossed~~");
    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);

    for(int i = 0; i < 11; i ++){
        QScopedPointer<QKeyEvent>  ShiftKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier)) ;
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }

    QString text = edit.textCursor().selectedText();
    text.replace(paragraphSeparator, '\n');
    REQUIRE("ld**\n*itali" == text);

    QScopedPointer<QKeyEvent> randomKeyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("d")));
    edit.keyPressEvent(randomKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("**bodc*\ncrossed" == text);

    for(int i = 0; i < 11; i ++){
        QScopedPointer<QKeyEvent>  ShiftKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier)) ;
        edit.keyPressEvent(ShiftKeyPressEvent.data());
    }
    text = edit.textCursor().selectedText();
    text.replace(paragraphSeparator, '\n');
    REQUIRE("c*\n~~crosse" == text);

    QScopedPointer<QKeyEvent> randomKeyPressEvent2 (new QKeyEvent(QEvent::KeyPress, Qt::Key_Any, Qt::NoModifier, QString("d")));
    edit.keyPressEvent(randomKeyPressEvent2.data());

    int currentPosition = edit.textCursor().position();
    int expectedPosition = 6;

    REQUIRE( currentPosition == expectedPosition);

    text = edit.toPlainText();
    REQUIRE("**boddd~~" == text);

    QScopedPointer<QKeyEvent>  undoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("**bodc*\n~~crossed~~" == text);

    QString selectedTextAfterUndo = edit.textCursor().selectedText();
    selectedTextAfterUndo.replace(paragraphSeparator, '\n');
    REQUIRE("c*\n~~crosse" == selectedTextAfterUndo);

    QScopedPointer<QKeyEvent>  undoKeyPressEvent2(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent2.data());
    text = edit.toPlainText();
    REQUIRE("**bold**\n*italic*\ncrossed" == text);

    selectedTextAfterUndo = edit.textCursor().selectedText();
    selectedTextAfterUndo.replace(paragraphSeparator, '\n');
    REQUIRE("ld**\n*itali" == selectedTextAfterUndo);
}


TEST_CASE("MkEdit type all strings with bold format then check if the cursor is at the right place", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int desiredPos = 8; //**bop

    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QObject::connect(&edit,&MkEdit::saveSingleRawBlock,
                     &doc,&MkTextDocument::saveSingleRawBlockHandler);


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

TEST_CASE("MkEdit checkbox mouse click with undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    int initialPos = 30;

    edit.setDocument(&doc);
    doc.setPlainText("- [x]  option1\n- [x]  option2\n");

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QObject::connect(&edit,&MkEdit::saveSingleRawBlock,
                     &doc,&MkTextDocument::saveSingleRawBlockHandler);

    QObject::connect(&edit,&MkEdit::pushCheckBox,
                     &doc,&MkTextDocument::pushCheckBoxHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);


    QAbstractTextDocumentLayout* layout = doc.documentLayout();
    QTextBlock block1 = doc.findBlockByNumber(0);
    QTextBlock block2 = doc.findBlockByNumber(1);

    QRectF firstRect = layout->blockBoundingRect(block1);
    QRectF secondRect = layout->blockBoundingRect(block2);
    QRect combineRect(0, 0, firstRect.width() + 10, firstRect.height() + secondRect.height() + 10 );

    // apply markdown formats and update checkbox positions from screen to text cursor position
    doc.applyAllMkDataHandle(3, false, nullptr);

    int countCheckBoxes = 0;
    for(auto it = doc.checkMarkPosBegin(); it!= doc.checkMarkPosEnd(); it++){
        countCheckBoxes++;
    }
    REQUIRE(countCheckBoxes>0);

    QString text = edit.toPlainText();
    REQUIRE("☑ option1\n☑ option2\n"==text);

    //mouse press on the checkbox to remove the tick
    QPoint firstCheckBocPoint(6,6);
    QTest::mousePress(edit.viewport(), Qt::LeftButton,Qt::NoModifier,firstCheckBocPoint);

    text = edit.toPlainText();
    REQUIRE("☐ option1\n☑ option2\n"==text);

    //check if the raw document is updated from the mouse click
    text = doc.getRawDocument()->toPlainText();
    REQUIRE("- [ ]  option1\n- [x]  option2\n" == text);

    //undo
    QScopedPointer<QKeyEvent>  undoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier)) ;
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("☑ option1\n☑ option2\n"==text);

    text = doc.getRawDocument()->toPlainText();
    REQUIRE("- [x]  option1\n- [x]  option2\n" == text);

    //redo
    QScopedPointer<QKeyEvent>  redoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier)) ;
    edit.keyPressEvent(redoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("☐ option1\n☑ option2\n"==text);

    text = doc.getRawDocument()->toPlainText();
    REQUIRE("- [ ]  option1\n- [x]  option2\n" == text);
}

TEST_CASE("MkEdit press backspace in the first position of the text block, undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;
    doc.setPlainText("**bold**\n *italic*");
    int initialPos = 9;

    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveSingleRawBlock,
                     &doc,&MkTextDocument::saveSingleRawBlockHandler);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QObject::connect(&edit,&MkEdit::enterKeyPressed,
                     &doc,&MkTextDocument::enterKeyPressedHandle);


    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPos);
    edit.setTextCursor(cursor);
    QString text = edit.toPlainText();
    REQUIRE("bold\n *italic*" == text);

    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress,Qt::Key_Backspace, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("**bold** *italic*" == text);

    QScopedPointer<QKeyEvent> undoKeyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("bold\n *italic*" == text);

    QScopedPointer<QKeyEvent>  redoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier)) ;
    edit.keyPressEvent(redoKeyPressEvent.data());

    text = edit.toPlainText();
    REQUIRE("**bold** *italic*" == text);
}

TEST_CASE("MkEdit create code block with ```, undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveSingleRawBlock,
                     &doc,&MkTextDocument::saveSingleRawBlockHandler);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QObject::connect(&edit,&MkEdit::enterKeyPressed,
                     &doc,&MkTextDocument::enterKeyPressedHandle);

    QObject::connect(&edit,&MkEdit::quoteLeftKeyPressed,
                     &doc,&MkTextDocument::quoteLeftKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_QuoteLeft, Qt::NoModifier,QString("`")));
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());

    QString text = edit.toPlainText();
    text = edit.toPlainText();
    REQUIRE("```\n```" == text);

    QScopedPointer<QKeyEvent> undoKeyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("``" == text);

    QScopedPointer<QKeyEvent>  redoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier)) ;
    edit.keyPressEvent(redoKeyPressEvent.data());

    text = edit.toPlainText();
    REQUIRE("```\n```" == text);
}

TEST_CASE("MkEdit pressing enter after creating code block with ```, undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    edit.setDocument(&doc);

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveSingleRawBlock,
                     &doc,&MkTextDocument::saveSingleRawBlockHandler);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QObject::connect(&edit,&MkEdit::enterKeyPressed,
                     &doc,&MkTextDocument::enterKeyPressedHandle);

    QObject::connect(&edit,&MkEdit::quoteLeftKeyPressed,
                     &doc,&MkTextDocument::quoteLeftKeyPressedHandle);

    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_QuoteLeft, Qt::NoModifier,QString("`")));
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());
    edit.keyPressEvent(keyPressEvent.data());

    QString text = edit.toPlainText();
    text = edit.toPlainText();
    REQUIRE("```\n```" == text);

    keyPressEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("```\n\n```" == text);

    QScopedPointer<QKeyEvent> undoKeyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("```\n```" == text);

    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("``" == text);

    QScopedPointer<QKeyEvent>  redoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier)) ;
    edit.keyPressEvent(redoKeyPressEvent.data());

    text = edit.toPlainText();
    REQUIRE("```\n```" == text);
}

TEST_CASE("MkEdit pressing delete as the end of the text block, undo/redo", "[MkEdit]")
{
    MkTextDocument doc;
    MkEdit edit;

    doc.setPlainText("**bold**\n*italic*");
    edit.setDocument(&doc);
    int initialPosition = 8;

    QObject::connect(&edit,&MkEdit::cursorPosChanged,
                     &doc,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(&edit,&MkEdit::removeAllMkData,
                     &doc,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::applyAllMkData,
                     &doc,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(&edit,&MkEdit::undoStackPushSignal,
                     &doc,&MkTextDocument::undoStackPush);

    QObject::connect(&edit,&MkEdit::undoStackUndoSignal,
                     &doc,&MkTextDocument::undoStackUndo);

    QObject::connect(&edit,&MkEdit::undoStackRedoSignal,
                     &doc,&MkTextDocument::undoStackRedo);

    QObject::connect(&edit,&MkEdit::saveSingleRawBlock,
                     &doc,&MkTextDocument::saveSingleRawBlockHandler);

    QObject::connect(&edit,&MkEdit::saveRawDocument,
                     &doc,&MkTextDocument::saveRawDocumentHandler);

    QObject::connect(&edit,&MkEdit::enterKeyPressed,
                     &doc,&MkTextDocument::enterKeyPressedHandle);

    QObject::connect(&edit,&MkEdit::quoteLeftKeyPressed,
                     &doc,&MkTextDocument::quoteLeftKeyPressedHandle);

    QTextCursor cursor = edit.textCursor();
    cursor.setPosition(initialPosition, QTextCursor::MoveAnchor);
    edit.setTextCursor(cursor);

    QString text = edit.toPlainText();
    REQUIRE("**bold**\nitalic"==text) ;

    QScopedPointer<QKeyEvent> keyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier));
    edit.keyPressEvent(keyPressEvent.data());

    text = edit.toPlainText();
    REQUIRE("**bold***italic*" == text);

    QScopedPointer<QKeyEvent> undoKeyPressEvent (new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
    edit.keyPressEvent(undoKeyPressEvent.data());
    text = edit.toPlainText();
    REQUIRE("**bold**\nitalic" == text);

    QScopedPointer<QKeyEvent>  redoKeyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier)) ;
    edit.keyPressEvent(redoKeyPressEvent.data());

    text = edit.toPlainText();
    REQUIRE("**bold***italic*" == text);
}
