#include "mkedit.h"

void MkEdit::paintEvent(QPaintEvent *e)
{

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    QTextBlock block = this->document()->begin();
    bool drawBlock = false;
    int xBlock, yBlock;
    while (block.isValid()) {
        QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(block.text());
        if(matchCodeBlock.hasMatch()){

            if(!drawBlock){
                xBlock = block.layout()->position().x()-2;
                yBlock = block.layout()->position().y();
                drawBlock = true;
            }else{
                drawBlock = false;
                int height = block.layout()->position().y() - yBlock +TEXT_SIZE;
                painter.setPen(penCodeBlock);
                painter.drawRoundedRect(xBlock,yBlock,widthCodeBlock,height,5,5);
            }
        }
        block = block.next();
    }
    QTextEdit::paintEvent(e);
}

void MkEdit::keyPressEvent(QKeyEvent *event)
{
    QTextEdit::keyPressEvent(event);
    if( (event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return))
    {
        numberListDetect();
        codeBockDetect();
    }
}

void MkEdit::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);
    widthCodeBlock = this->width()-15;
    heightCodeBlock = this->height();
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



//    cursor.beginEditBlock();
//    cursor.insertText("Your text here");

//    QTextBlockFormat format;
//    format.setTopBorder(1, Qt::SolidLine);
//    format.setBottomBorder(1, Qt::SolidLine);
//    format.setLeftBorder(1, Qt::SolidLine);
//    format.setRightBorder(1, Qt::SolidLine);

//    cursor.setBlockFormat(format);
//    cursor.endEditBlock();
}


