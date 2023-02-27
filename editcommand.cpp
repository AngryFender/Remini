#include "editcommand.h"

EditCommand::EditCommand(QTextEdit *view, QTextDocument *doc, QString text, int cursorPos, QString oldText, int oldCursorPos, QUndoCommand *parent):QUndoCommand(parent)
{
    this->view = view;
    this->doc = dynamic_cast<MkTextDocument*>(doc);
    this->text = text;
    this->cursorPos = cursorPos;

    this->oldText = oldText;
    this->oldCursorPos = oldCursorPos;   
}

void EditCommand::undo()
{
    qDebug()<<"text "<< text <<" pos " << cursorPos << " oldText "<<oldText << "old pos" << oldCursorPos ;
    doc->setUndoRedoText(oldText);
    QTextCursor textCursor = view->textCursor();
    textCursor.setPosition(oldCursorPos);
    view->setTextCursor(textCursor);
}

void EditCommand::redo()
{
    qDebug()<<"text "<< text <<" pos " << cursorPos << " oldText "<<oldText << "old pos" << oldCursorPos ;
    doc->setUndoRedoText(text);
    QTextCursor textCursor = view->textCursor();
    textCursor.setPosition(cursorPos);
    view->setTextCursor(textCursor);
}
