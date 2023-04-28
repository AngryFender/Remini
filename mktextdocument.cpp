#include "mktextdocument.h"
#include <QElapsedTimer>

MkTextDocument::MkTextDocument(QObject *parent)
    : QTextDocument{parent}
{
    regexCodeBlock.setPattern(CODEBLOCK_SYMBOL);
    regexHorizontalLine.setPattern(HORIZONTALLINE_SYMBOL);

    this->setUndoRedoEnabled(false);

    locBoldA.symbol = BOLD_SYMBOL_A;
    locBoldU.symbol = BOLD_SYMBOL_U;
    locItalicA.symbol = ITALIC_SYMBOL_A;
    locItalicU.symbol = ITALIC_SYMBOL_U;
    locStrike.symbol = STRIKETHROUGH_SYMBOL;

}

void MkTextDocument::setPlainText(const QString &text)
{
    if(text.isEmpty())
        return;

    QTextDocument::setPlainText(text);
    identifyUserData(false);
    emit clearUndoStack();
}

void MkTextDocument::setUndoRedoText(const QString &text)
{
    QTextDocument::clear();
    QTextDocument::setPlainText(text);
}

void MkTextDocument::clear()
{
    while(!savedBlocks.empty()){
        savedBlocks.dequeue();
    }
    savedBlocks.clear();
    QTextDocument::clear();
}

void MkTextDocument::cursorPosChangedHandle( bool hasSelection, int blockNumber,QRect rect)
{
    hideMKSymbolsFromDrawingRect(rect,hasSelection,blockNumber,false);
}

void MkTextDocument::removeAllMkDataHandle()
{
    showMKSymbolsFromSavedBlocks();
    stripUserData();
}

void MkTextDocument::applyAllMkDataHandle(bool hasSelection, int blockNumber, bool showAll,QRect rect)
{
    resetTextBlockFormat(blockNumber);
    identifyUserData(showAll, hasSelection);
    hideMKSymbolsFromDrawingRect(rect, hasSelection, blockNumber, showAll);
}

void MkTextDocument::identifyUserData(bool showAll, bool hasSelection)
{
    bool openBlock = false;
    QTextBlock startBlock;
    QTextBlockFormat blockFormat;

    for(QTextBlock tBlock = this->begin(); tBlock != this->end(); tBlock = tBlock.next()){

        QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(tBlock.text());
        if(matchCodeBlock.hasMatch()){
            BlockData *blockData = new BlockData;
            tBlock.setUserData(blockData);

            if(!openBlock){
                openBlock = true;
                blockData->setStatus(BlockData::start);
                startBlock = tBlock;
            }
            else{
                openBlock = false;
                blockData->setStatus(BlockData::end);
            }
        }
        else{
            if(openBlock){
                BlockData *blockData = new BlockData;
                blockData->setStatus(BlockData::content);
                tBlock.setUserData(blockData);
            }
            else{
                QRegularExpressionMatch matchHorizontalLine = regexHorizontalLine.match(tBlock.text());
                if(matchHorizontalLine.hasMatch()){
                    LineData *lineData = new LineData;
                    lineData->setStatus(LineData::horizontalLine);
                    tBlock.setUserData(lineData);
                }
                identifyFormatData(tBlock, showAll, hasSelection);
            }
        }
    }
}

void MkTextDocument::identifyFormatData(QTextBlock &block, bool showAll, bool hasSelection)
{
    resetFormatLocation();
    FormatData *formatData = new FormatData;
    QString text = block.text();
    int index1 = 0;

    if(HEADING1_SYMBOL == text.left(HEADING1_SYMBOL_COUNT)){
        QString symbol(HEADING1_SYMBOL);
        formatData->addFormat(0, HEADING1_SYMBOL_COUNT-1, symbol);  index1+=HEADING1_SYMBOL_COUNT;
    }else if(HEADING2_SYMBOL == text.left(HEADING2_SYMBOL_COUNT)){
        QString symbol(HEADING2_SYMBOL);
        formatData->addFormat(0, HEADING2_SYMBOL_COUNT-1, symbol);  index1+=HEADING2_SYMBOL_COUNT;
    }else if(HEADING3_SYMBOL == text.left(HEADING3_SYMBOL_COUNT)){
        QString symbol(HEADING3_SYMBOL);
        formatData->addFormat(0, HEADING3_SYMBOL_COUNT-1, symbol);  index1+=HEADING3_SYMBOL_COUNT;
    }

    int index2 = index1+1;
    int index3 = index2+1;
    int len = text.length();
    while(index1<len){    
        QString test = composeSymbol(text, index1, index2, index3);

        if(test != ""){
            if(test == "_**"){
                test = "_";
            }else if(test == "**_"){
                test = "**";
            }else if(test == "***" || test == "___"){
                index1+=3; index2+=3; index3+=3;
                continue;
            }
            if(test == ITALIC_SYMBOL_A){
                if(locItalicA.start == -1){
                    locItalicA.start = index1;
                }else{
                    locItalicA.end = index1;
                    if(locItalicA.end-locItalicA.start>1){
                        locItalicA.symbol = ITALIC_SYMBOL_A;
                        formatData->addFormat(locItalicA.start, locItalicA.end, locItalicA.symbol);
                        locItalicA.reset();
                    }
                }
            }else if(test == ITALIC_SYMBOL_U){
                if(locItalicU.start == -1){
                   locItalicU.start = index1;
                }else{
                    locItalicU.end = index1;
                    if(locItalicU.end-locItalicU.start>1){
                        locItalicU.symbol = ITALIC_SYMBOL_U;
                        formatData->addFormat(locItalicU.start, locItalicU.end, locItalicU.symbol);
                        locItalicU.reset();
                    }
                }
            }else if(test == BOLD_SYMBOL_A){
                if(locBoldA.start == -1){
                    locBoldA.start = index1;
                    index1+=2; index2+=2; index3+=2;
                    continue;
                }else{
                    locBoldA.end = index1;
                    if(locBoldA.end-locBoldA.start>1){
                        locBoldA.symbol = BOLD_SYMBOL_A;
                        formatData->addFormat(locBoldA.start, locBoldA.end, locBoldA.symbol);
                        locBoldA.reset();
                        index1+=2; index2+=2; index3+=2;
                        continue;
                    }
                }
            }else if(test == BOLD_SYMBOL_U){
                if(locBoldU.start == -1){
                    locBoldU.start = index1;
                    index1+=2; index2+=2; index3+=2;
                    continue;
                }else{
                    locBoldU.end = index1;
                    if(locBoldU.end-locBoldU.start>1){
                        locBoldU.symbol = BOLD_SYMBOL_U;
                        formatData->addFormat(locBoldU.start, locBoldU.end, locBoldU.symbol);
                        locBoldU.reset();
                        index1+=2; index2+=2; index3+=2;
                        continue;
                    }
                }
            }else if(test == STRIKETHROUGH_SYMBOL){
                if(locStrike.start == -1){
                    locStrike.start = index1;
                    index1+=2; index2+=2; index3+=2;
                    continue;
                }else{
                    locStrike.end = index1;
                    if(locStrike.end-locStrike.start>1){
                        locStrike.symbol = STRIKETHROUGH_SYMBOL;
                        formatData->addFormat(locStrike.start, locStrike.end, locStrike.symbol);
                        locStrike.reset();
                        index1+=2; index2+=2; index3+=2;
                        continue;
                    }
                }
            }else if(test == CHECK_SYMBOL_START){
                if(locCheck.start == -1){
                    locCheck.start = index1;
                    index1+=3; index2+=3; index3+=3;
                    continue;
                }
            }else if(test == CHECKED_SYMBOL_END && locCheck.start != -1){
                locCheck.end = index1;
                if((locCheck.end-locCheck.start)==3){
                    locCheck.symbol = CHECKED_SYMBOL_END;
                    formatData->addFormat(locCheck.start, locCheck.end, locCheck.symbol);
                    locCheck.reset();
                    index1+=3; index2+=3; index3+=3;
                    continue;
                }
            } else if(test == UNCHECKED_SYMBOL_END && locCheck.start != -1){
                locCheck.end = index1;
                if((locCheck.end-locCheck.start)==3){
                    locCheck.symbol = UNCHECKED_SYMBOL_END;
                    formatData->addFormat(locCheck.start, locCheck.end, locCheck.symbol);
                    locCheck.reset();
                    index1+=3; index2+=3; index3+=3;
                    continue;
                }
            }
        }
        index1++;
        index2++;
        index3++;
    }

    if(!formatData->isEmpty()){
        block.setUserData(formatData);
        formatData->sortAscendingPos();
    }
}

QString MkTextDocument::convertCharacterToSymbol(QChar single)
{
    QString result = "";
    if(single =='*'){
        result = "*";
    }else if (single == '_'){
        result = "_";
    }else if (single == '~'){
        result = "~";
    }
    return result;
}

QString MkTextDocument::convertCharacterToCheckboxSymbol(QChar single)
{
    QString result = "";
    if(single == '-'){
        result = "-";
    }else if (single == ' '){
        result = " ";
    }else if (single == '['){
        result = "[";
    }else if (single == ']'){
        result = "]";
    }else if (single == 'x'){
        result = "x";
    }
    return result;
}

QString MkTextDocument::composeSymbol(QString &text, int &index1, int &index2, int &index3)
{
    QString first, second, third;
    int len = text.length();

    if(index3<text.length()){
        first  = convertCharacterToCheckboxSymbol(text.at(index1));
        second = convertCharacterToCheckboxSymbol(text.at(index2));
        third  = convertCharacterToCheckboxSymbol(text.at(index3));
        QString result = first+second+third;
        if(result == CHECK_SYMBOL_START ||
           result == CHECKED_SYMBOL_END ||
           result == UNCHECKED_SYMBOL_END){
            return result;
        }
    }
    first.clear();
    second.clear();
    third.clear();

    first = convertCharacterToSymbol(text.at(index1));
    if(first == ""){
        return "";
    }

    if(index2 >= len){
        return first;
    }

    second = convertCharacterToSymbol(text.at(index2));
    if(second ==""){
        return first;
    }

    if(index3>= len){
        return first+second;
    }
    third = convertCharacterToSymbol(text.at(index3));

    return(first+second+third);
}

void MkTextDocument::setCodeBlockMargin(QTextBlock &block, int leftMargin,int rightMargin, int topMargin)
{
    QTextCursor cursor(block);
    QTextBlockFormat blockFormat = cursor.blockFormat();
    blockFormat = cursor.blockFormat();
    blockFormat.setLeftMargin(leftMargin);
    blockFormat.setRightMargin(rightMargin);
    blockFormat.setTopMargin(topMargin);
    cursor.setBlockFormat(blockFormat);
}

void MkTextDocument::stripUserData()
{
    bool openBlock = false;
    for(QTextBlock tBlock = this->begin(); tBlock != this->end(); tBlock = tBlock.next()){
        tBlock.setUserData(nullptr);
        QTextCursor cursor(tBlock);
        QTextBlockFormat format;
        cursor.setBlockFormat(format);
    }
}

void MkTextDocument::resetTextBlockFormat(int blockNumber)
{
    QTextBlock block = this->findBlockByNumber(blockNumber);
    QTextCursor cursor(block);
    QTextCharFormat format;
    cursor.setPosition(block.position());
    cursor.setPosition(block.position()+block.length()-1, QTextCursor::KeepAnchor);
    cursor.setCharFormat(format);
}

void MkTextDocument::applyMkFormat(QTextBlock &block, int start, int end, FragmentData::FormatSymbol status)
{
    QTextCharFormat format;

    switch(status){
    case FragmentData::BOLD:format.setFontWeight(QFont::ExtraBold);break;
    case FragmentData::ITALIC:format.setFontItalic(true);break;;
    case FragmentData::STRIKETHROUGH:format.setFontStrikeOut(true);break;
    case FragmentData::HEADING1:format.setFontPointSize(this->defaultFont().pointSize() *2);    end = block.length()-1; break;
    case FragmentData::HEADING2:format.setFontPointSize(this->defaultFont().pointSize() *1.5);  end = block.length()-1; break;
    case FragmentData::HEADING3:format.setFontPointSize(this->defaultFont().pointSize() *1.25); end = block.length()-1; break;
    default:break;
    }

    QTextCursor cursor(block);
    int startPoint = block.position()+start;
    int endPoint = block.position()+end;
    if(end>=block.length()){
        endPoint = block.length()-1;
    }
    cursor.setPosition(startPoint);
    cursor.setPosition(endPoint, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
}

void MkTextDocument::hideSymbols(QTextBlock block,const QString &symbol)
{
    QString textBlock = block.text();
    QRegularExpression regex(symbol);
    textBlock.replace(regex,"");

    QTextCursor editCursor(block);
    editCursor.movePosition(QTextCursor::StartOfBlock);
    editCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    editCursor.removeSelectedText();

    editCursor.insertText(textBlock);
}

void MkTextDocument::hideAllFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData)
{
    QString textBlock = block.text();
    for(QVector<PositionData*>::Iterator it = formatData->pos_end()-1; it >= formatData->pos_begin(); it--)
    {
        hideSymbolsAtPos(textBlock, (*it)->getPos(), (*it)->getSymbol());
    }
    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
    cursor.insertText(textBlock);
}

void MkTextDocument::hideSymbolsAtPos(QString &text, int pos, const QString &symbol)
{
    //qDebug()<<"start =" <<pos<<" sybol = "<<symbol<< " text ="<<text<< " font size ="<<this->defaultFont().pointSize() ;
    text.remove(pos,symbol.length());
    if(symbol == CHECKED_SYMBOL_END){
        text.insert(pos,CHECKED_PIC);
    }else if(symbol == UNCHECKED_SYMBOL_END){
        text.insert(pos,UNCHECKED_PIC);
    }
    //qDebug()<<"start =" <<pos<<" sybol = "<<symbol<< " text ="<<text<< " font size ="<<this->defaultFont().pointSize() ;
}

void MkTextDocument::showSymbols(QTextBlock block, const QString &symbol)
{
    //avoid prepending more than 3 ```
    QRegularExpression regex(symbol);
    QRegularExpressionMatch matchCodeBlock = regex.match(block.text());
    if(matchCodeBlock.hasMatch()){
        return;
    }

    //only prepending the start and end of the codeblocks
    QTextCursor editCursor(block);
    QString newText= block.text().prepend(symbol);
    editCursor.movePosition(QTextCursor::StartOfBlock);
    editCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    editCursor.removeSelectedText();
    editCursor.insertText(newText);

}

void MkTextDocument::showAllFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData)
{
    QString textBlock = block.text();
    for(QVector<PositionData*>::Iterator it = formatData->pos_begin(); it < formatData->pos_end(); it++)
    {
        showSymbolsAtPos(textBlock, (*it)->getPos(), (*it)->getSymbol());
    }
    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
    cursor.insertText(textBlock);
}

void MkTextDocument::showSymbolsAtPos(QString &text, int pos, const QString &symbol)
{
    if(symbol == CHECKED_SYMBOL_END ||symbol == UNCHECKED_SYMBOL_END){
        text.remove(pos,1);
    }
    text.insert(pos,symbol);
    //qDebug()<<">>>>>>>>>>>> show start =" <<pos<<" sybol = "<<symbol<< " text ="<<text<< " font size ="<<this->defaultFont().pointSize() ;
}

void MkTextDocument::autoCompleteCodeBlock(int blockNumber ,bool &success)
{
    QTextCursor editCursor(this->findBlockByNumber(blockNumber));
    QTextBlock currentBlock = this->findBlockByNumber(blockNumber);

    QRegularExpressionMatch matchCodeBlockSymbol = regexCodeBlock.match(currentBlock.text());
    if(matchCodeBlockSymbol.hasMatch()){
        editCursor.insertText(CODEBLOCK_SYMBOL);
        editCursor.insertBlock();
        success = true;
    }else{
        success = false;
    }
}

BlockData* MkTextDocument::checkValidCodeBlock(QTextBlock &block)
{
    if(!block.isValid())
        return nullptr;

    QTextBlockUserData* data =block.userData();
    BlockData* blockData = dynamic_cast<BlockData*>(data);
    if(blockData){
        return blockData;
    }

    return nullptr;
}

void MkTextDocument::enterKeyPressedHandle(int blockNumber)
{
    numberListDetect(blockNumber);
}

void MkTextDocument::quoteLeftKeyPressedHandle(int blockNumber,bool &success)
{
    autoCompleteCodeBlock(blockNumber,success);
}

void MkTextDocument::checkRightClockOnCodeBlockHandle(int blockNumber, bool &valid)
{
    QTextBlock codeBlockSuspect = this->findBlockByNumber(blockNumber);
    BlockData* blockData = checkValidCodeBlock(codeBlockSuspect);
    if(blockData)
        valid = true;
    else
        valid = false;
}

void MkTextDocument::selectBlockCopyHandle(int blockNumber, int &startPos, int &endPos)
{
    QTextBlock currentBlock = this->findBlockByNumber(blockNumber);
    BlockData* blockData = checkValidCodeBlock(currentBlock);
    if(blockData){
        if(blockData->getStatus()==BlockData::end || blockData->getStatus()==BlockData::content){
            while(blockData->getStatus()!=BlockData::start){
                currentBlock = currentBlock.previous();
                blockData = checkValidCodeBlock(currentBlock);
            }
        }
        if(blockData->getStatus()==BlockData::start){
            startPos = currentBlock.next().position();
            while(blockData->getStatus()!=BlockData::end){
                currentBlock = currentBlock.next();
                blockData = checkValidCodeBlock(currentBlock);
                if(!blockData){
                    endPos = startPos;
                    return;
                }
            }
            endPos = currentBlock.previous().position()+currentBlock.previous().length()-1;
        }
    }
}

void MkTextDocument::duplicateLineHandle(int blockNumber)
{
    QTextBlock currentBlock = this->findBlockByNumber(blockNumber);
    QTextCursor editCursor(currentBlock);
    QString lineText = currentBlock.text();
    editCursor.movePosition(QTextCursor::EndOfBlock);
    editCursor.insertBlock();
    editCursor.insertText(lineText);
}

void MkTextDocument::smartSelectionHandle(int blockNumber, QTextCursor &cursor)
{
    QTextBlock currentBlock = this->findBlockByNumber(blockNumber);
    QString lineText = currentBlock.text();
    int currentBlockPos = currentBlock.position();
    int start = cursor.selectionStart() - currentBlockPos;
    int end = cursor.selectionEnd() - currentBlockPos;

    int lastCharacter = lineText.length();
    QChar firstCharacterCheck;
    QChar endCharacterCheck;

    if(!cursor.hasSelection()){
        cursor.movePosition(QTextCursor::StartOfWord);
        cursor.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
        return;
    }

    if((start >0)&&(end<lastCharacter)){
        firstCharacterCheck = lineText.at(start-1);
        endCharacterCheck = lineText.at(end) ;
        if(((firstCharacterCheck == '\"')&&(endCharacterCheck == '\"'))||
           ((firstCharacterCheck == '{')&&(endCharacterCheck == '}'))||
           ((firstCharacterCheck == '(')&&(endCharacterCheck == ')'))||
           ((firstCharacterCheck == '[')&&(endCharacterCheck == ']'))){
            if(start > 0) start--;
            if(end < lastCharacter) end++;

            cursor.setPosition(start+ currentBlockPos);
            cursor.setPosition(end + currentBlockPos,QTextCursor::KeepAnchor);
            return;
        }

        if(((firstCharacterCheck == '{')&&(endCharacterCheck == '\"'))||
        ((firstCharacterCheck == '{')&&(endCharacterCheck == ')'))||
        ((firstCharacterCheck == '{')&&(endCharacterCheck == ']'))||
        ((firstCharacterCheck == '(')&&(endCharacterCheck == '\"')) ||
        ((firstCharacterCheck == '(')&&(endCharacterCheck == '}')) ||
        ((firstCharacterCheck == '(')&&(endCharacterCheck == ']')) ||
        ((firstCharacterCheck == '[')&&(endCharacterCheck == '\"')) ||
        ((firstCharacterCheck == '[')&&(endCharacterCheck == ')')) ||
        ((firstCharacterCheck == '[')&&(endCharacterCheck == ']'))){
            if(end < lastCharacter) end++;

            cursor.setPosition(start+ currentBlockPos);
            cursor.setPosition(end + currentBlockPos,QTextCursor::KeepAnchor);
            return;
        }

        if(((firstCharacterCheck == '\"')&&(endCharacterCheck == '}'))||
        ((firstCharacterCheck == '\"')&&(endCharacterCheck == ')'))||
        ((firstCharacterCheck == '\"')&&(endCharacterCheck == ']'))||
        ((firstCharacterCheck == '}')&&(endCharacterCheck == '\"'))||
        ((firstCharacterCheck == '}')&&(endCharacterCheck == ')'))||
        ((firstCharacterCheck == '}')&&(endCharacterCheck == ']'))||
        ((firstCharacterCheck == ')')&&(endCharacterCheck == '\"')) ||
        ((firstCharacterCheck == ')')&&(endCharacterCheck == '}')) ||
        ((firstCharacterCheck == ')')&&(endCharacterCheck == ']')) ||
        ((firstCharacterCheck == ']')&&(endCharacterCheck == '\"')) ||
        ((firstCharacterCheck == ']')&&(endCharacterCheck == ')')) ||
        ((firstCharacterCheck == ']')&&(endCharacterCheck == ']'))){
            if(start > 0) start--;

            cursor.setPosition(start+ currentBlockPos);
            cursor.setPosition(end + currentBlockPos,QTextCursor::KeepAnchor);
            return;
        }
    }

    //skip spaces on the left side of the selection
    while(start >0){
        start--;
        if(!lineText.at(start).isSpace())
            break;
    }

    while(start >=0){
        if(lineText.at(start)== QChar('\"')){
            if(!(cursor.selectedText().contains("\""))){
                firstCharacterCheck = '\"'; break;
            }
        }else if(lineText.at(start)== QChar('{')){
            if(!(cursor.selectedText().contains("{")||cursor.selectedText().contains("}"))){
                firstCharacterCheck = '{'; break;
            }
        }else if(lineText.at(start)== QChar('(')){
            if(!(cursor.selectedText().contains("(")||cursor.selectedText().contains(")"))){
                firstCharacterCheck = '('; break;
            }
        }else if(lineText.at(start)== QChar('[')){
            if(!(cursor.selectedText().contains("[")||cursor.selectedText().contains("]"))){
                firstCharacterCheck = '['; break;
            }
        }else if(lineText.at(start).isSpace()){
            firstCharacterCheck = ' ';break;
        }
        start--;
    }
    start++;


    if(end < lastCharacter){
        //sometimes selection can start at the middle of a word
        if(lineText.at(end).isSpace()){
            //skip spaces on the right side of the selection
            while(end< (lastCharacter-1)){
                end++;
                if(!lineText.at(end).isSpace())
                    break;
            }
        }
    }

    while(end < lastCharacter){
        if(lineText.at(end)== QChar('\"')){
            if(!(cursor.selectedText().contains("\""))){
                endCharacterCheck = '\"'; break;
            }
        }else if(lineText.at(end)== QChar('}')){
            if(!(cursor.selectedText().contains("{")||cursor.selectedText().contains("}"))){
                endCharacterCheck = '}'; break;
            }
        }else if(lineText.at(end)== QChar(')')){
            if(!(cursor.selectedText().contains("(")||cursor.selectedText().contains(")"))){
                endCharacterCheck = ')'; break;
            }
        }else if(lineText.at(end)== QChar(']')){
            if(!(cursor.selectedText().contains("[")||cursor.selectedText().contains("]"))){
                endCharacterCheck = ']'; break;
            }
        }else if(lineText.at(end).isSpace()){
            endCharacterCheck = ' ';break;
        }
        end++;
    }
    cursor.setPosition(start+ currentBlockPos);
    cursor.setPosition(end + currentBlockPos,QTextCursor::KeepAnchor);
}


void MkTextDocument::drawTextBlocksHandler(bool hasSelection, int blockNumber, bool showAll, QRect rect)
{
    showMKSymbolsFromSavedBlocks(&rect);
    hideMKSymbolsFromDrawingRect(rect, hasSelection, blockNumber,showAll);
}

void MkTextDocument::showMKSymbolsFromSavedBlocks(QRect *rect)
{
//    QAbstractTextDocumentLayout* layout = this->documentLayout();

    while(!savedBlocks.empty()){
        QTextBlock block = savedBlocks.takeFirst();

//        if((optimiseScrolling)&&(rect!=nullptr)&&( layout->blockBoundingRect(block).bottom() < (rect->bottom()+40) && layout->blockBoundingRect(block).top() > (rect->top()-15))){
//            qDebug()<<"optimise container size"<<savedBlocks.count();
//            continue;
//        }

        QTextBlockUserData* data =block.userData();
        if(data == nullptr){
            block = block.next();
//            QTextCursor cursor(block);
//            QTextCharFormat format;
//            format.setFontPointSize(this->defaultFont().pointSize());
//            cursor.setPosition(block.position());
//            cursor.setPosition(block.length()-1, QTextCursor::KeepAnchor);
//            cursor.setCharFormat(format);
            continue;
        }

        BlockData* blockData = dynamic_cast<BlockData*>(data);
        if(blockData)
        {
            if(blockData->getStatus()!=BlockData::content)
            {
                showSymbols(block, CODEBLOCK_SYMBOL);
                showSymbols(block, CODEBLOCK_SYMBOL);
            }
        }
        else{
            LineData* lineData = dynamic_cast<LineData*>(data);
            if(lineData){
                lineData->setDraw(false);
                showSymbols(block, lineData->getSymbol());
            }
            FormatData* formatData = dynamic_cast<FormatData*>(data);
            if(formatData){
                if(formatData->isHidden()){
                    showAllFormatSymbolsInTextBlock(block, formatData);
                    formatData->setHidden(false);
                }
            }
        }
    }
}

void MkTextDocument::hideMKSymbolsFromDrawingRect(QRect rect, bool hasSelection, int blockNumber, bool showAll)
{
    int fontSize =this->defaultFont().pointSize();
    QAbstractTextDocumentLayout* layout = this->documentLayout();
    CheckingBlock checkBlock;
    QTextBlock block = this->firstBlock();

    while(block.isValid()){
        if( layout->blockBoundingRect(block).bottom() < (rect.bottom()+40) && layout->blockBoundingRect(block).top() > (rect.top()-15)){

            QTextBlockUserData* data =block.userData();
            if(data == nullptr){
                block = block.next();
                continue;
            }

            BlockData* blockData = dynamic_cast<BlockData*>(data);
            if(blockData)
            {
                if(blockData->getStatus()==BlockData::start)
                {
                    checkBlock.start = block;
                    setCodeBlockMargin(block, fontSize*3/4, fontSize, fontSize);
                }
                else if(blockData->getStatus()==BlockData::end)
                {
                    if(!checkBlock.start.isValid()){
                        block = block.next();
                        continue;
                    }
                    checkBlock.end = block;
                    setCodeBlockMargin(block,fontSize*3/4, fontSize, 0);
                    if(blockNumber >= checkBlock.start.blockNumber() && blockNumber <= checkBlock.end.blockNumber()){
                        showSymbols(checkBlock.start, CODEBLOCK_SYMBOL);
                        showSymbols(checkBlock.end, CODEBLOCK_SYMBOL);
                    }
                    else{
                        if(!hasSelection){
                            hideSymbols(checkBlock.start, CODEBLOCK_SYMBOL);
                            hideSymbols(checkBlock.end, CODEBLOCK_SYMBOL);
                        }
                    }
                }else if(blockData->getStatus()==BlockData::content){
                    setCodeBlockMargin(block, fontSize*5/3, fontSize, 0);
                }
            }
            else{
                int currentBlockNumber = block.blockNumber();

                LineData* lineData = dynamic_cast<LineData*>(data);
                if(lineData){
                    if(blockNumber == currentBlockNumber){
                        lineData->setDraw(false);
                        showSymbols(block, lineData->getSymbol());
                    }else{
                        if(!hasSelection){
                            lineData->setDraw(true);
                            hideSymbols(block, lineData->getSymbol());
                        }
                    }
                }
                FormatData* formatData = dynamic_cast<FormatData*>(data);
                if(formatData){

                    if(blockNumber == currentBlockNumber){
                        if(formatData->isHidden()){
                            formatData->setHidden(false);
                            resetTextBlockFormat(currentBlockNumber);
                            showAllFormatSymbolsInTextBlock(block, formatData);

                        }
                        for(QVector<FragmentData*>::Iterator it = formatData->formats_begin(); it < formatData->formats_end(); it++)
                        {
                            applyMkFormat(block, (*it)->getStart(), (*it)->getEnd(), (*it)->getStatus());
                        }
                    }
                    else{
                        if(!hasSelection){
                            if(!formatData->isHidden()){
                                formatData->setHidden(true);
                                resetTextBlockFormat(currentBlockNumber);
                                hideAllFormatSymbolsInTextBlock(block,formatData);
                                if(!formatData->isHiddenFormatsEmpty()){
                                    for(QVector<FragmentData*>::Iterator it = formatData->hiddenFormats_begin(); it < formatData->hiddenFormats_end(); it++)
                                    {
                                        applyMkFormat(block, (*it)->getStart(), (*it)->getEnd(), (*it)->getStatus());
                                    }
                                }
                            }
                        }
                    }
                }
            }
            savedBlocks.append(block);
        }
        block = block.next();
    }
}


void MkTextDocument::resetFormatLocation()
{
    locBoldA.reset();
    locBoldU.reset();
    locItalicA.reset();
    locItalicU.reset();
    locStrike.reset();
    locCheck.reset();
    locHeading1.reset();
    locHeading2.reset();
    locHeading3.reset();
}

void MkTextDocument::numberListDetect(int blockNumber)
{
    QTextCursor editCursor(this->findBlockByNumber(blockNumber));
    QTextBlock currentBlock = this->findBlockByNumber(blockNumber-1);
    QString lineText = currentBlock.text();

    regexNumbering.setPattern("^( {0,}|\\s{2,})\\d+\\.\\s*.*$");                    //look for " 10. abc123 abc123", "   1. abc123 abc"," 10. abc123 abc123","   1. abc123 abc","10. abc123 abc123","1. abc123 abc","10. abc123","1. abc123"," 1. ",
    QRegularExpressionMatch matchNumbering = regexNumbering.match(lineText);
    if(matchNumbering.hasMatch()){
        int spaces = numberListGetSpaces(matchNumbering.captured(0));
        editCursor.insertText(QString("").leftJustified(spaces,' '));
        editCursor.insertText(numberListGetNextNumber(matchNumbering.captured(0)));
        return;
    }
}

int MkTextDocument::numberListGetSpaces(const QString &text)
{
    regexNumbering.setPattern("^\\s+"); //look for spaces
    QRegularExpressionMatch matchSpace = regexNumbering.match(text);
    if (matchSpace.hasMatch()){
        return matchSpace.captured(0).length();
    }
    return 0;
}

QString MkTextDocument::numberListGetNextNumber(const QString &text)
{
    regexNumbering.setPattern("[0-9]+"); //look for numbers
    QRegularExpressionMatch matchSpace = regexNumbering.match(text);
    if (matchSpace.hasMatch()){
        int number =  matchSpace.captured(0).toInt();
        return QString::number(number+1)+". ";
    }
    return "";
}
