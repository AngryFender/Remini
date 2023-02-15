#include "mktextdocument.h"

MkTextDocument::MkTextDocument(QObject *parent)
    : QTextDocument{parent}
{
    regexCodeBlock.setPattern("```");
    regexHorizontalLine.setPattern("---");
//    regexHorizontalLine.setPattern("/---\\s*.*[^\\s]/");
}

void MkTextDocument::setPlainText(const QString &text)
{
    QTextDocument::setPlainText(text);

    if(text.isEmpty())
        return;

    bool openBlock = false;
    int startNumber= 0;

    for(QTextBlock tBlock = this->begin(); tBlock != this->end(); tBlock = tBlock.next()){

        QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(tBlock.text());
        if(matchCodeBlock.hasMatch()){
            BlockData *blockData = new BlockData;
            tBlock.setUserData(blockData);

            if(!openBlock){
                openBlock = true;
                blockData->setStatus(BlockData::start);
                qDebug()<<"Start Block number = "<< tBlock.blockNumber() << tBlock.text();
            }
            else{
                openBlock = false;
                blockData->setStatus(BlockData::end);
                qDebug()<<"End Block number = "<< tBlock.blockNumber() << tBlock.text();
            }
            hideSymbols(tBlock,"```");

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

QString MkTextDocument::toPlainText()
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
        }
        else
        {
            LineData* lineData = dynamic_cast<LineData*>(data);
            if(lineData){
                showSymbols(tblock, lineData->getSymbol());
            }
        }
        tblock = tblock.next();
    }

    QString contents = QTextDocument::toPlainText();
    return contents;
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
                    showSymbols(tblock, lineData->getSymbol());
                }else{
                    if(!hasSelection){
                        hideSymbols(tblock, lineData->getSymbol());
                    }
                }
            }
        }
        tblock = tblock.next();
    }
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

void MkTextDocument::hideAllCodeBlocksHandle(bool hasSelection, int blockNumber)
{
    cursorPosChangedHandle(hasSelection, blockNumber);
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
