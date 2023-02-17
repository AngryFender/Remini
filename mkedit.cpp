#include "blockdata.h"
#include "linedata.h"
#include "mkedit.h"


void MkEdit::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QTextBlock block = this->document()->begin();
    bool drawBlock = false;
    int xBlock =0, yBlock =0;

    int codeId = 1;
    while (block.isValid()) {
        QTextBlockUserData* data =block.userData();
        BlockData* blockData = dynamic_cast<BlockData*>(data);
        if(blockData){
            if(blockData->getStatus()==BlockData::start){
                xBlock = block.layout()->position().x()-2;
                yBlock = block.layout()->position().y();
            }

            if(blockData->getStatus()==BlockData::end){
                int height = block.layout()->position().y() - yBlock + TEXT_SIZE;
                painter.setPen(penCodeBlock);
                painter.drawRoundedRect(xBlock,yBlock,widthCodeBlock,height,BLOCKRADIUS,BLOCKRADIUS);
                painter.drawLine(xBlock,yBlock+TEXT_SIZE+7,widthCodeBlock,yBlock+TEXT_SIZE+7);
                codeId++;
            }
        }else{
            LineData* lineData = dynamic_cast<LineData*>(data);
            if(lineData){
                if(lineData->getStatus() == LineData::horizontalLine && lineData->getDraw()){
                    int lineX1 = block.layout()->position().x()-2;
                    int lineY1 = block.layout()->position().y()+(TEXT_SIZE*4/3);
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
    widthCodeBlock = this->width()-15-PADDING;
    heightCodeBlock = this->height();
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
    }else{
        QTextEdit::wheelEvent(e);
    }
}

void MkEdit::keyPressEvent(QKeyEvent *event)
{
    emit removeAllMkData();

    QTextEdit::keyPressEvent(event);

    switch(event->key()){
    case Qt::Key_Enter:
    case Qt::Key_Return: emit enterKeyPressed(this->textCursor().blockNumber()); break;
    case Qt::Key_QuoteLeft: quoteLeftKey(); break;
    }

    emit fileSave(); //save file
    emit applyAllMkData( this->textCursor().hasSelection(), this->textCursor().blockNumber());
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

void MkEdit::cursorPositionChangedHandle()
{
    int currentBlockNumber = textCursor().blockNumber();

    if(savedBlockNumber != currentBlockNumber){
        savedBlockNumber = currentBlockNumber;
        emit cursorPosChanged( textCursor().hasSelection(), currentBlockNumber);
        this->update();
    }
}
