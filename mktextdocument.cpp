#include "mktextdocument.h"
#include <QElapsedTimer>

MkTextDocument::MkTextDocument(QObject *parent)
    : QTextDocument{parent}
{
    regexCodeBlock.setPattern(CODEBLOCK_SYMBOL);
    regexHorizontalLine.setPattern(HORIZONTALLINE_SYMBOL);
    regexBulletCheckBox.setPattern("\\s*- - \\[.\\] \\s*");
    regexCheckBox.setPattern("\\s*- \\[.\\] \\s*");
    this->setUndoRedoEnabled(false);
    linkColor.setRgb(51,102,204);
    disableMarkdownState = false;
}

void MkTextDocument::setPlainText(const QString &text)
{
    if(text.isEmpty())
        return;

    QTextDocument::setPlainText(text);
    this->rawDocument.setPlainText(text);
    identifyUserData();
    undoStack.clear();
}

void MkTextDocument::setUndoRedoText(const QString &text)
{
    QTextDocument::clear();
    QTextDocument::setPlainText(text);
    this->rawDocument.setPlainText(text);
}

void MkTextDocument::setUndoRedoText(const int blockNo, const QString &text)
{
    QTextCursor cursor(this);
    cursor.setPosition(findBlockByNumber(blockNo).position());
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.insertText(text);
}

void MkTextDocument::setUndoSelectRange(const SelectRange range)
{
    this->undoSelectRange = range;
}

void MkTextDocument::setRedoSelectRange(const int blockNo, const int posInBlock, const bool isCheckBox, const int scrollValue)
{
    this->redoSelectRange.currentBlockNo = blockNo;
    this->redoSelectRange.currentposInBlock = posInBlock;
    this->redoSelectRange.isCheckBox = isCheckBox;
    this->redoSelectRange.scrollValue = scrollValue;
}

const SelectRange &MkTextDocument::getUndoSelectRange() const
{
    return this->undoSelectRange;
}

const SelectRange &MkTextDocument::getRedoSelectRange() const
{
    return this->redoSelectRange;
}

void MkTextDocument::clear()
{
    checkMarkPositions.clear();
    linkPositions.clear();
    QTextDocument::clear();
}

void MkTextDocument::setFilePath(const QString &filePath)
{
    this->filePath = filePath;
}

void MkTextDocument::setFileName(const QString &fileName)
{
    this->fileName = fileName;
}

void MkTextDocument::setCursorPos(const int blockNo, const int characterNo)
{
    this->blockNo = blockNo;
    this->characterNo = characterNo;
}

int MkTextDocument::getBlockNo() const
{
    return this->blockNo;
}

int MkTextDocument::getCharacterNo() const
{
    return this->characterNo;
}

QString MkTextDocument::getFilePath() const
{
    return this->filePath;
}

QString MkTextDocument::getFileName() const
{
    return this->fileName;
}

QTextDocument *MkTextDocument::getRawDocument()
{
    return &rawDocument;
}

void MkTextDocument::cursorPosChangedHandle( bool hasSelection, int blockNumber,QRect rect, SelectRange * range)
{
    if(range){
        this->rawBlockInfo.hasSelection = range->hasSelection;
        if(range->hasSelection){
            this->rawBlockInfo.rawFirstBlock = range->selectionFirstStartBlock;
            this->rawBlockInfo.rawEndBlock = range->selectionEndBlock;

            if(range->selectionFirstStartBlock < range->selectionEndBlock){
                this->selectRange.startBlock = range->selectionFirstStartBlock;
                this->selectRange.endBlock   = range->selectionEndBlock;
            }else{
                this->selectRange.startBlock = range->selectionEndBlock;
                this->selectRange.endBlock   =  range->selectionFirstStartBlock;
            }
        }else{
            this->selectRange.startBlock = -1;
            this->selectRange.endBlock = -1;
            this->rawBlockInfo.rawFirstBlock = range->currentBlockNo;
        }
    }
    hideMKSymbolsFromDrawingRect(blockNumber,false, range, true);

}

void MkTextDocument::removeAllMkDataHandle(int blockNo)
{
    QTextDocument::setPlainText(this->rawDocument.toPlainText());
}

void MkTextDocument::applyAllMkDataHandle(int blockNumber, bool showAll, SelectRange*range)
{
    QTextBlock block = this->findBlockByNumber(blockNumber);
    block.setUserData(NULL);

    identifyUserData();
    hideMKSymbolsFromDrawingRect(blockNumber, showAll, nullptr,true);
}

void MkTextDocument::identifyUserData()
{
    bool openBlock = false;

    for(QTextBlock tBlock = this->begin(); tBlock != this->end(); tBlock = tBlock.next()){

        QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(tBlock.text());
        if(matchCodeBlock.hasMatch()){
            BlockData *blockData = new BlockData;
            tBlock.setUserData(blockData);

            if(!openBlock){
                openBlock = true;
                blockData->setStatus(BlockData::start);
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
                    tBlock.setUserData(lineData);
                }
                identifyFormatData(tBlock);
            }
        }
    }
}

void MkTextDocument::formatAllLines(const QTextDocument &original, MkTextDocument &formatted)
{
    bool openBlock = false;

    QTextCursor cursor(&formatted);
    cursor.movePosition(QTextCursor::Start);
    for(QTextBlock block = original.begin(); block != original.end(); block = block.next()){
        cursor.insertBlock();
        QString blockText = block.text();

        QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(blockText);
        if(matchCodeBlock.hasMatch()){
            BlockData *blockData = new BlockData;
            cursor.block().setUserData(blockData);

            if(!openBlock){
                openBlock = true;
                blockData->setStatus(BlockData::start);
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
                cursor.block().setUserData(blockData);
            }
            else{
                QRegularExpressionMatch matchHorizontalLine = regexHorizontalLine.match(block.text());
                if(matchHorizontalLine.hasMatch()){
                    LineData *lineData = new LineData;
                    cursor.block().setUserData(lineData);
                }
                identifyFormatData(block);
            }
        }
        cursor.insertText(blockText);
    }
}
void MkTextDocument::identifyFormatData(QTextBlock &block)
{
    resetFormatLocation();
    FormatData *formatData = new FormatData;
    QString text(block.text());
    QString textLink = text;
    QString test;

    int index1 = 0; insertHeadingData(text, index1, formatData);
    int index2 = index1+1;
    int index3 = index2+1;

    int len = text.length();
    while(index1<len){

        composeSymbolCombination(len, text, index1, index2, index3, test);

        if(test != ""){
            if(test == "_**"){
                test = "_";
            }else if(test == "**_"){
                test = "**";
            }else if(test == "***" || test == "___"){
                incrementIndexes(index1, index2,index3, test.size());
                continue;
            }

            if(locLink.start != -1){
                if(test == LINK_SYMBOL_URL_END){
                    locLink.end = index1;
                    QString linkText = textLink.mid(locLink.start+2, (locLink.end-locLink.start-1));
                    insertFormatLinkData(locLinkTitle,locLink, index1, index2, index3 , formatData, test, &linkText);
                }
            }else{

                if(test == ITALIC_SYMBOL_A){
                    insertFormatData(locItalicA, index1, index2, index3, formatData, test);
                    continue;
                }else if(test == ITALIC_SYMBOL_U){
                    insertFormatData(locItalicU, index1, index2, index3, formatData, test);
                    continue;
                }else if(test == BOLD_SYMBOL_A){
                    insertFormatData(locBoldA, index1, index2, index3, formatData, test);
                    continue;
                }else if(test == BOLD_SYMBOL_U){
                    insertFormatData(locBoldU,  index1, index2, index3, formatData, test);
                    continue;
                }else if(test == STRIKETHROUGH_SYMBOL){
                    insertFormatData(locStrike,  index1, index2, index3, formatData, test);
                    continue;
                }else if(test == CHECKED_SYMBOL_END && locCheck.start != -1){
                    insertFormatCheckBoxData(locCheck,  index1, index2, index3, formatData, QString(CHECKED_SYMBOL_END));
                    continue;
                } else if(test == UNCHECKED_SYMBOL_END && locCheck.start != -1){
                    insertFormatCheckBoxData(locCheck,  index1, index2, index3, formatData, QString(UNCHECKED_SYMBOL_END));
                    continue;
                }else if(test == CHECK_SYMBOL_START){
                    locCheck.start = index1;
                    incrementIndexes(index1, index2,index3, test.size());
                    continue;
                }else if(test == LINK_SYMBOL_TITLE_START){
                    locLinkTitle.start = index1;
                }else if(test == LINK_SYMBOL_MID && locLinkTitle.start != -1){
                    locLinkTitle.end = index1;
                    locLink.start = index2;
                }
            }
        }
        incrementIndexes(index1, index2,index3);
    }

    if(!formatData->isEmpty()){
        block.setUserData(formatData);
        formatData->sortAscendingPos();
    }
}

void MkTextDocument::insertHeadingData( const QString &text, int &index1, FormatData *formatData)
{
    if(HEADING1_SYMBOL == text.left(HEADING1_SYMBOL_COUNT)){
        formatData->addFormat(0, HEADING1_SYMBOL_COUNT-1, QString(HEADING1_SYMBOL));   index1+=HEADING1_SYMBOL_COUNT;
    }else if(HEADING2_SYMBOL == text.left(HEADING2_SYMBOL_COUNT)){
        formatData->addFormat(0, HEADING2_SYMBOL_COUNT-1, QString(HEADING2_SYMBOL));   index1+=HEADING2_SYMBOL_COUNT;
    }else if(HEADING3_SYMBOL == text.left(HEADING3_SYMBOL_COUNT)){
        formatData->addFormat(0, HEADING3_SYMBOL_COUNT-1, QString(HEADING3_SYMBOL));   index1+=HEADING3_SYMBOL_COUNT;
    }
}

void MkTextDocument::insertFormatData(FormatLocation &loc, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test)
{
    if(loc.start == -1){
        loc.start = index1;
    }else{
        loc.end = index1;
        if(loc.end-loc.start>1){
            formatData->addFormat(loc.start, loc.end, test);
            loc.reset();
        }
    }
    incrementIndexes(index1, index2,index3, test.size());
}

void MkTextDocument::insertFormatCheckBoxData(FormatLocation &loc, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test)
{
    loc.end = index1;
    if(loc.end-loc.start==3){
        formatData->addFormat(loc.start, loc.end, test);
        loc.reset();
    }
    incrementIndexes(index1, index2,index3, test.size());
}

void MkTextDocument::insertFormatLinkData(FormatLocation &locTitle, FormatLocation &locLink, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test, QString * linkText)
{
    locLink.end = locLink.start + 2 + linkText->length() ;
    if(locLink.end-locLink.start>3){
        formatData->addFormat(locTitle.start, locTitle.end, QString(LINK_SYMBOL_TITLE_END),linkText);
        formatData->addFormat(locLink.start, locLink.end, QString(LINK_SYMBOL_URL_END), linkText);
        locLink.reset();
        locTitle.reset();
    }
    incrementIndexes(index1, index2,index3, test.size());
}

void MkTextDocument::incrementIndexes(int &index1, int &index2, int &index3, const int size)
{
    index1 += size;
    index2 += size;
    index3 += size;
}

bool MkTextDocument::convertCharacterToSymbol(const QChar &single, QString &text)
{
    if(single =='*' || single == '_' || single == '~'){
        text+=single;
        return true;
    }
    return false;
}

void MkTextDocument::convertCharacterToCheckboxSymbol(const QChar &single, QString &text)
{
    if(single =='-' || single == ' ' || single == '[' || single == ']' || single == 'x'){
        text+=single;
    }
}

bool MkTextDocument::convertCharacterToLinkSymbol(const QChar &single, QString &text)
{
    if(single == '[' || single == ']' ||single == '(' || single == ')' || single == '<' || single == '>'){
        text+=single;
        return true;
    }
    return false;
}

void MkTextDocument::composeSymbolCombination(int length, const QString &text, int &index1, int &index2, int &index3, QString &result)
{
    result.clear();
    if(index3<length){
        convertCharacterToCheckboxSymbol(text[index1], result);
        convertCharacterToCheckboxSymbol(text[index2], result);
        convertCharacterToCheckboxSymbol(text[index3], result);

        if(result == CHECK_SYMBOL_START ||
           result == CHECKED_SYMBOL_END ||
           result == UNCHECKED_SYMBOL_END){
            return;
        }
    }

    result.clear();
    if(index3<length){
        convertCharacterToLinkSymbol(text[index1], result);
        convertCharacterToLinkSymbol(text[index2], result);
        convertCharacterToLinkSymbol(text[index3], result);
        if(result == LINK_SYMBOL_MID ||
            result == LINK_SYMBOL_URL_START ||
            result == LINK_SYMBOL_URL_END ||
            result == ")>)" ){
            if(result == ")>)"){  //edge case
                result = ">)";
            }
            return;
        }
    }
    result.clear();
    if(convertCharacterToLinkSymbol(text[index1], result)){
        return;
    }

    result.clear();
    convertCharacterToSymbol(text[index1],result);
    if(result.isEmpty()){
        return;
    }

    if(index2 >= length){
        return;
    }

    if(!convertCharacterToSymbol(text[index2],result)){
        return;
    }

    if(index3>= length){
        return;
    }
    convertCharacterToSymbol(text[index3],result);

}

void MkTextDocument::composeOnlyLinkSymbolCombination(int length, const QString &text, int &index1, int &index2, int &index3, QString &result)
{
    result.clear();
    if(index3<length){
        convertCharacterToLinkSymbol(text[index1], result);
        convertCharacterToLinkSymbol(text[index2], result);
        convertCharacterToLinkSymbol(text[index3], result);
        if(result == LINK_SYMBOL_MID ||
            result == LINK_SYMBOL_URL_START ||
            result == LINK_SYMBOL_URL_END ||
            result == ")>)" ){
            if(result == ")>)"){  //edge case
                result = ">)";
            }
            return;
        }
    }
    result.clear();
    if(convertCharacterToLinkSymbol(text[index1], result)){
        return;
    }
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

void MkTextDocument::resetTextBlockFormat(QTextBlock block)
{
    QTextCursor cursor(block);
    QTextCharFormat format;
    cursor.setPosition(block.position());
    cursor.setPosition(block.position()+block.length()-1, QTextCursor::KeepAnchor);
    cursor.setCharFormat(format);
}

void MkTextDocument::applyMkFormat(QTextBlock &block, int start, int end, FragmentData::FormatSymbol status, QTextCursor &cursor, FormatCollection &formatCollection, bool onlyCheckboxAndLink)
{
    QTextCharFormat *format = nullptr;

    switch(status){
    case FragmentData::BOLD:{format = formatCollection.getBold();break;}
    case FragmentData::ITALIC:{format = formatCollection.getItalic();break;}
    case FragmentData::STRIKETHROUGH:{format = formatCollection.getStrikethrough();break;}
    case FragmentData::HEADING1:{format = formatCollection.getHeading1(); end = block.length()-1; break;}
    case FragmentData::HEADING2:{format = formatCollection.getHeading2(); end = block.length()-1; break;}
    case FragmentData::HEADING3:{format = formatCollection.getHeading3(); end = block.length()-1; break;}
    case FragmentData::CHECKED_END:
    case FragmentData::UNCHECKED_END:{
        const int blockPos = block.position();
        checkMarkPositions.append(blockPos + start);
        break;
    }
    case FragmentData::LINK_TITLE:{
        format = formatCollection.getLink();
        const int blockPos = block.position();
        linkPositions.append(QPair<int,int>(blockPos + start, blockPos + end));
        break;}

    default:break;
    }

    if(!format || onlyCheckboxAndLink)
        return;

    int startPoint = block.position()+start;
    int endPoint = block.position()+end;
    if(end>=block.length()){
        endPoint = block.length()-1;
    }
    cursor.setPosition(startPoint);
    cursor.setPosition(endPoint, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(*format);
}

void MkTextDocument::hideSymbols(QTextBlock &block,const QString &symbol)
{
    QString textBlock = block.text();
    QRegularExpression regex(symbol);
    textBlock.replace(regex,"");

    QTextCursor editCursor(block);
    editCursor.movePosition(QTextCursor::StartOfBlock);
    editCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    editCursor.insertText(textBlock);
}

void MkTextDocument::hideAllFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData)
{
    QString textBlock = block.text();
    bool isLink = false;
    int linkEnd = 0, linkStart = 0;

    const QString *symbol;
    int symbolPos = -1;
    for(QVector<PositionData*>::Iterator it = formatData->pos_end()-1; it >= formatData->pos_begin(); it--)
    {
        symbol = &(*it)->getSymbol();
        symbolPos = (*it)->getPos();
        hideSymbolsAtPos(textBlock, symbolPos, *symbol);

        if(*symbol == LINK_SYMBOL_URL_END){
            isLink = true;
            linkEnd = symbolPos-2;
        }

        if(*symbol == LINK_SYMBOL_URL_START || isLink){
            linkStart = symbolPos;
            textBlock.remove(linkStart, linkEnd-linkStart);
            isLink = false;
        }
    }

    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
    cursor.insertText(textBlock);
}

void MkTextDocument::hideSymbolsAtPos(QString &text, int pos, const QString &symbol)
{
    text.remove(pos,symbol.length());
    if(symbol == CHECKED_SYMBOL_END){
        text.insert(pos,CHECKED_PIC);
    }else if(symbol == UNCHECKED_SYMBOL_END){
        text.insert(pos,UNCHECKED_PIC);
    }
}

void MkTextDocument::showSymbols(QTextBlock &block, const QString &symbol)
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
    editCursor.insertText(newText);

}

void MkTextDocument::showAllFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData, SelectRange * selectRange)
{
    QString textBlock = block.text();
    const int blockPos = block.position();
     int index = 0;
    for(QString::Iterator cp = textBlock.begin(); cp != textBlock.end(); cp++){
        if(*cp == u'☑' || *cp == u'☐'){
            checkMarkPositions.removeAll(blockPos + index);

            if(selectRange){
                if(index < selectRange->currentposInBlock){
                    selectRange->currentposInBlock--;
                }
            }
        }

        QVector<QPair<int, int>>::iterator it = linkPositions.begin();
        while (it != linkPositions.end()) {
            if (it->first == (blockPos + index)) {
                it = linkPositions.erase(it);
            } else {
                ++it;
            }
        }
        index++;
    }

    for(QVector<PositionData*>::Iterator it = formatData->pos_begin(); it < formatData->pos_end(); it++)
    {
        int cursorPos = (*it)->getPos();
        int symbolLen = (*it)->getSymbol().length();

        if(selectRange){
            if(cursorPos <= selectRange->currentposInBlock){
                selectRange->currentposInBlock = selectRange->currentposInBlock+symbolLen;
            }
        }

        showSymbolsAtPos(textBlock, (*it)->getPos(), (*it)->getSymbol());

        if((*it)->getSymbol() == LINK_SYMBOL_URL_START){
            int pos = (*it)->getPos();

            if(selectRange){
                if(pos< selectRange->currentposInBlock){
                    selectRange->currentposInBlock = selectRange->currentposInBlock+formatData->getLinkUrl(pos).length();
                }
            }

            textBlock.insert(pos+2,formatData->getLinkUrl(pos));
        }
    }
    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
    cursor.insertText(textBlock);
    cursor.movePosition(QTextCursor::StartOfBlock);

    if((selectRange)){
        selectRange->isCursorCaculated = true;
        selectRange->currentBlockNo = block.blockNumber();
    }
}

void MkTextDocument::showSymbolsAtPos(QString &text, int pos, const QString &symbol)
{
    if(symbol == CHECKED_SYMBOL_END ||symbol == UNCHECKED_SYMBOL_END){
        text.remove(pos,1);
    }
    text.insert(pos,symbol);
}

void MkTextDocument::extractSymbolsInBlock(QTextBlock &block, QString &result)
{
    QTextBlockUserData* data =block.userData();
    if(data == nullptr){
        return;
    }

    BlockData* blockData = dynamic_cast<BlockData*>(data);
    if(blockData)
    {
        if(blockData->getStatus()!=BlockData::content)
        {
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
            showFormatSymbolsInTextBlock(block, formatData, result);
        }
    }
}

void MkTextDocument::showFormatSymbolsInTextBlock(QTextBlock &block, FormatData *formatData, QString &result)
{
    QString textBlock = block.text();
    for(QVector<PositionData*>::Iterator it = formatData->pos_begin(); it < formatData->pos_end(); it++)
    {
        showSymbolsAtPos(textBlock, (*it)->getPos(), (*it)->getSymbol());

        if((*it)->getSymbol() == LINK_SYMBOL_URL_START){
            int pos = (*it)->getPos();
            textBlock.insert(pos+2,formatData->getLinkUrl(pos));
        }
    }
    result = textBlock;
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

void MkTextDocument::saveRawDocumentHandler()
{
    this->rawDocument.setPlainText(this->toPlainText());
}

void MkTextDocument::saveSingleRawBlockHandler(int blockNumber)
{
    QTextCursor rawCursor(&this->rawDocument);
    rawCursor.setPosition(this->rawDocument.findBlockByNumber(blockNumber).position());
    rawCursor.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
    rawCursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
    rawCursor.insertText(this->findBlockByNumber(blockNumber).text());
}

void MkTextDocument::hideMKSymbolsFromDrawingRect(int blockNumber, bool showAll,SelectRange * const editSelectRange, const bool clearPushCheckBoxData)
{
    if(disableMarkdownState){
        return;
    }

    int fontSize =this->defaultFont().pointSize();
    FormatCollection formatCollection(fontSize);
    CheckingBlock checkBlock;
    if(clearPushCheckBoxData){
        checkMarkPositions.clear();
        linkPositions.clear();
    }

    for(QTextBlock block = this->begin(); block != this->end(); block = block.next()){

            int currentBlockNumber = block.blockNumber();
            QTextBlockUserData* data =block.userData();
            if(data == nullptr){
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
                        continue;
                    }
                    checkBlock.end = block;
                    setCodeBlockMargin(block,fontSize*3/4, fontSize, 0);

                    if(showAll ||
                        (currentBlockNumber >= selectRange.startBlock && currentBlockNumber <= selectRange.endBlock)||
                      ((blockNumber >= checkBlock.start.blockNumber() && blockNumber <= checkBlock.end.blockNumber()))){
                        showSymbols(checkBlock.start, CODEBLOCK_SYMBOL);
                        showSymbols(checkBlock.end, CODEBLOCK_SYMBOL);
                    }else{
                        hideSymbols(checkBlock.start, CODEBLOCK_SYMBOL);
                        hideSymbols(checkBlock.end, CODEBLOCK_SYMBOL);
                    }

                }else if(blockData->getStatus()==BlockData::content){
                    setCodeBlockMargin(block, fontSize*5/3, fontSize, 0);
                }
            }
            else{
                LineData* lineData = dynamic_cast<LineData*>(data);
                if(lineData){
                    if((blockNumber == currentBlockNumber)&&(selectRange.startBlock == NO_SELECTION_POS)&&(selectRange.endBlock == NO_SELECTION_POS)){
                        lineData->setDraw(false);
                        showSymbols(block, lineData->getSymbol());
                    }else{
                        lineData->setDraw(true);
                        hideSymbols(block, lineData->getSymbol());
                        if(showAll || (currentBlockNumber >= selectRange.startBlock && currentBlockNumber <= selectRange.endBlock)){
                            lineData->setDraw(false);
                            showSymbols(block, lineData->getSymbol());
                        }

                    }
                }
                FormatData* formatData = dynamic_cast<FormatData*>(data);
                if(formatData){
                    if(blockNumber == currentBlockNumber || (currentBlockNumber >= selectRange.startBlock && currentBlockNumber <= selectRange.endBlock && editSelectRange)){
                        if(formatData->isHidden()){
                            formatData->setHidden(false);
                            resetTextBlockFormat(block);
                            showAllFormatSymbolsInTextBlock(block, formatData, editSelectRange);
                        }
                        QTextCursor cursor(block);
                        for(QVector<FragmentData*>::Iterator it = formatData->formats_begin(); it < formatData->formats_end(); it++)
                        {
                            applyMkFormat(block, (*it)->getStart(), (*it)->getEnd(), (*it)->getStatus(), cursor, formatCollection,false);
                        }
                    }
                    else{
                        QTextCursor cursor(block);

                        if(!formatData->isHidden()){
                            formatData->setHidden(true);
                            resetTextBlockFormat(block);
                            hideAllFormatSymbolsInTextBlock(block,formatData);

                            if(!formatData->isHiddenFormatsEmpty()){
                                for(QVector<FragmentData*>::Iterator it = formatData->hiddenFormats_begin(); it < formatData->hiddenFormats_end(); it++)
                                {
                                    applyMkFormat(block, (*it)->getStart(), (*it)->getEnd(), (*it)->getStatus(), cursor, formatCollection, false);
                                }
                            }
                        }else{
                             if(!formatData->isHiddenFormatsEmpty()){
                                for(QVector<FragmentData*>::Iterator it = formatData->hiddenFormats_begin(); it < formatData->hiddenFormats_end(); it++)
                                {
                                    applyMkFormat(block, (*it)->getStart(), (*it)->getEnd(), (*it)->getStatus(), cursor, formatCollection, true);
                                }
                            }
                        }

                        if(showAll || (currentBlockNumber >= selectRange.startBlock && currentBlockNumber <= selectRange.endBlock && editSelectRange)){
                            if(formatData->isHidden()){
                                showAllFormatSymbolsInTextBlock(block, formatData);
                                formatData->setHidden(false);
                            }
                        }
                    }
                }
            }
    }
}

void MkTextDocument::pushCheckBoxHandle(const int position)
{
    QTextCursor cursor(this);
    cursor.setPosition(position);
    cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);

    QTextBlock block = findBlock(position);
    int blockNo = block.blockNumber();
    QTextBlockUserData* data =block.userData();
    FormatData* formatData = dynamic_cast<FormatData*>(data);

    if(!formatData)
        return;

    int linePosition = position - block.position();

    int index1 = 0;
    for(QVector<FragmentData*>::Iterator it = formatData->hiddenFormats_begin(); it < formatData->hiddenFormats_end(); it++)
    {
        if((*it)->getStatus() == FragmentData::CHECKED_END){
                if((*it)->getStart() == linePosition){
                    (*it)->setStatus(FragmentData::UNCHECKED_END);
                    cursor.insertText(UNCHECKED_PIC);
                    break;
                }
                index1++;
        }else if((*it)->getStatus() == FragmentData::UNCHECKED_END){

                if((*it)->getStart() == linePosition){
                    (*it)->setStatus(FragmentData::CHECKED_END);
                    cursor.insertText(CHECKED_PIC);
                    break;
                }
                index1++;
        }
    }

    int index2 = 0;

    for(QVector<PositionData*>::Iterator it = formatData->pos_begin(); it != formatData->pos_end(); ++it)
    {
        if((*it)->getSymbol()== CHECKED_SYMBOL_END){
                if(index2 == index1){
                    (*it)->setSymbol(UNCHECKED_SYMBOL_END);
                }
                index2++;
        }else if((*it)->getSymbol()== UNCHECKED_SYMBOL_END){
                if(index2 == index1){
                    (*it)->setSymbol(CHECKED_SYMBOL_END);
                }
                index2++;
        }
    }

    //update the pushing checking actions on the raw document as well
    QTextBlock rawBlock(block);
    QString rawString;
    extractSymbolsInBlock(rawBlock, rawString);

    QTextCursor cursorRaw(&rawDocument);
    cursorRaw.setPosition(rawDocument.findBlockByLineNumber(blockNo).position());
    cursorRaw.movePosition(QTextCursor::StartOfBlock);
    cursorRaw.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursorRaw.insertText(rawString);
}

void MkTextDocument::pushLinkHandle(const int position)
{
    QTextBlock block = findBlock(position);
    QTextBlockUserData* data =block.userData();
    FormatData* formatData = dynamic_cast<FormatData*>(data);

    if(!formatData)
        return;

    for(QVector<FragmentData*>::Iterator it = formatData->hiddenFormats_begin(); it < formatData->hiddenFormats_end(); it++)
    {
        if((*it)->getStatus() == FragmentData::LINK_TITLE){
            const QString *link =formatData->getHiddenLinkUrl((*it)->getStart());
            if(link){
                QDesktopServices::openUrl(QUrl(*link));
            }
        }
    }
}

void MkTextDocument::autoInsertSymobolHandle(const int position)
{
    QTextCursor cursor(this);
    cursor.setPosition(position);
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);

    QString text = cursor.selectedText().toLower();
    QString checkedSymbol = QString(CHECK_SYMBOL_START) + QString(CHECKED_SYMBOL_END) + " ";
    QString unCheckSymbol = QString(CHECK_SYMBOL_START) + QString(UNCHECKED_SYMBOL_END) + " ";
    if(text == "check" || text == "ch"){
        cursor.insertText(checkedSymbol);
    }else if(text == "uncheck" || text == "uch"){
        cursor.insertText(unCheckSymbol);
    }else if(text == "link" || text == "lk"){
        cursor.insertText(QString(LINK_SYMBOL_TITLE_START)+QString(LINK_SYMBOL_MID) + QString(LINK_SYMBOL_URL_END));
    }else{
        cursor.setPosition(position);
        cursor.insertText("    ");
    }
}

void MkTextDocument::setMarkdownHandle(bool state, QRect rect)
{
    disableMarkdownState = !state;

    if(disableMarkdownState){
        this->setPlainText(this->rawDocument.toPlainText());
    }else{
        hideMKSymbolsFromDrawingRect(-1,false,nullptr,false);
    }
}

void MkTextDocument::cursorUpdateHandle(const int blockNo, const int characterNo)
{
    this->blockNo = blockNo;
    this->characterNo = characterNo;
}

void MkTextDocument::undoStackPush(QUndoCommand *edit)
{
    undoStack.push(edit);
}

void MkTextDocument::undoStackUndo()
{
    undoStack.undo();
}

void MkTextDocument::undoStackRedo()
{
    undoStack.redo();
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
    locLink.reset();
    locLinkTitle.reset();
}

void MkTextDocument::numberListDetect(int blockNumber)
{
    QTextCursor editCursor(this->findBlockByNumber(blockNumber));
    QTextBlock currentBlock = this->findBlockByNumber(blockNumber-1);
    QString lineText = currentBlock.text();

    regexNumbering.setPattern("^( {0,}|\\s{2,})\\d+\\.\\s*.*$");                    //look for " 10. abc123 abc123", "   1. abc123 abc"," 10. abc123 abc123","   1. abc123 abc","10. abc123 abc123","1. abc123 abc","10. abc123","1. abc123"," 1. ",
    QRegularExpressionMatch matchNumbering = regexNumbering.match(lineText);

    regexBulletPoints.setPattern("^\\s*-\\s*") ;
    QRegularExpressionMatch matchBulletPoint = regexBulletPoints.match(lineText);

    QRegularExpressionMatch matchCheckBox = regexCheckBox.match(lineText);
    QRegularExpressionMatch matchBulletCheckBox = regexBulletCheckBox.match(lineText);

    if(matchNumbering.hasMatch()){
        int spaces = numberListGetSpaces(matchNumbering.captured(0));
        editCursor.insertText(QString("").leftJustified(spaces,' '));
        editCursor.insertText(numberListGetNextNumber(matchNumbering.captured(0)));
        return;
    }else if(matchBulletCheckBox.hasMatch()){
        int spaces = numberListGetSpaces(matchBulletPoint.captured(0));
        editCursor.insertText(QString("").leftJustified(spaces,' '));
        if(matchCheckBox.captured(0).contains("x")){
            editCursor.insertText("- - [x]  ");
        }else{
            editCursor.insertText("- - [ ]  ");
        }
        return;
    }else if(matchCheckBox.hasMatch()){
        int spaces = numberListGetSpaces(matchBulletPoint.captured(0));
        editCursor.insertText(QString("").leftJustified(spaces,' '));
        if(matchCheckBox.captured(0).contains("x")){
            editCursor.insertText("- [x]  ");
        }else{
            editCursor.insertText("- [ ]  ");
        }
        return;
    }else if(matchBulletPoint.hasMatch()){
        int spaces = numberListGetSpaces(matchBulletPoint.captured(0));
        editCursor.insertText(QString("").leftJustified(spaces,' '));
        editCursor.insertText("- ");
        return;
    }
    else{
        QTextCharFormat format;
        format.setFontPointSize(this->defaultFont().pointSize());
        editCursor.movePosition(QTextCursor::EndOfBlock);
        editCursor.movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor);
        editCursor.setCharFormat(format);
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

EditCommand::EditCommand(UndoData &data)
{
    this->view = data.view;
    this->doc = dynamic_cast<MkTextDocument*>(data.doc);
    this->scrollValue = data.scrollValue;
    isConstructorRedo = true;
    this->oldSelectRange = data.oldSelectRange;
    this->blockNo = data.blockNo;
    this->posInBlock = data.posInBlock;
    this->oldSelectRange.isCheckBox = data.isCheckBox;
    this->oldSelectRange.scrollValue = data.scrollValue;
    this->oldBlock = data.oldBlock;

    if(data.isSingle){
        this->undoType = singleBlockEdit;
        this->newBlock = data.newBlock;
        this->oldBlock = data.oldBlock;
    }else{
        this->undoType = multiBlockEdit;
        this->oldText = data.oldText;
        this->text = data.text;
    }
}

void EditCommand::undo()
{
    if(undoType == singleBlockEdit){
        doc->setUndoRedoText(oldSelectRange.currentBlockNo, this->oldBlock);
    }else{
        doc->setUndoRedoText(oldText);
    }
    doc->setUndoSelectRange(this->oldSelectRange);
}

void EditCommand::redo()
{
    if(isConstructorRedo){
        isConstructorRedo = false;
    }else{

        if(undoType == singleBlockEdit){
            doc->setUndoRedoText(this->blockNo, this->newBlock);
        }else{
            doc->setUndoRedoText(text);
        }

        QTextCursor cursor = this->view->textCursor();
        cursor.setPosition(this->view->document()->findBlockByNumber(this->blockNo).position());
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, this->posInBlock);

        this->view->setTextCursor(cursor);
        doc->setRedoSelectRange(this->blockNo, this->posInBlock,  this->oldSelectRange.isCheckBox, this->oldSelectRange.scrollValue );
    }
}
