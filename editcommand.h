#ifndef EDITCOMMAND_H
#define EDITCOMMAND_H

#include <QTextBlock>
#include <QTextEdit>
#include <mktextdocument.h>
#include <QUndoCommand>

class EditCommand : public QUndoCommand
{
public:
    EditCommand(QTextEdit *view,QTextDocument *doc, QString text, int cursorPos,
                QString oldText, int oldCursorPos,
                QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    QTextEdit *view;
    MkTextDocument *doc;
    QString text;
    int cursorPos;

    QString oldText;
    int oldCursorPos;
};

#endif // EDITCOMMAND_H
