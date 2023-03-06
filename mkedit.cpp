#include "blockdata.h"
#include "linedata.h"
#include "mkedit.h"


void MkEdit::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    bool drawBlock = false;
    int xBlock =0, yBlock =0;
    int fontSize = this->currentFont().pointSize();
    int scrollPos = this->verticalScrollBar()->value();
    penCodeBlock.setColor(codeBlockColor);

    QTextBlock block = this->document()->begin();
    while (block.isValid()) {

        QTextBlockUserData* data =block.userData();
        BlockData* blockData = dynamic_cast<BlockData*>(data);
        if(blockData){
            if(blockData->getStatus()==BlockData::start){
                xBlock = block.layout()->position().x()-2;
                yBlock = block.layout()->position().y()-scrollPos-2;
            }
            else if(blockData->getStatus()==BlockData::end){
                int height = block.layout()->position().y() - yBlock + (fontSize*0.4)-scrollPos;

                QBrush brushDefault(codeBlockColor);
                painter.setBrush(brushDefault);
                painter.setPen(penCodeBlock);
                painter.drawRoundedRect(xBlock,yBlock,widthCodeBlock,height,BLOCKRADIUS,BLOCKRADIUS);
            }
        }else{
            LineData* lineData = dynamic_cast<LineData*>(data);
            if(lineData){
                if(lineData->getStatus() == LineData::horizontalLine && lineData->getDraw()){
                    int lineX1 = block.layout()->position().x()-2;
                    int lineY1 = block.layout()->position().y()+fontSize-scrollPos;
                    int lineX2 = block.layout()->position().x()-2+widthCodeBlock;
                    painter.setPen(penCodeBlock);
                    painter.drawLine(lineX1,lineY1,lineX2,lineY1);
                }
            }
        }
        block = block.next();
    }

    QTextEdit::paintEvent(e);
}

void MkEdit::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);
    widthCodeBlock = event->size().width() - this->verticalScrollBar()->width();
}

void MkEdit::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier) {
        int zoomDelta = e->angleDelta().y();
        if (zoomDelta > 0) {
           this->zoomIn();
        } else {
           this->zoomOut();
        }
        emit removeAllMkData();
        emit applyAllMkData( this->textCursor().hasSelection(), this->textCursor().blockNumber(), false);
    }else{
        QTextEdit::wheelEvent(e);
    }
}

void MkEdit::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Alt:       QTextEdit::keyPressEvent(event);return;
    case Qt::Key_V:
    case Qt::Key_C:         if( event->modifiers() == Qt::CTRL) {QTextEdit::keyPressEvent(event);return;}
    }

    emit removeAllMkData();

    preUndoSetup();
    QTextEdit::keyPressEvent(event);
    switch(event->key()){
    case Qt::Key_Enter:
    case Qt::Key_Return:    emit enterKeyPressed(this->textCursor().blockNumber()); break;
    case Qt::Key_QuoteLeft: quoteLeftKey(); break;
    case Qt::Key_Z:         if( event->modifiers() == Qt::CTRL) undo(); undoData.undoRedoSkip = true; break;
    case Qt::Key_Y:         if( event->modifiers() == Qt::CTRL) redo(); undoData.undoRedoSkip = true; break;
    case Qt::Key_A:         if( event->modifiers() == Qt::CTRL) undoData.selectAll = true; undoData.undoRedoSkip = true; break;
    default: break;
    }

    int blockNumber = this->textCursor().blockNumber();
    postUndoSetup();

    emit fileSave();
    emit applyAllMkData( this->textCursor().hasSelection(), blockNumber, undoData.selectAll);
}

void MkEdit::quoteLeftKey()
{
    bool success =false;
    emit quoteLeftKeyPressed(this->textCursor().blockNumber(),success);
    if(success){
        QTextCursor textCursor = this->textCursor();
        textCursor.movePosition(QTextCursor::PreviousBlock);
        this->setTextCursor(textCursor);
    }
}

void MkEdit::preUndoSetup()
{
    undoData.view               = this;
    undoData.doc                = this->document();
    undoData.oldText            = this->document()->toPlainText();
    undoData.oldCursorPos       = this->textCursor().position();
    undoData.oldStartSelection  = this->textCursor().selectionStart();
    undoData.oldEndSelection    = this->textCursor().selectionEnd();
    undoData.undoRedoSkip       = false;
    undoData.selectAll          = false;
}

void MkEdit::postUndoSetup()
{
    undoData.text               = this->document()->toPlainText();
    undoData.cursorPos          = this->textCursor().position();

    if(!undoData.undoRedoSkip){
        EditCommand *edit = new EditCommand(undoData);
        undoStack.push(edit);
    }
}

QColor MkEdit::blockColor() const
{
    return codeBlockColor;
}

void MkEdit::blockColor(const QColor &color)
{
    if (codeBlockColor != color) {
        codeBlockColor = color;
        emit blockColorChanged(codeBlockColor);
        update();
    }
}

void MkEdit::undo()
{
    undoStack.undo();
}

void MkEdit::redo()
{
    undoStack.redo();
}

void MkEdit::clearUndoStackHandle()
{
    undoStack.clear();
}

void MkEdit::cursorPositionChangedHandle()
{
    int currentBlockNumber = textCursor().blockNumber();

    if(savedBlockNumber != currentBlockNumber){
        savedBlockNumber = currentBlockNumber;
        emit cursorPosChanged( textCursor().hasSelection(), currentBlockNumber);
        this->update();
    }
}
