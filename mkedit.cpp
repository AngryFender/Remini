#include <QElapsedTimer>
#include "blockdata.h"
#include "linedata.h"
#include "mkedit.h"

MkEdit::MkEdit(QWidget *parent):QTextEdit(parent){

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    undoAction.setText("Undo         Ctrl+Z");
    redoAction.setText("Redo          Ctrl+Y");
    copyTextAction.setText("Copy          Ctrl+C");
    pasteTextAction.setText("Paste          Ctrl+Y");
    deleteTextAction.setText("Delete        Del");
    selectAllAction.setText("Select All    Ctrl+A");
    selectBlockAction.setText("Select Block");

    connect(&undoAction, SIGNAL(triggered()), this, SLOT(undoContextMenu()));
    connect(&redoAction, SIGNAL(triggered()), this, SLOT(redoContextMenu()));
    connect(&copyTextAction, SIGNAL(triggered()), this, SLOT(copy()));
    connect(&pasteTextAction, SIGNAL(triggered()), this, SLOT(paste()));
    connect(&deleteTextAction, SIGNAL(triggered()), this, SLOT(deleteContextMenu()));
    connect(&selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));
    connect(&selectBlockAction, SIGNAL(triggered()), this, SLOT(selectBlock()));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(contextMenuHandler(QPoint)));

    penCodeBlock.setWidthF(1);
    penCodeBlock.setStyle(Qt::SolidLine);

    QObject::connect(this,SIGNAL(cursorPositionChanged()),
                     this, SLOT(cursorPositionChangedHandle()));

    QObject::connect(this->verticalScrollBar(),SIGNAL(valueChanged(int)),
                     this, SLOT(scrollValueUpdateHandle(int)));


    this->setUndoRedoEnabled(false);
}

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

    QAbstractTextDocumentLayout* layout = this->document()->documentLayout();
    QRect rect = getVisibleRect();

    QTextBlock block = this->document()->begin();
    while (block.isValid()) {
        if( layout->blockBoundingRect(block).bottom() < (rect.bottom()+40) && layout->blockBoundingRect(block).top() > rect.top()){
            QTextBlockUserData* data =block.userData();
            BlockData* blockData = dynamic_cast<BlockData*>(data);
            if(blockData){
                if(blockData->getStatus()==BlockData::start){
                    xBlock = block.layout()->position().x()-2;
                    yBlock = block.layout()->position().y()-scrollPos - (fontSize*0.4);
                }
                else if(blockData->getStatus()==BlockData::end){
                    int height = block.layout()->position().y() - yBlock + (fontSize*0.6)-scrollPos;

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

        QTextCursor cursor = this->cursorForPosition(QCursor::pos());
        this->setTextCursor(cursor);
        this->ensureCursorVisible();

    }else{
        QTextEdit::wheelEvent(e);
    }
}

void MkEdit::keyPressEvent(QKeyEvent *event)
{
    QElapsedTimer timer;
    timer.start();

    switch(event->key()){
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Up:
    case Qt::Key_Right:
    case Qt::Key_Left:
    case Qt::Key_Down:
    case Qt::Key_Alt:       QTextEdit::keyPressEvent(event);return;
    case Qt::Key_V:
    case Qt::Key_C:         if( event->modifiers() == Qt::CTRL) {QTextEdit::keyPressEvent(event);return;}break;
    case Qt::Key_S:         if( event->modifiers() == Qt::CTRL) {smartSelectionSetup(); return;}break;
    case Qt::Key_Tab:       if( event->modifiers() == Qt::NoModifier){
                                emit removeAllMkData();
                                postUndoSetup();
                                tabKeyPressed();
                                emit fileSave();
                                postUndoSetup();
                                emit applyAllMkData( this->textCursor().hasSelection(), this->textCursor().blockNumber(), undoData.selectAll, getVisibleRect());
                                return;
                            }break;
    }

    undoData.scrollValue = this->verticalScrollBar()->sliderPosition(); //this is important
    emit removeAllMkData();

    preUndoSetup();
    QTextEdit::keyPressEvent(event);

    switch(event->key()){
    case Qt::Key_Enter:
    case Qt::Key_Return:    emit enterKeyPressed(this->textCursor().blockNumber()); break;
    case Qt::Key_QuoteLeft: quoteLeftKey(); break;
    case Qt::Key_D:         if( event->modifiers() == Qt::CTRL) emit duplicateLine(this->textCursor().blockNumber());; break;
    case Qt::Key_Z:         if( event->modifiers() == Qt::CTRL) undo(); undoData.undoRedoSkip = true; break;
    case Qt::Key_Y:         if( event->modifiers() == Qt::CTRL) redo(); undoData.undoRedoSkip = true; break;
    case Qt::Key_A:         if( event->modifiers() == Qt::CTRL) undoData.selectAll = true; undoData.undoRedoSkip = true; break;
    default: break;
    }

    postUndoSetup();

    emit fileSave();

    emit applyAllMkData( this->textCursor().hasSelection(), this->textCursor().blockNumber(), undoData.selectAll, getVisibleRect());
    this->verticalScrollBar()->setSliderPosition(undoData.scrollValue);
    this->ensureCursorVisible();
    qDebug()<< ">>>time to process key press = "<<timer.elapsed();

}

void MkEdit::quoteLeftKey()
{
    bool success =false;
    emit quoteLeftKeyPressed(this->textCursor().blockNumber(),success);
    if(success){
        QTextCursor textCursor = this->textCursor();
        textCursor.movePosition(QTextCursor::PreviousBlock);
        textCursor.movePosition(QTextCursor::EndOfBlock);
        this->setTextCursor(textCursor);
    }
}

void MkEdit::smartSelectionSetup()
{
    QTextCursor cursor = this->textCursor();
    emit smartSelection(this->textCursor().blockNumber(), cursor);
    this->setTextCursor(cursor);
}

void MkEdit::tabKeyPressed()
{
    QTextCursor cursor = this->textCursor();
    cursor.insertText("    ");
    this->setTextCursor(cursor);
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

QRect MkEdit::getVisibleRect()
{
    QRect visibleRect = this->viewport()->rect();
    visibleRect.translate(this->horizontalScrollBar()->value(), this->verticalScrollBar()->value()); // translate the rectangle by the scroll bar offsets
    return visibleRect;
}

void MkEdit::contextMenuHandler(QPoint pos)
{
    contextMenuPos = pos;
    QTextCursor cursor = this->cursorForPosition(contextMenuPos);
    bool rightClockOnCodeBlock = false;
    emit checkRightClockOnCodeBlock(cursor.blockNumber(),rightClockOnCodeBlock);

    QMenu menu(this);
    menu.addAction(&undoAction);
    menu.addAction(&redoAction);
    menu.addSeparator();
    menu.addAction(&copyTextAction);
    menu.addAction(&pasteTextAction);
    menu.addAction(&deleteTextAction);
    menu.addSeparator();
    menu.addAction(&selectAllAction);
    if(rightClockOnCodeBlock)menu.addAction(&selectBlockAction);
    menu.exec(viewport()->mapToGlobal(pos));
}

void MkEdit::undoContextMenu()
{
    QKeyEvent keyPress(QKeyEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier);
    keyPressEvent(&keyPress);
}

void MkEdit::redoContextMenu()
{
    QKeyEvent keyPress(QKeyEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier);
    keyPressEvent(&keyPress);
}

void MkEdit::deleteContextMenu()
{
    QKeyEvent keyPress(QKeyEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
    keyPressEvent(&keyPress);
}

void MkEdit::selectBlock()
{
    QTextCursor cursor = this->cursorForPosition(contextMenuPos);

    int startPos = 0;
    int endPos = 0;
    emit selectBlockCopy(cursor.blockNumber(), startPos, endPos);

    if(startPos == endPos){
        return;
    }

    cursor.setPosition(startPos);
    cursor.setPosition(endPos,QTextCursor::KeepAnchor);

    this->setTextCursor(cursor);

    copy();
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

void MkEdit::insertFromMimeData(const QMimeData *source)
{
    emit removeAllMkData();
    preUndoSetup();

    QTextEdit::insertFromMimeData(source);

    postUndoSetup();
    emit fileSave();
    emit applyAllMkData( this->textCursor().hasSelection(), this->textCursor().blockNumber(), undoData.selectAll, getVisibleRect());
}

QColor MkEdit::getTypeColor() const
{
    return syntaxColor.type;
}

QColor MkEdit::getMethodColor() const
{
    return syntaxColor.method;
}

QColor MkEdit::getArgumentColor() const
{
    return syntaxColor.argument;
}

QColor MkEdit::getCommentColor() const
{
    return syntaxColor.comment;
}

QColor MkEdit::getQuoteColor() const
{
    return syntaxColor.quote;
}

QColor MkEdit::getKeywordColor() const
{
    return syntaxColor.keyword;
}

void MkEdit::setTypeColor(const QColor &color)
{
    syntaxColor.type = color;
}

void MkEdit::setMethodColor(const QColor &color)
{
    syntaxColor.method = color;
}

void MkEdit::setArgumentColor(const QColor &color)
{
    syntaxColor.argument = color;
}

void MkEdit::setCommentColor(const QColor &color)
{
    syntaxColor.comment = color;
}

void MkEdit::setQuoteColor(const QColor &color)
{
    syntaxColor.quote = color;
}

void MkEdit::setKeywordColor(const QColor &color)
{
    syntaxColor.keyword = color;
    emit syntaxColorUpdate(syntaxColor);
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
    emit cursorPosChanged(false, 0, getVisibleRect());
}

void MkEdit::scrollValueUpdateHandle(int value)
{
    int currentBlockNumber = textCursor().blockNumber();
    emit drawTextBlocks(textCursor().hasSelection(), currentBlockNumber,undoData.selectAll, getVisibleRect());
}

void MkEdit::cursorPositionChangedHandle()
{
    int currentBlockNumber = textCursor().blockNumber();

    if(savedBlockNumber != currentBlockNumber){
        savedBlockNumber = currentBlockNumber;
        emit cursorPosChanged( textCursor().hasSelection(), currentBlockNumber, getVisibleRect());
        this->update();
    }
}
