#include <QElapsedTimer>
#include <QSettings>
#include "blockdata.h"
#include "linedata.h"
#include "mkedit.h"

MkEdit::MkEdit(QWidget *parent):QTextEdit(parent){

    fileSaveTimer.setInterval(FILE_SAVE_TIMEOUT);
    regexUrl.setPattern("(https?|ftp|file)://[\\w\\d._-]+(?:\\.[\\w\\d._-]+)+[\\w\\d._-]*(?:(?:/[\\w\\d._-]+)*/?)?(?:\\?[\\w\\d_=-]+(?:&[\\w\\d_=-]+)*)?(?:#[\\w\\d_-]+)?");
    regexCodeBlock.setPattern("```(?s)(.*?)```");
    regexFolderFile.setPattern("[a-zA-Z]:[\\\\/](?:[^\\\\/]+[\\\\/])*([^\\\\/]+\\.*)");
    savedCharacterNumber = -1;
    isShiftKeyPressed = false;
    isDisconnectedViaHighPriority = false;
    undoData.viewEditTypeStore = &undoRedoEditType;
    undoData.viewSelectRangeStore = &undoRedoSelectRange;

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    undoAction.setText("Undo         Ctrl+Z");
    redoAction.setText("Redo          Ctrl+Y");
    copyTextAction.setText("Copy          Ctrl+C");
    pasteTextAction.setText("Paste          Ctrl+Y");
    deleteTextAction.setText("Delete        Del");
    selectAllAction.setText("Select All    Ctrl+A");
    selectBlockAction.setText("Copy Block");
    disableMarkdown.setText("Disable Markdown");
    lineWrapAction.setText("Disable line-wrap");

    connect(&undoAction, &QAction::triggered, this, &MkEdit::undoContextMenu);
    connect(&redoAction, &QAction::triggered, this, &MkEdit::redoContextMenu);
    connect(&copyTextAction, &QAction::triggered, this, &MkEdit::copy);
    connect(&pasteTextAction, &QAction::triggered, this, &MkEdit::paste);
    connect(&deleteTextAction, &QAction::triggered, this, &MkEdit::deleteContextMenu);
    connect(&selectAllAction, &QAction::triggered, this, &MkEdit::selectAll);
    connect(&selectBlockAction, &QAction::triggered, this, &MkEdit::selectBlock);
    connect(&disableMarkdown, &QAction::triggered,this, &MkEdit::diableMarkdown_internal);
    connect(&lineWrapAction, &QAction::triggered,this, &MkEdit::lineWrapHandler);
    connect(this, &MkEdit::customContextMenuRequested,
            this, &MkEdit::contextMenuHandler);

    connect(&fileSaveTimer, &QTimer::timeout,
            this, &MkEdit::fileSaveHandle);

    penCodeBlock.setWidthF(1);
    penCodeBlock.setStyle(Qt::SolidLine);

    connectSignals(true);
    this->setUndoRedoEnabled(false);
    preUndoSetup();
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
    painter.setBrush(brushDefault);
    painter.setPen(penCodeBlock);

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
                    xBlock = block.layout()->position().x();
                    yBlock = block.layout()->position().y()-scrollPos - (fontSize*0.4) - block.blockFormat().topMargin()*0.8;
                }
                else{
                    if(blockData->getStatus()==BlockData::end
                        || (blockData->getStatus()==BlockData::content && layout->blockBoundingRect(nextBlock).bottom()>= (rect.bottom()+40))){
                        int height = block.layout()->position().y() - yBlock + (fontSize*1.6)-scrollPos;
                        painter.drawRoundedRect(xBlock,yBlock,widthCodeBlock,height,BLOCKRADIUS,BLOCKRADIUS);
                    }
                }
            }else{
                LineData* lineData = dynamic_cast<LineData*>(data);
                if(lineData){
                    int lineX1 = this->x();
                    int lineY1 = block.layout()->position().y()+fontSize-scrollPos;
                    int lineX2 = this->x()+widthCodeBlock;

                    painter.save();
                    painter.setPen(whitePen);
                    painter.drawLine(lineX1,lineY1,lineX2,lineY1);
                    painter.restore();
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
    widthCodeBlock = event->size().width()- 2 - this->verticalScrollBar()->width()/2;
}

void MkEdit::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier) {
        int zoomDelta = e->angleDelta().y();
        if (zoomDelta > 0) {
            if((this->currentFont().pointSizeF())<MAXIMUM_FONT_SIZE)
                this->zoomIn();
        } else {
            if((this->currentFont().pointSizeF())>MINIMUM_FONT_SIZE)
                this->zoomOut();
        }
        this->ensureCursorVisible();

        QSettings settings("Remini","Remini");
        settings.setValue("fontsize",this->currentFont().pointSizeF());
    }else{
        QTextEdit::wheelEvent(e);
    }
}

void MkEdit::keyPressEvent(QKeyEvent *event)
{
    Connector connector(
        std::bind(&MkEdit::disconnectSignals,this,std::placeholders::_1),
        std::bind(&MkEdit::connectSignals,this,std::placeholders::_1)
    );

    undoData.editType = singleEdit;
    switch(event->key()){
    case Qt::Key_Shift: isShiftKeyPressed = true;
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
    case Qt::Key_Up:
    case Qt::Key_Right:
    case Qt::Key_Left:
    case Qt::Key_Down:      setPreArrowKeys(event->modifiers()==Qt::SHIFT,event->key() == Qt::Key_Up || event->key() == Qt::Key_Down);
                            QTextEdit::keyPressEvent(event);
                            setPostArrowKeys(event->modifiers() == Qt::SHIFT, event->key() == Qt::Key_Left,event->key() == Qt::Key_Up || event->key() == Qt::Key_Down);
                            return;
    case Qt::Key_Control:
    case Qt::Key_Alt:       QTextEdit::keyPressEvent(event);return;
    case Qt::Key_V:         if( event->modifiers() == Qt::CTRL) {pasteTextAction.trigger();return;}break;
    case Qt::Key_C:         if( event->modifiers() == Qt::CTRL) {QTextEdit::keyPressEvent(event);return;}break;
    case Qt::Key_S:         if( event->modifiers() == Qt::CTRL) {smartSelectionSetup(); return;}break;
    case Qt::Key_Tab:       if( event->modifiers() == Qt::NoModifier){
                                clearMkEffects(undoData.editType);
                                tabKeyPressed();
                                fileSaveNow(); return;
                            }break;
    case Qt::Key_Delete:    if(textCursor().positionInBlock()== (textCursor().block().length()-1)){ undoData.editType = multiEdit;} break;
    case Qt::Key_Enter:
    case Qt::Key_Return:    undoData.editType = multiEdit; break;
    case Qt::Key_D:         if( event->modifiers() == Qt::CTRL) {undoData.editType = multiEdit;}break;
    case Qt::Key_Z:         if( event->modifiers() == Qt::CTRL) {undoData.editType = undoRedo;undoData.undoRedoSkip = true;}break;
    case Qt::Key_Y:         if( event->modifiers() == Qt::CTRL) {undoData.editType = undoRedo;undoData.undoRedoSkip = true;}break;
    case Qt::Key_Backspace:{
                            QString blockText = this->textCursor().block().text();
                            if((textCursor().positionInBlock() == 0) || (blockText.left(3)=="```")){
                                undoData.editType = multiEdit;
                            } break;}
    case Qt::Key_QuoteLeft: undoData.editType = multiEdit; break;
    }

    if(textCursor().hasSelection() && undoData.editType != undoRedo){
        if(selectRange.selectionFirstStartBlock == selectRange.selectionEndBlock){
            undoData.editType = singleEdit;
        }else{
            undoData.editType = multiEdit;
        }
    }
    clearMkEffects(undoData.editType);
    QTextEdit::keyPressEvent(event);
    selectRange.currentBlockNo    = textCursor().blockNumber();
    selectRange.currentposInBlock = selectRange.arrowPosInBlock = textCursor().positionInBlock();

    switch(event->key()){
    case Qt::Key_Enter:
    case Qt::Key_Return:    emit enterKeyPressed(this->textCursor().blockNumber(), this->selectRange.currentposInBlock);
    case Qt::Key_Space:     fileSaveNow(); return;
    case Qt::Key_QuoteLeft: quoteLeftKey();
    case Qt::Key_Delete:
    case Qt::Key_Backspace: this->selectRange.currentBlockNo = textCursor().blockNumber();
                            this->selectRange.currentposInBlock = textCursor().positionInBlock();
                            if( undoData.editType != EditType::singleEdit){
                                fileSaveNow();
                                return;
                            }break;
    case Qt::Key_D:         if( event->modifiers() == Qt::CTRL) {emit duplicateLine(this->textCursor().blockNumber());; fileSaveNow(); return;}break;
    case Qt::Key_Z:         if( event->modifiers() == Qt::CTRL) {
                                bool success = false;
                                emit undoStackUndoSignal(success);
                                if(success){
                                    undoData.editType = (undoData.viewEditTypeStore? *undoData.viewEditTypeStore: multiEdit);
                                    undoData.undoRedoSkip = true;
                                    fileSaveTimer.stop();
                                    postUndoSetup();
                                    emit fileSaveRaw();
                                    applyMkEffects(undoRedoSelectRange.currentBlockNo);
                                    showSelectionAfterUndo();
                                }
                                return;
                            }break;
    case Qt::Key_Y:         if( event->modifiers() == Qt::CTRL) {
                                bool success = false;
                                emit undoStackRedoSignal(success);
                                if(success){
                                    undoData.editType = (undoData.viewEditTypeStore? *undoData.viewEditTypeStore: multiEdit);
                                    undoData.undoRedoSkip = true;
                                    fileSaveTimer.stop();
                                    postUndoSetup();
                                    emit fileSaveRaw();
                                    applyMkEffects(undoRedoSelectRange.currentBlockNo);
                                    showSelectionAfterRedo();
                                }
                                return;
                            }break;
    default: break;
    }

    updateRawDocument();
    applyMkEffects(textCursor().blockNumber());
}

void MkEdit::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Shift: isShiftKeyPressed = false;
        default: break;
    }
    QTextEdit::keyReleaseEvent(event);
}

void MkEdit::showSelectionAfterUndo(){
    selectRange = undoRedoSelectRange;

    //first show all the Markdown symbols in the editor
    emit cursorPosChanged(&selectRange, this->isReadOnly());
    postCursorPosChangedSignal();

    //ensure the textcursor is visible
    this->verticalScrollBar()->setSliderPosition(undoData.scrollValue);
    if(!isTextCursorVisible()){
        this->ensureCursorVisible();
    }

    if(selectRange.isCheckBox){
        this->verticalScrollBar()->setSliderPosition(selectRange.scrollValue);
    }
}

void MkEdit::showSelectionAfterRedo()
{
    SelectRange &range = undoRedoSelectRange;

    //first show all the Markdown symbols in the editor
    emit cursorPosChanged(&range, this->isReadOnly());

    QTextCursor cursor = this->textCursor();
    cursor.setPosition(this->document()->findBlockByNumber(range.currentBlockNo).position()+range.currentposInBlock);
    this->setTextCursor(cursor);

    if(range.isCheckBox){
        this->verticalScrollBar()->setSliderPosition(range.scrollValue);
    }
}

void MkEdit::setPreArrowKeys(const bool isShiftPressed, const bool isUpOrDownArrowPressed)
{
    QTextCursor cursor = this->textCursor();
    if(!cursor.hasSelection() && isShiftPressed){
        selectRange.selectionFirstStartBlock = cursor.blockNumber();
        selectRange.selectionFirstStartPosInBlock = cursor.positionInBlock();
    }

    selectRange.arrowBlock = cursor.blockNumber() ;
    selectRange.arrowPosInBlock = isUpOrDownArrowPressed && cursor.positionInBlock()<selectRange.arrowPosInBlock ? selectRange.arrowPosInBlock: cursor.positionInBlock() ;
}

void MkEdit::setPostArrowKeys(const bool isShiftPressed, const bool isLeftArrowPressed, const bool isUpOrDownArrowPressed)
{
    disconnectSignals(true);
    QTextCursor cursor = this->textCursor();
    if(!isShiftPressed){
        selectRange.selectionFirstStartBlock 		= selectRange.selectionEndBlock 		= selectRange.currentBlockNo    = cursor.blockNumber();
        selectRange.selectionFirstStartPosInBlock 	= selectRange.selectionEndPosInBlock 	= selectRange.currentposInBlock = cursor.positionInBlock();
        selectRange.hasSelection = false;
        emit cursorPosChanged(&selectRange, this->isReadOnly());
        cursor.setPosition(this->textCursor().block().position() + selectRange.selectionFirstStartPosInBlock);
        this->setTextCursor(cursor);

        if(selectRange.arrowBlock != textCursor().blockNumber()){
            int arrowPosInBlock;
            arrowPosInBlock = isUpOrDownArrowPressed? selectRange.arrowPosInBlock : selectRange.selectionFirstStartPosInBlock;
            arrowPosInBlock = (arrowPosInBlock >= textCursor().block().text().length())? textCursor().block().text().length(): arrowPosInBlock;
            arrowPosInBlock = (textCursor().block().text().length()==0)? 0 : arrowPosInBlock;
            cursor.setPosition(this->textCursor().block().position() + arrowPosInBlock);
            this->setTextCursor(cursor);
        }else{
            if(!isUpOrDownArrowPressed){
                int arrowPosInBlock = selectRange.arrowPosInBlock = selectRange.selectionFirstStartPosInBlock;
                cursor.setPosition(this->textCursor().block().position() + arrowPosInBlock);
                this->setTextCursor(cursor);
            }
        }
    }else{
        selectRange.selectionEndBlock 		= selectRange.currentBlockNo    = cursor.blockNumber();
        selectRange.selectionEndPosInBlock 	= selectRange.currentposInBlock = cursor.positionInBlock();
        selectRange.hasSelection = true;
        emit cursorPosChanged(&selectRange,this->isReadOnly());

        selectRange.arrowPosInBlock = isUpOrDownArrowPressed? selectRange.arrowPosInBlock : selectRange.selectionEndPosInBlock;
        selectRange.arrowPosInBlock = (selectRange.arrowPosInBlock >textCursor().block().text().length())? textCursor().block().text().length(): selectRange.arrowPosInBlock;

        cursor.setPosition(this->document()->findBlockByNumber(selectRange.selectionFirstStartBlock).position() + selectRange.selectionFirstStartPosInBlock);
        cursor.setPosition(this->document()->findBlockByNumber(selectRange.selectionEndBlock).position() + selectRange.arrowPosInBlock,QTextCursor::KeepAnchor);
        this->setTextCursor(cursor);
    }

    connectSignals(true);
}

void MkEdit::restoreTextCursor(int blockNo, int posInBlock, bool hasSelection)
{
    QTextCursor cursor = this->textCursor();
    if(hasSelection){
        cursor.setPosition(this->document()->findBlockByNumber(selectRange.selectionFirstStartBlock).position() + selectRange.selectionFirstStartPosInBlock);
        cursor.setPosition(this->document()->findBlockByNumber(blockNo).position() + posInBlock, QTextCursor::KeepAnchor);
    }else{
        cursor.setPosition(this->document()->findBlockByNumber(blockNo).position() + posInBlock);
    }
    this->setTextCursor(cursor);
}

void MkEdit::postCursorPosChangedSignal()
{
    QTextCursor cursor = this->textCursor();
    if(!isCalcuatedForStartPos && selectRange.selectionFirstStartBlock == selectRange.currentBlockNo && !isShiftKeyPressed){
        isCalcuatedForStartPos = true;
        selectRange.selectionFirstStartBlock = selectRange.currentBlockNo;
        selectRange.selectionFirstStartPosInBlock = selectRange.currentposInBlock;
    }

    //make sure selection works regardless of the formatting used
    if(selectRange.hasSelection){
        int startInDoc = this->document()->findBlockByNumber(selectRange.selectionFirstStartBlock).position() + selectRange.selectionFirstStartPosInBlock;
        int endInDoc   = this->document()->findBlockByNumber(selectRange.selectionEndBlock).position() + selectRange.selectionEndPosInBlock;

        QTextCursor newCursor = this->textCursor();
        newCursor.clearSelection();
        newCursor.setPosition(startInDoc);
        newCursor.setPosition(endInDoc,QTextCursor::KeepAnchor);
        this->setTextCursor(newCursor);
    }else{
        //insert cursor inbetween the formatted words since after symbols are inserted the positions are shifted
        cursor.setPosition(this->document()->findBlockByNumber(selectRange.currentBlockNo).position()+selectRange.currentposInBlock);
        this->setTextCursor(cursor);
    }
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
    int newPosition;
    emit autoInsertSymbol(this->textCursor().position(), newPosition);
    selectRange.currentposInBlock = newPosition;
}

void MkEdit::preUndoSetup()
{
    MkTextDocument * mkDoc = dynamic_cast<MkTextDocument*>(this->document());
    undoData.oldText            = (mkDoc?mkDoc->getRawDocument()->toPlainText(): document()->toPlainText());
    undoData.view               = this;
    undoData.doc                = this->document();
    undoData.undoRedoSkip       = false;
    undoData.selectAll          = false;
    undoData.oldSelectRange     = this->selectRange;
    undoData.oldSelectRange.hasSelection     	= this->textCursor().hasSelection();
    undoData.oldSelectRange.currentBlockNo 		= this->textCursor().blockNumber();
    undoData.oldSelectRange.currentposInBlock 	= this->textCursor().positionInBlock();
    undoData.oldBlock = this->document()->findBlockByNumber(this->textCursor().blockNumber()).text();
}

void MkEdit::postUndoSetup()
{
    undoData.blockNo 	= this->textCursor().blockNumber();
    undoData.posInBlock = this->textCursor().positionInBlock();

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

void MkEdit::clearMkEffects(EditType editType)
{
    undoData.scrollValue = this->verticalScrollBar()->sliderPosition(); //this is important
    if(editType == undoRedo){
        return;
    }

    QTextCursor cursor = this->textCursor();
    int blockNumber = cursor.blockNumber();
    int posInBlock = cursor.positionInBlock();
    bool hasSelection = cursor.hasSelection();

    if(editType != EditType::singleEdit){
        emit removeAllMkData(this->textCursor().blockNumber());
    }

    int oldStartPosition = document()->findBlockByNumber(selectRange.selectionFirstStartBlock).position() + selectRange.selectionFirstStartPosInBlock;
    oldStartPosition = (oldStartPosition < 0)? 0 : oldStartPosition;
    oldStartPosition = (oldStartPosition < document()->characterCount())? oldStartPosition : document()->characterCount()-1;

    int oldEndPosition = this->document()->findBlockByNumber(blockNumber).position() + posInBlock;
    oldEndPosition = (oldEndPosition < 0)? 0 : oldEndPosition;
    oldEndPosition = (oldEndPosition < document()->characterCount())? oldEndPosition : document()->characterCount()-1;

    if(hasSelection){
        cursor.setPosition(oldStartPosition);
        cursor.setPosition(oldEndPosition, QTextCursor::KeepAnchor);
    }else{
        cursor.setPosition(oldEndPosition);
    }

    this->setTextCursor(cursor);

    if(!fileSaveTimer.isActive()){
        preUndoSetup();
    }
    fileSaveTimer.start();
}

void MkEdit::applyMkEffects(const int blockNumber)
{
    switch(undoData.editType){
    case undoRedo: break;
    case singleEdit: 	emit applyMkSingleBlock(blockNumber); break;
    case checkbox:
    case enterPressed:
    case multiEdit: 	emit applyAllMkData(blockNumber); break;
    }

    QTextCursor cursor = this->textCursor();
    int newPosition = this->document()->findBlockByNumber(this->selectRange.currentBlockNo).position() + this->selectRange.currentposInBlock;
    newPosition = (newPosition < 0) ? 0: newPosition;
    if(newPosition < 0 ||newPosition<this->document()->characterCount() ){
        cursor.setPosition(newPosition);
        this->setTextCursor(cursor);
    }

    this->verticalScrollBar()->setSliderPosition(undoData.scrollValue);
    if(!isTextCursorVisible()){
        this->ensureCursorVisible();
    }
}

void MkEdit::updateRawDocument()
{
    switch(undoData.editType){
    case EditType::undoRedo: break;
    case EditType::singleEdit:
        if(undoData.oldSelectRange.currentBlockNo == this->textCursor().blockNumber()){
            emit saveSingleRawBlock(textCursor().blockNumber()); return;
        }
    case EditType::checkbox:
    case EditType::enterPressed:
    case EditType::multiEdit:
        emit saveRawDocument();
    break;
    }
}

void MkEdit::fileSaveNow()
{
    fileSaveTimer.stop();
    updateRawDocument();
    postUndoSetup();
    emit fileSaveRaw();
    applyMkEffects(textCursor().blockNumber());
}

void MkEdit::fileSaveWithScroll()
{
    fileSaveTimer.stop();
    postUndoSetup();
    emit fileSaveRaw();
}

bool MkEdit::isTextCursorVisible()
{
    return this->cursorRect().contains(getVisibleRect());
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

    QFontMetrics metrics(this->currentFont());
    int width = metrics.horizontalAdvance(CHECKED_PIC);
    int last = this->document()->lastBlock().position()+this->document()->lastBlock().length();

    QTextCursor cursor(this->textCursor());
    int checkPos;
    QRect rect;
    for(auto it = mkDoc->checkMarkPosBegin(); it!= mkDoc->checkMarkPosEnd(); it++){
        checkPos = this->document()->findBlockByNumber((*it).first).position() + (*it).second;
        if(checkPos>last){
            continue;
        }
        cursor.setPosition(checkPos);
        rect = this->cursorRect(cursor);
        rect.setWidth(width);
        if(rect.contains(pointer)){
            disconnectSignals(true);
            undoData.scrollValue = this->verticalScrollBar()->sliderPosition();
            undoData.editType = EditType::checkbox;
            preUndoSetup();
            emit pushCheckBox(checkPos);
            fileSaveWithScroll();
            connectSignals(true);

            return true;
        }
    }

    int linkTextWidth = 0;
    int linkStart, linkEnd;
    for(auto it = mkDoc->linkPosBegin(); it!= mkDoc->linkPosEnd(); ++it){
        linkStart = this->document()->findBlockByNumber(std::get<0>(*it)).position() + std::get<1>(*it);
        linkEnd   = this->document()->findBlockByNumber(std::get<0>(*it)).position() + std::get<2>(*it);

        if(linkStart >last ||linkEnd >last ){
            continue;
        }
        cursor.setPosition(linkStart);

        const QString *linkTitle= std::get<3>(*it);
        if(linkTitle && !linkTitle->isEmpty()){
            linkTextWidth = metrics.horizontalAdvance(*linkTitle);
            rect = this->cursorRect(cursor);
            rect.setWidth(linkTextWidth);
            if(rect.contains(pointer)){
                emit pushLink(std::get<0>(*it),std::get<1>(*it));
                return true;
            }
        }
    }

    return false;
}

void MkEdit::connectSignals(bool override)
{
    if(this->isDisconnectedViaHighPriority && !override){
        return;
    }

    isDisconnectedViaHighPriority = false;
    QObject::connect(this,&MkEdit::cursorPositionChanged,
                     this, &MkEdit::cursorPositionChangedHandle);
}

void MkEdit::disconnectSignals(bool override)
{
    QObject::disconnect(this,&MkEdit::cursorPositionChanged,
                     this, &MkEdit::cursorPositionChangedHandle);
    if(override){
        this->isDisconnectedViaHighPriority = true;
    }
}

void MkEdit::setEditState(bool edit)
{
    this->setReadOnly(edit);
    this->update();
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
    menu.addAction(&lineWrapAction);
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

    cursor.clearSelection();
    this->setTextCursor(cursor);

    emit selectBlockCopy(cursor.blockNumber(), startPos, endPos);

    if(startPos == endPos){
        return;
    }

    cursor.setPosition(startPos);
    cursor.setPosition(endPos,QTextCursor::KeepAnchor);

    disconnectSignals(true);
    {
        this->setTextCursor(cursor);
    }
    connectSignals(true);
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

        brushDefault.setColor(codeBlockColor);
        brushDefault.setStyle(Qt::SolidPattern);

        penCodeBlock.setColor(codeBlockColor);
        penCodeBlock.setWidth(2);

        whitePen.setColor(Qt::gray);
        whitePen.setCapStyle(Qt::RoundCap);
        whitePen.setWidth(1);

        update();
    }
}

void MkEdit::insertFromMimeData(const QMimeData *source)
{
    bool isBlock = false;
    QTextCursor cursor = this->textCursor();
    int cursorBlockNo = cursor.blockNumber();
    int cursorPosInBlock = cursor.positionInBlock();
    bool hasSelection = cursor.hasSelection();

    disconnectSignals(true);
    emit checkIfCursorInBlock(isBlock,cursor);

    QString text = source->text();
    matchCodeBlockRegex = regexCodeBlock.match(text);

    undoData.scrollValue = this->verticalScrollBar()->sliderPosition(); //this is important
    undoData.editType = EditType::multiEdit;
    //if the mime text itself is a code block
    if(matchCodeBlockRegex.hasMatch()){
        emit removeAllMkData(cursor.blockNumber());
        restoreTextCursor(cursorBlockNo, cursorPosInBlock, hasSelection);
        cursor = this->textCursor();
        preUndoSetup();

        if(isBlock){
            cursor.insertBlock();
        }
        cursor.insertText(text);
    }else{

        if(!isBlock){
            emit removeAllMkData(this->textCursor().blockNumber());
            restoreTextCursor(cursorBlockNo, cursorPosInBlock, hasSelection);
            cursor = this->textCursor();
            preUndoSetup();

            QString link = source->text();
            matchUrl = regexUrl.match(link);
            matchFolderFile = regexFolderFile.match(link);
            if (matchUrl.hasMatch() && !link.contains(LINK_SYMBOL_URL_START)) {
                int pos = cursor.position()+1;
                QString symbolsWithLink = QString(LINK_SYMBOL_TITLE_START) + LINK_SYMBOL_MID + link + LINK_SYMBOL_URL_END;
                cursor.insertText(symbolsWithLink);
                cursor.setPosition(pos);
                this->setTextCursor(cursor);
            }else if(matchFolderFile.hasMatch() && !link.contains(LINK_SYMBOL_URL_START)) {
                int pos = cursor.position()+1;
                QString symbolsWithLink =  QString(LINK_SYMBOL_TITLE_START) + LINK_SYMBOL_MID + "file:///"+link+LINK_SYMBOL_URL_END;
                cursor.insertText(symbolsWithLink);
                cursor.setPosition(pos);
                this->setTextCursor(cursor);
            }
            else{
                QTextEdit::insertFromMimeData(source);
            }
        }else{
            emit removeAllMkData(cursor.blockNumber());
            restoreTextCursor(cursorBlockNo, cursorPosInBlock, hasSelection);
            cursor = this->textCursor();
            preUndoSetup();

            QTextEdit::insertFromMimeData(source);
        }
    }
    //save the update cursor position
    this->selectRange.currentBlockNo = textCursor().blockNumber();
    this->selectRange.currentposInBlock = textCursor().positionInBlock();

    emit saveRawDocument();
    postUndoSetup();
    emit fileSaveRaw();
    emit applyAllMkData(this->textCursor().blockNumber());

    //restore the saved cursor position
    cursor = this->textCursor();
    int newPosition = this->document()->findBlockByNumber(this->selectRange.currentBlockNo).position() + this->selectRange.currentposInBlock;
    newPosition = (newPosition < 0) ? 0: newPosition;
    if(newPosition < 0 ||newPosition<this->document()->characterCount() ){
        cursor.setPosition(newPosition);
        this->setTextCursor(cursor);
    }

    //ensure the cursor is always visible
    this->verticalScrollBar()->setSliderPosition(undoData.scrollValue);
    if(!isTextCursorVisible()){
        this->ensureCursorVisible();
    }

    connectSignals(true);
}

void MkEdit::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton && !this->textCursor().hasSelection()){
        const QPoint pos (e->position().x(),e->position().y());
        this->setTextCursor(this->cursorForPosition(pos));
    }

    if(!isMouseOnCheckBox(e)){
        if(e->button() == Qt::LeftButton){
            selectRange.isFirstMousePress = true;
        }

        bool selectionState = textCursor().hasSelection();
        isCursorChangedHandleTriggered = false;
        QTextEdit::mousePressEvent(e);
        if(selectionState != textCursor().hasSelection() && !isCursorChangedHandleTriggered){
            cursorPositionChangedHandle();
        }
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
    QFontMetrics metrics(this->currentFont());

    if(nullptr == mkDoc){
        return;
    }

    int width = metrics.horizontalAdvance(CHECKED_PIC);
    int last = this->document()->lastBlock().position()+this->document()->lastBlock().length();

    QTextCursor cursor(this->textCursor());
    QRect rect;
    int checkPos;
    for(auto it = mkDoc->checkMarkPosBegin(); it!= mkDoc->checkMarkPosEnd(); it++){
        checkPos = this->document()->findBlockByNumber((*it).first).position() + (*it).second;
        if((checkPos>last)){
            continue;
        }
        cursor.setPosition(checkPos);
        rect = this->cursorRect(cursor);
        rect.setWidth(width);
        if(rect.contains(pointer)){
            this->viewport()->setCursor(Qt::CursorShape::PointingHandCursor);
            return;
        }
    }

    int linkTextWidth = 0;
    int linkStart, linkEnd;

    for(auto it = mkDoc->linkPosBegin(); it!= mkDoc->linkPosEnd(); ++it){
        linkStart = this->document()->findBlockByNumber(std::get<0>(*it)).position() + std::get<1>(*it);
        linkEnd   = this->document()->findBlockByNumber(std::get<0>(*it)).position() + std::get<2>(*it);

        if(linkStart >last || linkEnd >last ){
            continue;
        }
        cursor.setPosition(linkStart);

        const QString *linkTitle= std::get<3>(*it);
        if(linkTitle && !linkTitle->isEmpty()){
            linkTextWidth = metrics.horizontalAdvance(*linkTitle);
            rect = this->cursorRect(cursor);
            rect.setWidth(linkTextWidth);
            if(rect.contains(pointer)){
                this->viewport()->setCursor(Qt::CursorShape::PointingHandCursor);
                return;
            }
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

QColor MkEdit::getSearchMatchColor() const
{
    return syntaxColor.searchMatch;
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

void MkEdit::setSearchMatchColor(const QColor &color)
{
    syntaxColor.searchMatch = color;
}

void MkEdit::setKeywordColor(const QColor &color)
{
    syntaxColor.keyword = color;
    emit syntaxColorUpdate(syntaxColor);
}

void MkEdit::setDocument(QTextDocument *document)
{
    disconnectSignals(true);
    QTextEdit::setDocument(document);
    connectSignals(true);
}

QString MkEdit::rawPlainText() const
{
    MkTextDocument *mkDoc = dynamic_cast<MkTextDocument*>(this->document());
    if(nullptr == mkDoc){
        return QTextEdit::toPlainText();
    }
    return mkDoc->getRawDocument()->toPlainText();
}

void MkEdit::setMkState(bool enable)
{
    if(enable){
        disableMarkdown.setText("Disable Markdown");
        emit setMarkdownStatus(true);
    }else{
        disableMarkdown.setText("Enable Markdown");
        emit setMarkdownStatus(false);
    }
}

void MkEdit::setFont(const QFont &font)
{
    QFont menuFont = font;
    menuFont.setPointSize(10);
    undoAction.setFont(menuFont);
    redoAction.setFont(menuFont);
    copyTextAction.setFont(menuFont);
    pasteTextAction.setFont(menuFont);
    deleteTextAction.setFont(menuFont);
    selectAllAction.setFont(menuFont);
    selectBlockAction.setFont(menuFont);
    disableMarkdown.setFont(menuFont);
    lineWrapAction.setFont(menuFont);

    QTextEdit::setFont(font);
}

void MkEdit::fileSaveHandle()
{
    fileSaveWithScroll();
}

void MkEdit::diableMarkdown_internal()
{
    QSettings settings("Remini","Remini");
    bool markdownStatus = settings.value("markdown",true).toBool();
    setMkState(!markdownStatus);
    settings.setValue("markdown",!markdownStatus);
}

void MkEdit::lineWrapHandler()
{
    if("Enable line-wrap" == lineWrapAction.text()){
        this->setLineWrapMode(QTextEdit::WidgetWidth);
        lineWrapAction.setText("Disable line-wrap");
    }else{
        this->setLineWrapMode(QTextEdit::NoWrap);
        lineWrapAction.setText("Enable line-wrap");
    }
}

void MkEdit::cursorPositionChangedHandle()
{
    isCursorChangedHandleTriggered = true;
    QTextCursor cursor = this->textCursor();

    if(selectRange.isFirstMousePress && !isShiftKeyPressed){
        isCalcuatedForStartPos = false;
        selectRange.isFirstMousePress = false;
        if(!cursor.hasSelection()){
            selectRange.selectionFirstStartBlock = cursor.blockNumber();
            selectRange.selectionFirstStartPosInBlock = cursor.positionInBlock();
        }
        selectRange.selectionEndBlock = selectRange.selectionFirstStartBlock;
        selectRange.selectionEndPosInBlock = selectRange.selectionFirstStartPosInBlock;
    }

    if(!cursor.hasSelection()){
        selectRange.hasSelection = false;
        selectRange.currentBlockNo = cursor.blockNumber();
        selectRange.currentposInBlock = cursor.positionInBlock();

        selectRange.selectionEndBlock = selectRange.selectionFirstStartBlock;
        selectRange.selectionEndPosInBlock = selectRange.selectionFirstStartPosInBlock;
    }else{
        selectRange.hasSelection = true;
        selectRange.selectionEndBlock = cursor.blockNumber();
        selectRange.selectionEndPosInBlock = cursor.positionInBlock();
    }

    Connector connector(
        std::bind(&MkEdit::disconnectSignals,this,std::placeholders::_1),
        std::bind(&MkEdit::connectSignals,this,std::placeholders::_1)
    );
    emit cursorPosChanged(&selectRange,this->isReadOnly());
    postCursorPosChangedSignal();
}
