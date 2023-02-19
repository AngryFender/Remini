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

                QBrush brushDefault(QColor(194,201,207));
                painter.setBrush(brushDefault);
                painter.setPen(penCodeBlock);
                painter.drawRoundedRect(xBlock,yBlock,widthCodeBlock,height,BLOCKRADIUS,BLOCKRADIUS);
            }else{
                QTextCursor tcursor(block);
                QTextBlockFormat blockFormat = tcursor.blockFormat();
                blockFormat.setLeftMargin(fontSize);
                tcursor.setBlockFormat(blockFormat);
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
    widthCodeBlock = this->width()-15-PADDING - this->verticalScrollBar()->width();
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
    switch(event->modifiers()){
        case Qt::CTRL:
        case Qt::ALT:QTextEdit::keyPressEvent(event);return;
    }

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
