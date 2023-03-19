#ifndef EDITCOMMAND_H
#define EDITCOMMAND_H

#include <QTextBlock>
#include <QTextEdit>
#include <mktextdocument.h>
#include <QUndoCommand>
#include <QScrollBar>

struct UndoData{
    QTextEdit *view;
    QTextDocument *doc;
    QString text;
    int cursorPos;
    QString oldText;
    int oldCursorPos;
    int oldStartSelection;
    int oldEndSelection;
    bool undoRedoSkip;
    bool selectAll;
    int scrollValue;
};

class EditCommand : public QUndoCommand
{
public:
    EditCommand(QTextEdit *view,QTextDocument *doc, QString text, int cursorPos,
                QString oldText, int oldCursorPos,
                QUndoCommand *parent = nullptr);
    EditCommand(UndoData &data);

    void undo() override;
    void redo() override;

private:
    QTextEdit *view;
    MkTextDocument *doc;
    QString text;
    int cursorPos;
    int scrollValue;

    QString oldText;
    int oldCursorPos;
    int oldStartSelection;
    int oldEndSelection;
};

#endif // EDITCOMMAND_H
