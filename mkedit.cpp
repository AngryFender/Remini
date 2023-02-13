#include "blockdata.h"
#include "mkedit.h"


void MkEdit::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::NonCosmeticBrushPatterns);

    QTextBlock block = this->document()->begin();
    bool drawBlock = false;
    int xBlock, yBlock;

    int codeId = 1;
    while (block.isValid()) {
        QTextBlockUserData* data =block.userData();
        BlockData* blockData = static_cast<BlockData*>(data);
        if(blockData){
            if(blockData->getStatus()==BlockData::start){
                xBlock = block.layout()->position().x()-2;
                yBlock = block.layout()->position().y();
                positionStartBlock = block.blockNumber();
            }

            if(blockData->getStatus()==BlockData::end){
                positionEndBlock = block.blockNumber();
                int height = block.layout()->position().y() - yBlock +(int)TEXT_SIZE;
                painter.setPen(penCodeBlock);
                painter.drawRoundedRect(xBlock,yBlock,widthCodeBlock,height,BLOCKRADIUS,BLOCKRADIUS);
                painter.drawLine(xBlock,yBlock+TEXT_SIZE+7,widthCodeBlock,yBlock+TEXT_SIZE+7);
                codeId++;
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

void MkEdit::keyPressEvent(QKeyEvent *event)
{
    QTextEdit::keyPressEvent(event);
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        numberListDetect();
    }
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

void MkEdit::cursorPositionChanged()
{
    int currentBlockNumber = textCursor().blockNumber();

    if(savedBlockNumber != currentBlockNumber){
        savedBlockNumber = currentBlockNumber;
        emit cursorPosChanged( this->document(), currentBlockNumber);
        this->update();

    }
}


