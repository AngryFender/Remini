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
    QQueue<QTextBlock> queBlocks;                                                           //list of all blocks inside the codeblock
    for(QTextBlock tBlock = this->begin(); tBlock != this->end(); tBlock = tBlock.next()){

        QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(tBlock.text());
        if(matchCodeBlock.hasMatch()){
            tBlock.setUserData(&blockData);
            queBlocks.append(tBlock);

            if(!openBlock){
                openBlock = true;
                blockData.setStart(tBlock.blockNumber());
            }
            else{
                openBlock = false;
                blockData.setFinish(tBlock.blockNumber());
            }

            hideSymbols(tBlock,"```");
            qDebug()<<"Block number = "<< tBlock.blockNumber() << tBlock.text();
        }
        else{
            if(openBlock){
                tBlock.setUserData(&blockData);
                queBlocks.append(tBlock);
            }
        }
    }
}

void MkTextDocument::updateMkGui( QTextDocument *doc, int blockNumber)
{
    qDebug()<<"blocknumber = "<<blockNumber;
   if(doc->isEmpty())
        return;


   QTextBlock block = this->findBlockByLineNumber(blockNumber);
   QTextBlockUserData* data =block.userData();

   BlockData* blockData = static_cast<BlockData*>(data);

//   qDebug()<<blockData->getStart()<<" start and finish "<< blockData->getFinish();
//   blockData.
   if(blockData){

        QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(block.text());
        if(matchCodeBlock.hasMatch()){
            hideSymbols( block, "```");
        }
        else
        {
            showSymbols( block, "```");
        }
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

    editCursor.insertText(textBlock);
}

void MkTextDocument::showSymbols(QTextBlock block, const QString &symbol)
{
    QString newText= block.text().prepend(symbol);
    QTextCursor editCursor(block);
    editCursor.movePosition(QTextCursor::StartOfBlock);
    editCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    editCursor.removeSelectedText();

    editCursor.insertText(newText);
}



//if(doc->isEmpty())
//    return;

//QTextCursor cursor(doc);
//for(QTextBlock it = this->begin(); it != this->end(); it = it.next()){
//    QString textBlock = it.text();
//    QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(it.text());
//    if(matchCodeBlock.hasMatch()){
//        for(const auto &par: codeBlockPosList){
//            if(par.contains(blockNumber)){

//                for(int line = par.firstPage(); line <=par.lastPage();++line){
//                    QTextBlock selectText = doc->findBlockByLineNumber(blockNumber);
//                    QTextCursor selectCursor(selectText);
//                    cursor.movePosition(QTextCursor::StartOfBlock);
//                    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
//                    cursor.removeSelectedText();
//                    cursor.insertText(textBlock);
//                }
//                break;
//            }
//        }
//    }

//}

