#include "blockdata.h"
#include "linedata.h"
#include "mkedit.h"


void MkEdit::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::NonCosmeticBrushPatterns);

    QTextBlock block = this->document()->begin();
    bool drawBlock = false;
    int xBlock =0, yBlock =0;

    int codeId = 1;
    while (block.isValid()) {
        QTextBlockUserData* data =block.userData();
        BlockData* blockData = dynamic_cast<BlockData*>(data);
        if(blockData){
            if(blockData->getStatus()==BlockData::start){
                xBlock = block.layout()->position().x()-2;
                yBlock = block.layout()->position().y();
                positionStartBlock = block.blockNumber();
            }

            if(blockData->getStatus()==BlockData::end){
                positionEndBlock = block.blockNumber();
                int height = block.layout()->position().y() - yBlock + TEXT_SIZE;
                painter.setPen(penCodeBlock);
                painter.drawRoundedRect(xBlock,yBlock,widthCodeBlock,height,BLOCKRADIUS,BLOCKRADIUS);
                painter.drawLine(xBlock,yBlock+TEXT_SIZE+7,widthCodeBlock,yBlock+TEXT_SIZE+7);
                codeId++;
            }
        }else{
            LineData* lineData = dynamic_cast<LineData*>(data);
            if(lineData){
                if(lineData->getStatus() == LineData::horizontalLine){
                    int lineX1 = block.layout()->position().x()-2;
                    int lineY1 = block.layout()->position().y()+(TEXT_SIZE*4/3);
                    int lineX2 = block.layout()->position().x()-2+widthCodeBlock;
                    painter.setPen(penCodeBlock);
                    painter.drawLine(lineX1,lineY1,lineX2,lineY1);
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
    widthCodeBlock = this->width()-15-PADDING;
    heightCodeBlock = this->height();
}

QString MkEdit::toPlainText()
{
    int cursorPosition = this->textCursor().position();

    emit showAllCodeBlocks();
    QString content = QTextEdit::toPlainText();

    emit hideAllCodeBlocks( textCursor().hasSelection(), textCursor().blockNumber());
    this->textCursor().setPosition(cursorPosition);

    return content;
}

void MkEdit::keyPressEvent(QKeyEvent *event)
{
    QTextEdit::keyPressEvent(event);
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        numberListDetect();
    }
    emit contentChanged();
}

void MkEdit::numberListDetect()
{
    QTextCursor cursor = textCursor();
    int currentLineNumber = cursor.blockNumber();
    QTextBlock currentLine = this->document()->findBlockByNumber(currentLineNumber-1);
    QString lineText = currentLine.text();

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
            cursor.insertText(QString("").leftJustified(spaces,' '));
            cursor.insertText(numberListGetNextNumber(matchNumbering.captured(0)));
            return;
        }
    }
}

int MkEdit::numberListGetSpaces(const QString &text)
{
    regexNumbering.setPattern("^\\s+"); //look for spaces
    QRegularExpressionMatch matchSpace = regexNumbering.match(text);
    if (matchSpace.hasMatch()){
        return matchSpace.captured(0).length();
    }
    return 0;
}

QString MkEdit::numberListGetNextNumber(const QString &text)
{
    regexNumbering.setPattern("[0-9]+"); //look for numbers
    QRegularExpressionMatch matchSpace = regexNumbering.match(text);
    if (matchSpace.hasMatch()){
        int number =  matchSpace.captured(0).toInt();
        return QString::number(number+1)+". ";
    }
    return "";
}

void MkEdit::codeBockDetect()
{
    QTextCursor cursor = textCursor();
    int currentLineNumber = cursor.blockNumber();
    QTextBlock currentLine = this->document()->findBlockByNumber(currentLineNumber-1);
    QString lineText = currentLine.text();

    QRegularExpressionMatch matchCodeBlock = regexStartBlock.match(lineText);
    if(matchCodeBlock.hasMatch()){
        cursor.insertText("```");
    }
}

void MkEdit::cursorPositionChangedHandle()
{
    int currentBlockNumber = textCursor().blockNumber();

    if(savedBlockNumber != currentBlockNumber){
        savedBlockNumber = currentBlockNumber;
        emit cursorPosChanged( textCursor().hasSelection(), currentBlockNumber);
        this->update();
    }
}

void MkEdit::textChangedHandle()
{

}


