#include "mktextdocument.h"

MkTextDocument::MkTextDocument(QObject *parent)
    : QTextDocument{parent}
{
    regexCodeBlock.setPattern(CODEBLOCK_SYMBOL);
    regexHorizontalLine.setPattern(HORIZONTALLINE_SYMBOL);

    this->setUndoRedoEnabled(false);
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
        }
        tblock = tblock.next();
    }
}

void MkTextDocument::applyAllMkDataHandle(bool hasSelection, int blockNumber, bool showAll)
{
    identifyUserData(showAll);
    cursorPosChangedHandle(hasSelection, blockNumber);
}

void MkTextDocument::identifyUserData(bool showAll)
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
                setCodeBlockMargin(tBlock, blockFormat, this->defaultFont().pointSize()*3/4,this->defaultFont().pointSize());
                startBlock = tBlock;
            }
            else{
                openBlock = false;
                blockData->setStatus(BlockData::end);
                setCodeBlockMargin(tBlock, blockFormat, this->defaultFont().pointSize()*3/4);
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
                setCodeBlockMargin(tBlock, blockFormat, this->defaultFont().pointSize()*5/3);
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
            }
        }
    }
}

void MkTextDocument::setCodeBlockMargin(QTextBlock &block, QTextBlockFormat &blockFormat, int leftMargin, int topMargin)
{
    QTextCursor cursor(block);
    blockFormat = cursor.blockFormat();
    blockFormat.setLeftMargin(leftMargin);
    if(topMargin != 0) blockFormat.setTopMargin(topMargin);
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

void MkTextDocument::duplicateLine(int blockNumber)
{
    QTextBlock currentBlock = this->findBlockByNumber(blockNumber);
    QTextCursor editCursor(currentBlock);
    QString lineText = currentBlock.text();
    editCursor.movePosition(QTextCursor::EndOfBlock);
    editCursor.insertBlock();
    editCursor.insertText(lineText);
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
