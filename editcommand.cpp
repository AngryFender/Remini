#include "editcommand.h"

EditCommand::EditCommand(UndoData &data)
{
    this->view = data.view;
    this->doc = dynamic_cast<MkTextDocument*>(data.doc);
    this->text = data.text;
    this->cursorPos = data.cursorPos;
    this->scrollValue = data.scrollValue;

    this->oldText = data.oldText;
    this->oldCursorPos = data.oldCursorPos;
    this->oldStartSelection = data.oldStartSelection;
    this->oldEndSelection = data.oldEndSelection;
    isConstructorRedo = true;

}

void EditCommand::undo()
{
    doc->setUndoRedoText(oldText);
    QTextCursor textCursor = view->textCursor();
    if(oldCursorPos == oldStartSelection){
        textCursor.setPosition(oldEndSelection);
        textCursor.setPosition(oldCursorPos,QTextCursor::KeepAnchor);
    }else{
        textCursor.setPosition(oldStartSelection);
        textCursor.setPosition(oldEndSelection,QTextCursor::KeepAnchor);
    }
    view->setTextCursor(textCursor);
}

void EditCommand::redo()
{
    if(isConstructorRedo){
        isConstructorRedo = false;
    }else{
        doc->setUndoRedoText(text);
        QTextCursor textCursor = view->textCursor();
        textCursor.setPosition(cursorPos);
        view->setTextCursor(textCursor);
    }
}
