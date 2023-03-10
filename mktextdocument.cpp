#include "mktextdocument.h"

MkTextDocument::MkTextDocument(QObject *parent)
    : QTextDocument{parent}
{
    regexCodeBlock.setPattern(CODEBLOCK_SYMBOL);
    regexHorizontalLine.setPattern(HORIZONTALLINE_SYMBOL);
    regexBoldA.setPattern("(?<!\\*)\\*\\*(?!\\*{1,2})");
    regexBoldU.setPattern(BOLD_SYMBOL_U);

    this->setUndoRedoEnabled(false);

    boldALoc.symbol = BOLD_SYMBOL_A;
    boldULoc.symbol = BOLD_SYMBOL_U;
    italicALoc.symbol = ITALIC_SYMBOL_A;
    italicULoc.symbol = ITALIC_SYMBOL_U;

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

void MkTextDocument::cursorPosChangedHandle( bool hasSelection, int blockNumber)
{
    if(this->isEmpty())
        return;

    QTextBlock tblock = this->begin();
    CheckBlock checkBlock;

    while (tblock.isValid()) {
        QTextBlockUserData* data =tblock.userData();
        BlockData* blockData = dynamic_cast<BlockData*>(data);
        if(blockData)
        {
            if(blockData->getStatus()==BlockData::start)
            {
                checkBlock.start = tblock.blockNumber();
            }
            else if(blockData->getStatus()==BlockData::end)
            {
                checkBlock.end = tblock.blockNumber();
                if(blockNumber >= checkBlock.start && blockNumber <= checkBlock.end){
                    showSymbols(this->findBlockByNumber(checkBlock.start), CODEBLOCK_SYMBOL);
                    showSymbols(this->findBlockByNumber(checkBlock.end), CODEBLOCK_SYMBOL);
                }
                else{
                    if(!hasSelection){
                        hideSymbols(this->findBlockByNumber(checkBlock.start), CODEBLOCK_SYMBOL);
                        hideSymbols(this->findBlockByNumber(checkBlock.end), CODEBLOCK_SYMBOL);
                    }
                }
            }
        }
        else{
            LineData* lineData = dynamic_cast<LineData*>(data);
            if(lineData){
                if(blockNumber == tblock.blockNumber()){
                    lineData->setDraw(false);
                    showSymbols(tblock, lineData->getSymbol());
                }else{
                    if(!hasSelection){
                        lineData->setDraw(true);
                        hideSymbols(tblock, lineData->getSymbol());
                    }
                }
            }
            FormatData* formatData = dynamic_cast<FormatData*>(data);
            if(formatData){
                if(blockNumber == tblock.blockNumber()){
                    if(formatData->isHidden()){
                        for(QVector<PositionData*>::Iterator it = formatData->pos_begin(); it < formatData->pos_end(); it++)
                        {
                            showSymbolsAtPos(tblock, (*it)->getPos(), (*it)->getSymbol());
                        }
                        formatData->setHidden(false);
                    }
                }
                else{
                    if(!hasSelection){
                        if(!formatData->isHidden()){
                            //hide symbols, start at the end to avoid changing position of the symbols
                            for(QVector<PositionData*>::Iterator it = formatData->pos_end()-1; it >= formatData->pos_begin(); it--)
                            {
                                hideSymbolsAtPos(tblock, (*it)->getPos(), (*it)->getSymbol());
                            }
                            formatData->setHidden(true);
                        }
                    }
                }
            }
        }
        tblock = tblock.next();
    }
}

void MkTextDocument::removeAllMkDataHandle()
{
    showAllSymbols();
    stripUserData();
}

void MkTextDocument::showAllSymbols()
{
    QTextBlock tblock = this->begin();

    //show all ```
    while (tblock.isValid()) {
        QTextBlockUserData* data =tblock.userData();
        BlockData* blockData = dynamic_cast<BlockData*>(data);
        if(blockData)
        {
            if(blockData->getStatus()==BlockData::start || blockData->getStatus()==BlockData::end)
            {
                showSymbols(tblock, CODEBLOCK_SYMBOL);
            }
        }else{
            LineData* lineData = dynamic_cast<LineData*>(data);
            if(lineData){
                showSymbols(tblock, lineData->getSymbol());
            }

            FormatData* formatData = dynamic_cast<FormatData*>(data);
            if(formatData){
                if(formatData->isHidden()){
                    for(QVector<PositionData*>::Iterator it = formatData->pos_begin(); it < formatData->pos_end(); it++)
                    {
                        showSymbolsAtPos(tblock, (*it)->getPos(), (*it)->getSymbol());
                    }
                    formatData->setHidden(false);
                }
            }
        }
        tblock = tblock.next();
    }
}

void MkTextDocument::applyAllMkDataHandle(bool hasSelection, int blockNumber, bool showAll)
{
    identifyUserData(showAll, blockNumber);
    cursorPosChangedHandle(hasSelection, blockNumber);
}

void MkTextDocument::identifyUserData(bool showAll, int blockNumber)
{
    int fontSize =this->defaultFont().pointSize();
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
                setCodeBlockMargin(tBlock, blockFormat, fontSize*3/4, fontSize, fontSize);
                startBlock = tBlock;
            }
            else{
                openBlock = false;
                blockData->setStatus(BlockData::end);
                setCodeBlockMargin(tBlock, blockFormat, fontSize*3/4, fontSize);
                if(!showAll){
                    hideSymbols(tBlock, CODEBLOCK_SYMBOL);
                    hideSymbols(startBlock, CODEBLOCK_SYMBOL);
                }
            }
        }
        else{
            if(openBlock){
                BlockData *blockData = new BlockData;
                blockData->setStatus(BlockData::content);
                tBlock.setUserData(blockData);
                setCodeBlockMargin(tBlock, blockFormat, fontSize*5/3, fontSize);
            }else{
                QRegularExpressionMatch matchHorizontalLine = regexHorizontalLine.match(tBlock.text());
                if(matchHorizontalLine.hasMatch()){
                    LineData *lineData = new LineData;
                    lineData->setStatus(LineData::horizontalLine);
                    tBlock.setUserData(lineData);
                    if(!showAll){
                        hideSymbols(tBlock,lineData->getSymbol());
                    }
                }
                identifyFormatData(tBlock, showAll, blockNumber);
            }
        }
    }
}

void MkTextDocument::identifyFormatData(QTextBlock &block, bool showAll,int blockNumber)
{
    QRegularExpressionMatch matchBoldA = regexBoldA.match(block.text());
    QRegularExpressionMatch matchBoldU = regexBoldU.match(block.text());
    if(matchBoldA.hasMatch()||matchBoldU.hasMatch()){
        FormatData *formatData = new FormatData;
        QString text = block.text();
        int index1 = 0;
        int index2 = index1+1;

        resetFormatLocation();
        while(index2<text.length()){
            QString testSym=  QString(text.at(index1)) + QString(text.at(index2));
            identifyFormatLocation(testSym, index1, boldALoc, formatData);
            identifyFormatLocation(testSym, index1, boldULoc, formatData);
            index1++;
            index2++;
        }
        if(!formatData->isEmpty()){
            block.setUserData(formatData);
            for(QVector<FragmentData*>::Iterator it = formatData->formats_begin(); it < formatData->formats_end(); it++)
            {
                applyMkFormat(block, (*it)->getStart(), (*it)->getEnd(), (*it)->getStatus());
            }
            if(!showAll){
                if(blockNumber != block.blockNumber() && !formatData->isHidden()){
                    //hide symbols, start at the end to avoid changing position of the symbols
                    for(QVector<PositionData*>::Iterator it = formatData->pos_end()-1; it >= formatData->pos_begin(); it--)
                    {
                        hideSymbolsAtPos(block, (*it)->getPos(), (*it)->getSymbol());
                    }
                    formatData->setHidden(true);
                }
            }
        }
    }
}

void MkTextDocument::identifyFormatLocation(QString &text, int index, FormatLocation &location, FormatData *format)
{
    if(location.symbol == text){
        if(location.start == -1){
            location.start = index;
        }else{
            location.end = index;
            if(location.end-location.start>2){
                format->addFormat(location.start, location.end, location.symbol);
                location.reset();
            }
        }
    }
}

void MkTextDocument::setCodeBlockMargin(QTextBlock &block, QTextBlockFormat &blockFormat, int leftMargin,int rightMargin, int topMargin)
{
    QTextCursor cursor(block);
    blockFormat = cursor.blockFormat();
    blockFormat.setLeftMargin(leftMargin);
    if(topMargin != 0) blockFormat.setTopMargin(topMargin);
    blockFormat.setRightMargin(rightMargin);
    cursor.mergeBlockFormat(blockFormat);
}

void MkTextDocument::stripUserData()
{
    bool openBlock = false;
    for(QTextBlock tBlock = this->begin(); tBlock != this->end(); tBlock = tBlock.next()){
        tBlock.setUserData(nullptr);
        QTextCursor tcursor(tBlock);
        QTextBlockFormat blockFormat ;
        tcursor.setBlockFormat(blockFormat);
    }
}

void MkTextDocument::applyMkFormat(QTextBlock &block, int start, int end, FragmentData::FormatSymbol status)
{
    QTextCharFormat format;
    format.setFontWeight(QFont::ExtraBold);

    QTextCursor cursor(block);
    cursor.setPosition(block.position()+start);
    cursor.setPosition(block.position()+end, QTextCursor::KeepAnchor);
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

void MkTextDocument::hideSymbolsAtPos(QTextBlock &block, int pos, const QString &symbol)
{
    QString text = block.text();
    if((pos+1)>=text.length())
        return;

    QTextCursor cursor(block);
    cursor.setPosition(block.position()+pos);

    QString testSym = QString(text.at(pos)) + QString(text.at(pos+1));
    //check if the position really has those symbols, they may be already hidden
    if(testSym == symbol){
        for(int i = 0; i < symbol.length(); i++){
            cursor.deleteChar();
        }
    }
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

void MkTextDocument::showSymbolsAtPos(QTextBlock &block, int pos, const QString &symbol)
{
    QTextCursor cursor(block);
    cursor.setPosition(block.position()+pos);
    cursor.insertText(symbol);
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

void MkTextDocument::resetFormatLocation()
{
    boldALoc.reset();
    boldULoc.reset();
    italicALoc.reset();
    italicULoc.reset();
}

void MkTextDocument::numberListDetect(int blockNumber)
{
    QTextCursor editCursor(this->findBlockByNumber(blockNumber));
    QTextBlock currentBlock = this->findBlockByNumber(blockNumber-1);
    QString lineText = currentBlock.text();

    QStringList patterns = {"\\s+[0-9]+\\.\\s([A-Za-z0-9]+( [A-Za-z0-9]+)+)",   //look for " 10. abc123 abc123" , "   1. abc123 abc"
                                "[0-9]+\\.\\s([A-Za-z0-9]+( [A-Za-z0-9]+)+)",   //look for " 10. abc123 abc123" , "   1. abc123 abc"
                            "\\s+[0-9]+\\.\\s[A-Za-z0-9]+",                     //look for "10. abc123 abc123"  , "1. abc123 abc"
                                "[0-9]+\\.\\s[A-Za-z0-9]+",                     //look for "10. abc123"         , "1. abc123"
                            "\\s+[0-9]+\\.\\s+",                                //look for " 1. "
                                "[0-9]+\\.\\s+"};                               //look for "1. "
    for(QString &pattern :patterns){
        regexNumbering.setPattern(pattern);
        QRegularExpressionMatch matchNumbering = regexNumbering.match(lineText);
        if(matchNumbering.hasMatch()){
            int spaces = numberListGetSpaces(matchNumbering.captured(0));
            editCursor.insertText(QString("").leftJustified(spaces,' '));
            editCursor.insertText(numberListGetNextNumber(matchNumbering.captured(0)));
            return;
        }
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
