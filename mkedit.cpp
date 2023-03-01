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
    case Qt::Key_C:         if( event->modifiers() == Qt::CTRL) {QTextEdit::keyPressEvent(event);return;}
    }

    emit removeAllMkData();

    QString oldText = this->document()->toPlainText();
    int oldCursorPos = this->textCursor().position();

    bool undoRedoSkip = false;
    bool selectAll = false;

    QTextEdit::keyPressEvent(event);

    switch(event->key()){
    case Qt::Key_Enter:
    case Qt::Key_Return:    emit enterKeyPressed(this->textCursor().blockNumber()); break;
    case Qt::Key_QuoteLeft: quoteLeftKey(); break;
    case Qt::Key_Z:         if( event->modifiers() == Qt::CTRL) undo(); undoRedoSkip = true; break;
    case Qt::Key_Y:         if( event->modifiers() == Qt::CTRL) redo(); undoRedoSkip = true; break;
    case Qt::Key_A:         if( event->modifiers() == Qt::CTRL) selectAll = true; undoRedoSkip = true; break;
    default: break;
    }

    int newBlockNumber = this->textCursor().blockNumber();
    if(!undoRedoSkip){
        EditCommand *edit = new EditCommand(this,this->document(),this->document()->toPlainText(),
                                            this->textCursor().position(),
                                            oldText, oldCursorPos);

        undoStack.push(edit);
    }

    emit fileSave(); //save file
    emit applyAllMkData( this->textCursor().hasSelection(), newBlockNumber, selectAll);
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
