#include "mktextdocument.h"

MkTextDocument::MkTextDocument(QObject *parent)
    : QTextDocument{parent}
{
    regexCodeBlock.setPattern("```");
    regexHorizontalLine.setPattern("---");
}

void MkTextDocument::setPlainText(const QString &text)
{
    if(text.isEmpty())
        return;

    QTextDocument::setPlainText(text);
    identifyUserData();
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
                    showSymbols(this->findBlockByNumber(checkBlock.start), "```");
                    showSymbols(this->findBlockByNumber(checkBlock.end), "```");
                }
                else{
                    if(!hasSelection){
                        hideSymbols(this->findBlockByNumber(checkBlock.start),"```");
                        hideSymbols(this->findBlockByNumber(checkBlock.end),"```");
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
    showAllCodeBlocksHandle();
    stripUserData();
}

void MkTextDocument::showAllCodeBlocksHandle()
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
                showSymbols(tblock, "```");
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

void MkTextDocument::applyAllMkDataHandle(bool hasSelection, int blockNumber)
{
    identifyUserData();
    cursorPosChangedHandle(hasSelection, blockNumber);
}

void MkTextDocument::hideAllCodeBlocksHandle(bool hasSelection, int blockNumber)
{
    cursorPosChangedHandle(hasSelection, blockNumber);
}

void MkTextDocument::identifyUserData()
{
    bool openBlock = false;
    QTextBlock startBlock;
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
                hideSymbols(tBlock,"```");
                hideSymbols(startBlock,"```");
            }
        }
        else{
            if(openBlock){
                BlockData *blockData = new BlockData;
                blockData->setStatus(BlockData::content);
                tBlock.setUserData(blockData);
            }else{
                QRegularExpressionMatch matchHorizontalLine = regexHorizontalLine.match(tBlock.text());
                if(matchHorizontalLine.hasMatch()){
                    LineData *lineData = new LineData;
                    lineData->setStatus(LineData::horizontalLine);
                    tBlock.setUserData(lineData);
                    hideSymbols(tBlock,lineData->getSymbol());
                }
            }
        }
    }
}

void MkTextDocument::stripUserData()
{
    bool openBlock = false;
    for(QTextBlock tBlock = this->begin(); tBlock != this->end(); tBlock = tBlock.next()){
        tBlock.setUserData(nullptr);
    }
}

void MkTextDocument::showCursoredBlock(int blockNumber, int start, int end, const QString &symbol)
{
    if(start > end ){
        return;
    }

    if(blockNumber >= start && blockNumber <= end){
            showSymbols(this->findBlock(start), symbol);
            showSymbols(this->findBlock(end), symbol);
    }
    else{
        QTextBlock startBlock = this->findBlock(start);
        QTextBlock endBlock = this->findBlock(end);
            hideSymbols(startBlock,symbol);
            hideSymbols(endBlock,symbol);
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
            editCursor.insertText("```");
            editCursor.insertBlock();
            editCursor.insertText("```");
            success = true;
    }else{
        success = false;
    }
}

void MkTextDocument::enterKeyPressedHandle(int blockNumber)
{
    numberListDetect(blockNumber);
}

void MkTextDocument::quoteLeftKeyPressedHandle(int blockNumber,bool &success)
{
    autoCompleteCodeBlock(blockNumber,success);
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

void MkTextDocument::isUserDataStillValidHandle(int blockNumber)
{
    QTextBlock tblock = this->findBlockByNumber(blockNumber);       qDebug()<<"- "<<tblock.text();
    QTextBlockUserData* data =tblock.userData();
    BlockData* blockData = dynamic_cast<BlockData*>(data);
    if(blockData){
        if(blockData->getStatus()==BlockData::start){

        }

    }else{
        LineData* lineData = dynamic_cast<LineData*>(data);
        if(lineData){
            if(lineData->getStatus()== LineData::horizontalLine){
                QRegularExpressionMatch matchLine = regexHorizontalLine.match(tblock.text());
                if (!matchLine.hasMatch()){
                    qDebug()<<"- "<<lineData->getSymbol();
                    tblock.setUserData(nullptr);
                }
            }
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
