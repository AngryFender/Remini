#include "mktextdocument.h"

MkTextDocument::MkTextDocument(QObject *parent)
    : QTextDocument{parent}
{
    regexCodeBlock.setPattern("```");
//    QObject::connect(this,SIGNAL(textChanged()),
//                     this, SLOT(fileSave()));

}

void MkTextDocument::setPlainText(const QString &text)
{
    QTextDocument::setPlainText(text);

    if(text.isEmpty())
        return;

    bool openBlock = false;

    int startNumber= 0;
    CodeBox * codebox = nullptr;
    for(QTextBlock tBlock = this->begin(); tBlock != this->end(); tBlock = tBlock.next()){

        QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(tBlock.text());
        if(matchCodeBlock.hasMatch()){
            tBlock.setUserData(&blockData);

            if(!openBlock){
                openBlock = true;
                codebox = blockData.addCodeBoxBack();
                codebox->setStart(tBlock.blockNumber());
            }
            else{
                openBlock = false;
                codebox->setFinish(tBlock.blockNumber());
            }

            hideSymbols(tBlock,"```");
            qDebug()<<"Block number = "<< tBlock.blockNumber() << tBlock.text();
        }
        else{
            if(openBlock){
                tBlock.setUserData(&blockData);
            }
        }
    }
}

void MkTextDocument::updateMkGui( QTextDocument *doc, int blockNumber)
{
    qDebug()<<"blocknumber = "<<blockNumber;
    if(doc->isEmpty())
        return;
    QTextBlock tblock = this->begin();
    CodeBox *box;
    int codeId = 1;
    while (tblock.isValid()) {
        QTextBlockUserData* data =tblock.userData();
        BlockData* blockData = dynamic_cast<BlockData*>(data);
        if(blockData){
            box = blockData->getCodeBox(codeId);
            if(blockNumber >= box->getStart() && (blockNumber <= box->getFinish())){
                showSymbols( box, tblock, "```");
            }else{
                hideSymbols( tblock, "```");
            }
            if(box->getFinish()==tblock.blockNumber()){
                codeId++;
            }
        }
        tblock = tblock.next();
    }
}

void MkTextDocument::KeyEnterPressedHandle(int blockNumber)
{
    qDebug()<<"Enter Key Pressed "<<blockNumber;
    QTextBlock tblock = this->begin();
    while (tblock.isValid()) {
        QTextBlockUserData* data =tblock.userData();
        BlockData* blockData = dynamic_cast<BlockData*>(data);
        hideSymbols( tblock, "```");
        tblock = tblock.next();
    }
}


void MkTextDocument::hideSymbols(QTextBlock block,const QString &symbol)
{
    QString textBlock = block.text();
    textBlock.replace(regexCodeBlock,"");

    QTextCursor editCursor(block);
    editCursor.movePosition(QTextCursor::StartOfBlock);
    editCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    editCursor.removeSelectedText();

    qDebug()<<"block text = " << block.text() <<" new text = "<< textBlock;

    editCursor.insertText(textBlock);
}

void MkTextDocument::showSymbols(CodeBox* blockData, QTextBlock block, const QString &symbol)
{
    //avoid prepending more than 3 ```
    QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(block.text());
    if(matchCodeBlock.hasMatch()){
        return;
    }

    //only prepending the start and end of the codeblocks
    QTextCursor editCursor(block);
    if(blockData->getStart() == editCursor.blockNumber() ||
       blockData->getFinish() == editCursor.blockNumber())
    {
        QString newText= block.text().prepend(symbol);
        editCursor.movePosition(QTextCursor::StartOfBlock);
        editCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        editCursor.removeSelectedText();
        editCursor.insertText(newText);
    }
}
