#include <QElapsedTimer>
#include "blockdata.h"
#include "linedata.h"
#include "mkedit.h"

MkEdit::MkEdit(QWidget *parent):QTextEdit(parent){

    fileSaveTimer.setInterval(FILE_SAVE_TIMEOUT);
    regexUrl.setPattern("(https?|ftp|file)://[\\w\\d._-]+(?:\\.[\\w\\d._-]+)+[\\w\\d._-]*(?:(?:/[\\w\\d._-]+)*/?)?(?:\\?[\\w\\d_=-]+(?:&[\\w\\d_=-]+)*)?(?:#[\\w\\d_-]+)?");
    regexFolderFile.setPattern("[a-zA-Z]:[\\\\/](?:[^\\\\/]+[\\\\/])*([^\\\\/]+\\.*)");
    savedCharacterNumber = -1;

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    undoAction.setText("Undo         Ctrl+Z");
    redoAction.setText("Redo          Ctrl+Y");
    copyTextAction.setText("Copy          Ctrl+C");
    pasteTextAction.setText("Paste          Ctrl+Y");
    deleteTextAction.setText("Delete        Del");
    selectAllAction.setText("Select All    Ctrl+A");
    selectBlockAction.setText("Copy Block");
    disableMarkdown.setText("Disable Markdown");

    connect(&undoAction, &QAction::triggered, this, &MkEdit::undoContextMenu);
    connect(&redoAction, &QAction::triggered, this, &MkEdit::redoContextMenu);
    connect(&copyTextAction, &QAction::triggered, this, &MkEdit::copy);
    connect(&pasteTextAction, &QAction::triggered, this, &MkEdit::paste);
    connect(&deleteTextAction, &QAction::triggered, this, &MkEdit::deleteContextMenu);
    connect(&selectAllAction, &QAction::triggered, this, &MkEdit::selectAll);
    connect(&selectBlockAction, &QAction::triggered, this, &MkEdit::selectBlock);
    connect(&disableMarkdown, &QAction::triggered,this, &MkEdit::diableMarkdown_internal);
    connect(this, &MkEdit::customContextMenuRequested,
            this, &MkEdit::contextMenuHandler);

    connect(&fileSaveTimer, &QTimer::timeout,
            this, &MkEdit::fileSaveHandle);

    penCodeBlock.setWidthF(1);
    penCodeBlock.setStyle(Qt::SolidLine);

    connectSignals();
    this->setUndoRedoEnabled(false);
    preUndoSetup();
}

void MkEdit::initialialCursorPosition()
{
    disconnectSignals();
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    this->setTextCursor(cursor);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->update();
    this->verticalScrollBar()->setSliderPosition(0);

    int savedBlockNumber = cursor.blockNumber();
    selectRange.start = this->textCursor().selectionStart();
    selectRange.end =  this->textCursor().selectionEnd();

    emit cursorPosChanged( textCursor().hasSelection(), savedBlockNumber , getVisibleRect(), &selectRange);
    connectSignals();
}

void MkEdit::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    int xBlock =0, yBlock =0;
    int fontSize = this->document()->defaultFont().pointSizeF();
    int scrollPos = this->verticalScrollBar()->value();
    penCodeBlock.setColor(codeBlockColor);

    QAbstractTextDocumentLayout* layout = this->document()->documentLayout();
    QRect rect = getVisibleRect();

    QTextBlock block = this->document()->begin();
    while (block.isValid()) {
        if( layout->blockBoundingRect(block).bottom() < (rect.bottom()+60) && layout->blockBoundingRect(block).top() > (rect.top()-15)){
            QTextBlockUserData* data =block.userData();
            BlockData* blockData = dynamic_cast<BlockData*>(data);
            QTextBlock nextBlock = block.next();
            if(blockData){
                if(blockData->getStatus()==BlockData::start){
                    xBlock = block.layout()->position().x()-2;
                    yBlock = block.layout()->position().y()-scrollPos - (fontSize*0.4);
                }
                else{
                    if(blockData->getStatus()==BlockData::end
                        || (blockData->getStatus()==BlockData::content && layout->blockBoundingRect(nextBlock).bottom()>= (rect.bottom()+40))){
                        int height = block.layout()->position().y() - yBlock + (fontSize*0.6)-scrollPos;

                        QBrush brushDefault(codeBlockColor);
                        painter.setBrush(brushDefault);
                        painter.setPen(penCodeBlock);
                        painter.drawRoundedRect(xBlock,yBlock,widthCodeBlock,height,BLOCKRADIUS,BLOCKRADIUS);
                    }
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
        disconnectSignals();
        emit removeAllMkData(this->textCursor().blockNumber());
        if (zoomDelta > 0) {
            if((this->currentFont().pointSizeF())<MAXIMUM_FONT_SIZE)
                this->zoomIn();
        } else {
            if((this->currentFont().pointSizeF())>MINIMUM_FONT_SIZE)
                this->zoomOut();
        }
        emit applyAllMkData( this->textCursor().hasSelection(), this->textCursor().blockNumber(), undoData.selectAll, getVisibleRect());
        connectSignals();
        this->ensureCursorVisible();
    }else{
        QTextEdit::wheelEvent(e);
    }
}

void MkEdit::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Up:
    case Qt::Key_Right:
    case Qt::Key_Left:
    case Qt::Key_Down:
    case Qt::Key_Alt:       QTextEdit::keyPressEvent(event);return;
    case Qt::Key_C:         if( event->modifiers() == Qt::CTRL) {QTextEdit::keyPressEvent(event);return;}break;
    case Qt::Key_S:         if( event->modifiers() == Qt::CTRL) {smartSelectionSetup(); return;}break;
    case Qt::Key_Tab:       if( event->modifiers() == Qt::NoModifier){
                                clearMkEffects();
                                tabKeyPressed();
                                fileSaveNow(); return;
                            }break;
    }

    clearMkEffects();
    QTextEdit::keyPressEvent(event);

    switch(event->key()){
    case Qt::Key_Enter:
    case Qt::Key_Return:    emit enterKeyPressed(this->textCursor().blockNumber());
    case Qt::Key_Space:     fileSaveNow(); return;
    case Qt::Key_QuoteLeft: quoteLeftKey(); fileSaveNow(); return;
    case Qt::Key_D:         if( event->modifiers() == Qt::CTRL) emit duplicateLine(this->textCursor().blockNumber());; fileSaveNow(); return;
    case Qt::Key_Z:         if( event->modifiers() == Qt::CTRL) emit undoStackUndoSignal(); undoData.undoRedoSkip = true; fileSaveNow(); scrollValueUpdateHandle(undoData.scrollValue);return;
    case Qt::Key_Y:         if( event->modifiers() == Qt::CTRL) emit undoStackRedoSignal(); undoData.undoRedoSkip = true; fileSaveNow(); ;return;

    default: break;
    }

    applyMkEffects();
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
    emit autoInsertSymbol(this->textCursor().position());
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
        emit undoStackPushSignal(edit) ;
    }
}

QRect MkEdit::getVisibleRect()
{
    QRect visibleRect = this->viewport()->rect();
    visibleRect.translate(this->horizontalScrollBar()->value(), this->verticalScrollBar()->value()); // translate the rectangle by the scroll bar offsets
    return visibleRect;
}

void MkEdit::clearMkEffects()
{
    disconnectSignals();
    undoData.scrollValue = this->verticalScrollBar()->sliderPosition(); //this is importantp
    emit removeAllMkData(this->textCursor().blockNumber());
    if(!fileSaveTimer.isActive()){
        preUndoSetup();
    }
    fileSaveTimer.start();
}

void MkEdit::removeAllMkDataFunc(int blockNumber)
{
    disconnectSignals();
    emit removeAllMkData(blockNumber);
    connectSignals();
}

void MkEdit::applyMkEffects(const bool scroll)
{
    disconnectSignals();
    emit applyAllMkData( this->textCursor().hasSelection(), this->textCursor().blockNumber(), undoData.selectAll, getVisibleRect());
    if(scroll){
        this->verticalScrollBar()->setSliderPosition(undoData.scrollValue);
        this->ensureCursorVisible();
    }
    connectSignals();
}

void MkEdit::fileSaveNow()
{
    fileSaveTimer.stop();
    postUndoSetup();
    emit fileSave();
    applyMkEffects();
}

void MkEdit::fileSaveWithScroll(const bool scroll)
{
    fileSaveTimer.stop();
    removeAllMkDataFunc(this->textCursor().blockNumber());
    postUndoSetup();
    emit fileSave();
    applyMkEffects(scroll);
}

bool MkEdit::isMouseOnCheckBox(QMouseEvent *e)
{
    int yPos = e->pos().y();
    int xPos = e->pos().x();
    QPoint pointer(xPos,yPos);

    QTextDocument *doc = this->document();
    MkTextDocument *mkDoc = dynamic_cast<MkTextDocument*>(doc);

    if(nullptr == mkDoc){
        return false;
    }

    int width = this->document()->defaultFont().pointSize()*1.30;
    int last = this->document()->lastBlock().position()+this->document()->lastBlock().length();

    QTextCursor cursor(this->textCursor());
    QRect rect;
    for(auto it = mkDoc->checkMarkPosBegin(); it!= mkDoc->checkMarkPosEnd(); it++){
        if((*it>last)){
            continue;
        }
        cursor.setPosition(*it);
        rect = this->cursorRect(cursor);
        rect.setWidth(width);
        if(rect.contains(pointer)){
            int pos = (*it);
            removeAllMkDataFunc(this->textCursor().blockNumber());
            preUndoSetup();
            applyMkEffects(false);
            emit pushCheckBox(pos);
            fileSaveWithScroll(false);
            return true;
        }
    }

    int linkTextWidth = 0;
    for(auto it = mkDoc->linkPosBegin(); it!= mkDoc->linkPosEnd(); ++it){
        if((*it).first >last ||(*it).second >last ){
            continue;
        }
        cursor.setPosition((*it).first);

        rect = this->cursorRect(cursor);
        linkTextWidth = ((*it).second - (*it).first) *this->document()->defaultFont().pointSize()*0.55;
        rect.setWidth(linkTextWidth);
        if(rect.contains(pointer)){
            int pos = (*it).first;
            removeAllMkDataFunc(this->textCursor().blockNumber());
            applyMkEffects(false);
            preUndoSetup();
            connectSignals();
            emit pushLink(pos);
            fileSaveWithScroll(false);
            return true;
        }
    }

    return false;
}

void MkEdit::connectSignals()
{
    QObject::connect(this,&MkEdit::cursorPositionChanged,
                     this, &MkEdit::cursorPositionChangedHandle);
    QObject::connect(this->verticalScrollBar(),&QScrollBar::valueChanged,
                     this, &MkEdit::scrollValueUpdateHandle);
}

void MkEdit::disconnectSignals()
{
    QObject::disconnect(this->verticalScrollBar(),&QScrollBar::valueChanged,
                     this, &MkEdit::scrollValueUpdateHandle);
    QObject::disconnect(this,&MkEdit::cursorPositionChanged,
                     this, &MkEdit::cursorPositionChangedHandle);
}

void MkEdit::contextMenuHandler(QPoint pos)
{
    contextMenuPos = pos;
    QTextCursor cursor = this->cursorForPosition(contextMenuPos);
    bool rightClockOnCodeBlock = false;
    emit checkRightClockOnCodeBlock(cursor.blockNumber(),rightClockOnCodeBlock);

    QMenu menu(this);
    if(rightClockOnCodeBlock)menu.addAction(&selectBlockAction);
    menu.addAction(&undoAction);
    menu.addAction(&redoAction);
    menu.addSeparator();
    menu.addAction(&copyTextAction);
    menu.addAction(&pasteTextAction);
    menu.addAction(&deleteTextAction);
    menu.addSeparator();
    menu.addAction(&selectAllAction);
    menu.addSeparator();
    menu.addAction(&disableMarkdown);
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
    bool isBlock = false;
    QTextCursor cursor = this->textCursor();
    disconnectSignals();
    emit checkIfCursorInBlock(isBlock,cursor);

    if(!isBlock){
        emit removeAllMkData(this->textCursor().blockNumber());
        preUndoSetup();

          QString link = source->text();
        matchUrl = regexUrl.match(link);
        matchFolderFile = regexFolderFile.match(link);
        if (matchUrl.hasMatch() && !link.contains("](")) {
            int pos = cursor.position()+1;
            QString symbolsWithLink = "[]("+link+")";
            cursor.insertText(symbolsWithLink);
            cursor.setPosition(pos);
            this->setTextCursor(cursor);
        }else if(matchFolderFile.hasMatch() && !link.contains("](")) {
            int pos = cursor.position()+1;
            QString symbolsWithLink = "[](file:///"+link+")";
            cursor.insertText(symbolsWithLink);
            cursor.setPosition(pos);
            this->setTextCursor(cursor);
        }
        else{
            QTextEdit::insertFromMimeData(source);
        }
    }else{
        QTextEdit::insertFromMimeData(source);
    }


    postUndoSetup();
    emit fileSave();
    emit applyAllMkData( this->textCursor().hasSelection(), this->textCursor().blockNumber(), undoData.selectAll, getVisibleRect());
    connectSignals();
}

void MkEdit::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton && !this->textCursor().hasSelection()){
        const QPoint pos (e->position().x(),e->position().y());
        this->setTextCursor(this->cursorForPosition(pos));
    }

    if(!isMouseOnCheckBox(e)){
//        selectRange.blockStart = this->textCursor().block().blockNumber();
//        selectRange.posInBlockStart = this->textCursor().positionInBlock();
        QTextEdit::mousePressEvent(e);
    }
}

void MkEdit::mouseMoveEvent(QMouseEvent *e)
{
    QTextEdit::mouseMoveEvent(e);

    int yPos = e->pos().y();
    int xPos = e->pos().x();
    QPoint pointer(xPos,yPos);

    QTextDocument *doc = this->document();
    MkTextDocument *mkDoc = dynamic_cast<MkTextDocument*>(doc);

    if(nullptr == mkDoc){
        return;
    }

    int width = this->document()->defaultFont().pointSize()*1.30;
    int last = this->document()->lastBlock().position()+this->document()->lastBlock().length();

    QTextCursor cursor(this->textCursor());
    QRect rect;
    for(auto it = mkDoc->checkMarkPosBegin(); it!= mkDoc->checkMarkPosEnd(); it++){
        if((*it>last)){
            continue;
        }
        cursor.setPosition(*it);
        rect = this->cursorRect(cursor);
        rect.setWidth(width);
        if(rect.contains(pointer)){
            this->viewport()->setCursor(Qt::CursorShape::PointingHandCursor);
            return;
        }
    }

    int linkTextWidth = 0;
    for(auto it = mkDoc->linkPosBegin(); it!= mkDoc->linkPosEnd(); ++it){
        if((*it).first >last ||(*it).second >last ){
            continue;
        }
        cursor.setPosition((*it).first);

        rect = this->cursorRect(cursor);
        linkTextWidth = ((*it).second - (*it).first) *this->document()->defaultFont().pointSize()*0.55;
        rect.setWidth(linkTextWidth);
        if(rect.contains(pointer)){
            this->viewport()->setCursor(Qt::CursorShape::PointingHandCursor);
            return;
        }
    }

    this->viewport()->setCursor(Qt::CursorShape::ArrowCursor);
}

void MkEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(!isMouseOnCheckBox(e)){
        QTextEdit::mouseDoubleClickEvent(e);
    }
}

void MkEdit::dragEnterEvent(QDragEnterEvent *e)
{

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

void MkEdit::setDocument(QTextDocument *document)
{
    disconnectSignals();
    QTextEdit::setDocument(document);
    connectSignals();
}

void MkEdit::scrollValueUpdateHandle(int value)
{
    int currentBlockNumber = textCursor().blockNumber();
    disconnectSignals();
    emit drawTextBlocks(textCursor().hasSelection(), currentBlockNumber,undoData.selectAll, getVisibleRect(), &selectRange);
    connectSignals();
}

void MkEdit::fileSaveHandle()
{
    fileSaveWithScroll();
}

void MkEdit::diableMarkdown_internal()
{
    QTextCursor cursor = this->textCursor();
    QTextBlock block = this->document()->findBlockByNumber(cursor.blockNumber());

    if("Disable Markdown" ==disableMarkdown.text()){
        disableMarkdown.setText("Enable Markdown");
        emit setMarkdownStatus(false, getVisibleRect());
        cursor.setPosition(block.position());
        this->setTextCursor(cursor);
    }else{
        disableMarkdown.setText("Disable Markdown");
        emit setMarkdownStatus(true, getVisibleRect());
    }
}

void MkEdit::cursorPositionChangedHandle()
{
    int currentBlockNumber = textCursor().blockNumber();
    int currentCharacterNumber = textCursor().position();

    if(savedCharacterNumber != currentCharacterNumber){
        savedCharacterNumber = currentCharacterNumber;

        QTextCursor cursor = this->textCursor();

        if(!cursor.hasSelection()){
            selectRange.hasSelection = false;
            selectRange.start = -1;
            selectRange.end = -1;
            selectRange.currentposInBlock = cursor.positionInBlock();
            selectRange.currentBlockPos = cursor.block().position();
            selectRange.isCursorCaculated = false;
            selectRange.selectionFirstStartPos = NO_SELECTION_POS;
        }else{
            selectRange.hasSelection = true;
            selectRange.start = cursor.selectionStart();
            selectRange.end = cursor.selectionEnd();
            if(NO_SELECTION_POS == selectRange.selectionFirstStartPos){
                selectRange.selectionFirstStartPos = cursor.selectionStart();
                selectRange.selectionFirstStartBlock = cursor.blockNumber();
                selectRange.selectionFirstStartPosInBlock = cursor.positionInBlock();
            }
        }

        disconnectSignals();
        emit cursorPosChanged( textCursor().hasSelection(), currentBlockNumber, getVisibleRect(), &selectRange);

        //insert cursor inbetween the formatted words since after symbols are inserted the positions are shifted
        if(!cursor.hasSelection() && selectRange.isCursorCaculated){
            cursor.setPosition(this->document()->findBlockByNumber(selectRange.currentBlockNo).position()+selectRange.currentposInBlock);
            this->setTextCursor(cursor);
        }

        //make sure selection works regardless of the formatting used
        if(selectRange.hasSelection){
            if(currentCharacterNumber > selectRange.selectionFirstStartPos){
//                cursor.setPosition( this->document()->findBlockByNumber(selectRange.selectionFirstStartBlock).position() + selectRange.selectionFirstStartPosInBlock);
                cursor.setPosition(selectRange.selectionFirstStartPos);
                cursor.setPosition(currentCharacterNumber,QTextCursor::KeepAnchor);
            }else{
                cursor.clearSelection();
                cursor.setPosition( this->document()->findBlockByNumber(selectRange.selectionFirstStartBlock).position() + selectRange.selectionFirstStartPosInBlock + 1);
                cursor.setPosition(currentCharacterNumber,QTextCursor::KeepAnchor);
            }
            this->setTextCursor(cursor);
        }
        connectSignals();
    }
}
