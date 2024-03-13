#include <QElapsedTimer>
#include "blockdata.h"
#include "linedata.h"
#include "mkedit.h"

MkEdit::MkEdit(QWidget *parent):QTextEdit(parent){

    fileSaveTimer.setInterval(FILE_SAVE_TIMEOUT);
    regexUrl.setPattern("(https?|ftp|file)://[\\w\\d._-]+(?:\\.[\\w\\d._-]+)+[\\w\\d._-]*(?:(?:/[\\w\\d._-]+)*/?)?(?:\\?[\\w\\d_=-]+(?:&[\\w\\d_=-]+)*)?(?:#[\\w\\d_-]+)?");
    regexCodeBlock.setPattern("```(?s)(.*?)```");
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
                    xBlock = this->x();
                    yBlock = block.layout()->position().y()-scrollPos - (fontSize*0.4);
                }
                else{
                    if(blockData->getStatus()==BlockData::end
                        || (blockData->getStatus()==BlockData::content && layout->blockBoundingRect(nextBlock).bottom()>= (rect.bottom()+40))){
                        int height = block.layout()->position().y() - yBlock + (fontSize*0.6)-scrollPos;

                        painter.drawRoundedRect(xBlock,yBlock,widthCodeBlock,height,BLOCKRADIUS,BLOCKRADIUS);
                    }
                }
            }else{
                LineData* lineData = dynamic_cast<LineData*>(data);
                if(lineData){
                    if(lineData->getStatus() == LineData::horizontalLine && lineData->getDraw()){
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
        }
        block = block.next();
    }
    QTextEdit::paintEvent(e);
}

void MkEdit::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);
    widthCodeBlock = event->size().width()+2 - this->verticalScrollBar()->width()/2;
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
    case Qt::Key_Escape: emit escapeFocus(this);return;
    case Qt::Key_Up:
    case Qt::Key_Right:
    case Qt::Key_Left:
    case Qt::Key_Down: setSelectionUsingArrowKeys(event->modifiers() == Qt::SHIFT);
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Alt:       QTextEdit::keyPressEvent(event);return;
    case Qt::Key_V:         if( event->modifiers() == Qt::CTRL) {pasteTextAction.trigger();return;}break;
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
    case Qt::Key_Z:         if( event->modifiers() == Qt::CTRL) emit undoStackUndoSignal(); undoData.undoRedoSkip = true; fileSaveNow(); scrollValueUpdateHandle(undoData.scrollValue); showSelectionAfterUndo(); return;
    case Qt::Key_Y:         if( event->modifiers() == Qt::CTRL) emit undoStackRedoSignal(); undoData.undoRedoSkip = true; fileSaveNow(); return;

    default: break;
    }

    applyMkEffects();
}

void MkEdit::showSelectionAfterUndo(){
    MkTextDocument *mkDoc = dynamic_cast<MkTextDocument*>(this->document());
    if(nullptr == mkDoc){
        return ;
    }
    SelectRange range = mkDoc->getUndoSelectRange();

    QTextCursor textCursor = this->textCursor();
    int currentBlockNo = -1;
    if(range.hasSelection){
        selectRange.hasSelection = true;
        selectRange.selectionFirstStartBlock = range.selectionFirstStartBlock;
        selectRange.selectionFirstStartPosInBlock = range.selectionFirstStartPosInBlock;
        selectRange.selectionEndBlock = range.selectionEndBlock;
        selectRange.selectionEndPosInBlock = range.selectionEndPosInBlock;
        currentBlockNo = range.selectionEndBlock;
    }else{
        currentBlockNo = range.currentBlockNo;
        selectRange.hasSelection = false;
    }

    QObject::disconnect(this,&MkEdit::cursorPositionChanged,this,&MkEdit::cursorPositionChangedHandle);
    {
        //first show all the Markdown symbols in the editor
        emit cursorPosChanged( range.hasSelection, currentBlockNo, getVisibleRect(), &selectRange);

        if(range.hasSelection){
            int startInDoc = this->document()->findBlockByNumber(range.selectionFirstStartBlock).position() + range.selectionFirstStartPosInBlock;
            int endInDoc   = this->document()->findBlockByNumber(range.selectionEndBlock).position() + range.selectionEndPosInBlock;
            textCursor.clearSelection();
            textCursor.setPosition(startInDoc);
            textCursor.setPosition(endInDoc,QTextCursor::KeepAnchor);
        }else{
            int cursorPos = this->document()->findBlockByNumber(range.currentBlockNo).position() + range.currentposInBlock ;
            textCursor.setPosition(cursorPos);
        }

        //highlight the selection
        this->setTextCursor(textCursor);
    }
    QObject::connect(this,&MkEdit::cursorPositionChanged,this,&MkEdit::cursorPositionChangedHandle);
}

void MkEdit::setSelectionUsingArrowKeys(bool isShiftPressed)
{
    QTextCursor cursor = this->textCursor();
    if(!isShiftPressed){
        selectRange.selectionFirstStartBlock 		= selectRange.selectionEndBlock 		= cursor.blockNumber();
        selectRange.selectionFirstStartPosInBlock 	= selectRange.selectionEndPosInBlock 	= cursor.positionInBlock();
        selectRange.hasSelection = false;
        emit cursorPosChanged( selectRange.hasSelection, cursor.blockNumber(), getVisibleRect(), &selectRange);
        return;
    }

    if(!cursor.hasSelection()){
        selectRange.selectionFirstStartBlock = cursor.blockNumber();
        selectRange.selectionFirstStartPosInBlock = cursor.positionInBlock();
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
    emit autoInsertSymbol(this->textCursor().position());
}

void MkEdit::preUndoSetup()
{
    undoData.view               = this;
    undoData.doc                = this->document();
    undoData.oldText            = this->document()->toPlainText();
    undoData.undoRedoSkip       = false;
    undoData.selectAll          = false;
    undoData.oldSelectRange     = this->selectRange;
    undoData.oldSelectRange.hasSelection     	= this->textCursor().hasSelection();
    undoData.oldSelectRange.currentBlockNo 		= this->textCursor().blockNumber();
    undoData.oldSelectRange.currentposInBlock 	= this->textCursor().positionInBlock();
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

    QFontMetrics metrics(this->currentFont());
    int width = metrics.horizontalAdvance(CHECKED_PIC);
    int averageCharacterWidth = metrics.horizontalAdvance("t");
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
        linkTextWidth = ((*it).second - (*it).first) * averageCharacterWidth;
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

    disconnectSignals();
    {
        this->setTextCursor(cursor);
    }
    connectSignals();
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
        whitePen.setWidth(2);

        update();
    }
}

void MkEdit::insertFromMimeData(const QMimeData *source)
{
    bool isBlock = false;
    QTextCursor cursor = this->textCursor();
    disconnectSignals();
    emit checkIfCursorInBlock(isBlock,cursor);

    QString text = source->text();
    matchCodeBlockRegex = regexCodeBlock.match(text);

    //if the mime text itself is a code block
    if(matchCodeBlockRegex.hasMatch()){
        emit removeAllMkData(cursor.blockNumber());
        preUndoSetup();

        if(isBlock){
            cursor.insertBlock();
        }
        cursor.insertText(text);
    }else{

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
            emit removeAllMkData(cursor.blockNumber());
            preUndoSetup();
            QTextEdit::insertFromMimeData(source);
        }
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
        if(e->button() == Qt::LeftButton){
            selectRange.isFirstMousePress = true;
            selectRange.selectionFirstStartBlock = selectRange.selectionEndBlock = textCursor().blockNumber();
            selectRange.selectionFirstStartPosInBlock = selectRange.selectionEndPosInBlock = textCursor().positionInBlock();
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
    int averageCharacterWidth = metrics.horizontalAdvance("t");

    for(auto it = mkDoc->linkPosBegin(); it!= mkDoc->linkPosEnd(); ++it){
        if((*it).first >last ||(*it).second >last ){
            continue;
        }
        cursor.setPosition((*it).first);

        rect = this->cursorRect(cursor);
        linkTextWidth = ((*it).second - (*it).first) * averageCharacterWidth;
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
    disconnectSignals();
    QTextEdit::setDocument(document);
    connectSignals();
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

void MkEdit::scrollValueUpdateHandle(int value)
{
    disconnectSignals();
    {
        emit drawTextBlocks(textCursor().hasSelection(), textCursor().blockNumber(),undoData.selectAll, getVisibleRect(), &selectRange);
    }
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

    if(selectRange.isFirstMousePress){
        isCalcuatedForStartPos = false;
        selectRange.isFirstMousePress = false;
        selectRange.isCursorCaculated = false;
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
        selectRange.isCursorCaculated = false;

        selectRange.selectionEndBlock = selectRange.selectionFirstStartBlock;
        selectRange.selectionEndPosInBlock = selectRange.selectionFirstStartPosInBlock;


    }else{
        selectRange.hasSelection = true;
        selectRange.selectionEndBlock = cursor.blockNumber();
        selectRange.selectionEndPosInBlock = cursor.positionInBlock();
    }

    disconnectSignals();
    emit cursorPosChanged( textCursor().hasSelection(), textCursor().blockNumber(), getVisibleRect(), &selectRange);

    //insert cursor inbetween the formatted words since after symbols are inserted the positions are shifted
    if(!cursor.hasSelection() && selectRange.isCursorCaculated){
        cursor.setPosition(this->document()->findBlockByNumber(selectRange.currentBlockNo).position()+selectRange.currentposInBlock);
        this->setTextCursor(cursor);
    }

    if(!isCalcuatedForStartPos && selectRange.selectionFirstStartBlock == selectRange.currentBlockNo){
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
    }
    connectSignals();
}
