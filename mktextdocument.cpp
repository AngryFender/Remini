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

    QTextCursor rawCursor(&rawDocument);
    rawCursor.setPosition(rawDocument.findBlockByNumber(blockNo).position());
    rawCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    rawCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    rawCursor.insertText(text);
}

void MkTextDocument::setUndoEnterPressedText(const int blockNo, const QString &text)
{
    QTextCursor cursor(this);
    cursor.setPosition(findBlockByNumber(blockNo).position());
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.insertText(text);
    cursor.movePosition(QTextCursor::NextBlock);
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.deleteChar();
    cursor.deleteChar();

    QTextCursor rawCursor(&rawDocument);
    rawCursor.setPosition(rawDocument.findBlockByNumber(blockNo).position());
    rawCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    rawCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    rawCursor.insertText(text);
    rawCursor.movePosition(QTextCursor::NextBlock);
    rawCursor.select(QTextCursor::BlockUnderCursor);
    rawCursor.deleteChar();
    rawCursor.deleteChar();
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

void MkTextDocument::cursorPosChangedHandle(SelectRange * const range, const bool readOnly)
{
    if(range && !readOnly){
        int start;
        int end;

        this->selectRange.hasSelection      = range->hasSelection;
        if(range->hasSelection){
            start = std::min(range->selectionFirstStartBlock, range->selectionEndBlock);
            end   = std::max(range->selectionFirstStartBlock, range->selectionEndBlock);
        }else{
            this->selectRange.currentposInBlock = range->currentposInBlock;
            this->selectRange.currentBlockNo    = range->currentBlockNo;
            start = end = range->currentBlockNo;
        }

        for(int num = start; num <= end; num++){
            this->selectRange.hideBlocks.erase(num);
            this->selectRange.showBlocks.insert(num);
        }

        hideMKSymbolsFromPreviousSelectedBlocks(&this->selectRange);
        showMKSymbolsFromCurrentSelectedBlocks(&this->selectRange);

        range->currentBlockNo    = this->selectRange.currentBlockNo;
        range->currentposInBlock = this->selectRange.currentposInBlock;
    }
}

void MkTextDocument::removeAllMkDataHandle(int blockNo)
{
    this->clear();
    QTextDocument::setPlainText(this->rawDocument.toPlainText());
}

void MkTextDocument::applyAllMkDataHandle(int blockNumber)
{
    QTextBlock block = this->findBlockByNumber(blockNumber);
    block.setUserData(NULL);

    identifyUserData();

    if(disableMarkdownState){
        for(int num = 0; num < this->blockCount(); num++){
            this->selectRange.showBlocks.insert(num);
        }
        showMKSymbolsFromCurrentSelectedBlocks(&this->selectRange);
        return;
    }

    this->selectRange.showBlocks.clear();
    this->selectRange.hideBlocks.clear();
    for(int num = 0; num < this->blockCount(); num++){
        this->selectRange.hideBlocks.insert(num);
    }
    this->selectRange.hideBlocks.erase(blockNumber);
    hideMKSymbolsFromPreviousSelectedBlocks(&this->selectRange);

    this->selectRange.showBlocks.insert(blockNumber);
    showMKSymbolsFromCurrentSelectedBlocks(&this->selectRange);
}

void MkTextDocument::applyMkSingleBlockHandle(int blockNumber)
{
    QTextBlock block = this->findBlockByNumber(blockNumber);
    QTextBlockUserData* data =block.userData();

    BlockData* blockData = dynamic_cast<BlockData*>(data);
    if(!blockData){
        block.setUserData(NULL);
        QRegularExpressionMatch matchHorizontalLine = regexHorizontalLine.match(block.text());
        if(matchHorizontalLine.hasMatch()){
            LineData *lineData = new LineData;
            block.setUserData(lineData);
        }
        identifyFormatData(block);
        this->selectRange.hideBlocks.erase(blockNumber);
        hideMKSymbolsFromPreviousSelectedBlocks(&this->selectRange);
        this->selectRange.showBlocks.insert(blockNumber);
        showMKSymbolsFromCurrentSelectedBlocks(&this->selectRange);
    }
}

void MkTextDocument::identifyUserData()
{
    checkMarkPositions.clear();
    linkPositions.clear();

    QList<BlockData*> codeBlockBody;
    int startBlock;
    BlockData *code;

    bool openBlock = false;
    for(QTextBlock block = this->begin(); block != this->end(); block = block.next()){
        block.setUserData(nullptr);
        QRegularExpressionMatch matchCodeBlock = regexCodeBlock.match(block.text());
        if(matchCodeBlock.hasMatch()){
            if(!openBlock){
                openBlock = true;
                code = new BlockData(BlockData::start);
                startBlock = block.blockNumber();
                block.setUserData(code);
                codeBlockBody.append(code);
            }
            else{
                openBlock = false;
                code = new BlockData(BlockData::end);
                block.setUserData(code);
                codeBlockBody.append(code);

                foreach (auto codeBlock, codeBlockBody) {
                    codeBlock->setStartBlock(startBlock);
                    codeBlock->setEndBlock(block.blockNumber());
                }
                codeBlockBody.clear();
            }
            continue;
        }

        if(openBlock){
            code = new BlockData(BlockData::content);
            block.setUserData(code);
            codeBlockBody.append(code);
            continue;
        }

        QRegularExpressionMatch matchHorizontalLine = regexHorizontalLine.match(block.text());
        if(matchHorizontalLine.hasMatch()){
            block.setUserData(new LineData);
            continue;
        }
        identifyFormatData(block);
    }
}

void MkTextDocument::identifyUserData(QTextBlock &block)
{
    QTextBlockUserData *data = block.userData();
    BlockData *blockData = dynamic_cast<BlockData*>(data);
    if(blockData){
        return;
    }

    block.setUserData(NULL);
    QRegularExpressionMatch matchHorizontalLine = regexHorizontalLine.match(block.text());
    if(matchHorizontalLine.hasMatch()){
        LineData *lineData = new LineData;
        block.setUserData(lineData);
    }else{
        identifyFormatData(block);
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

void MkTextDocument::resetAllLoc()
{
    locItalicA.reset();
    locItalicU.reset();
    locBoldA.reset();
    locBoldU.reset();
    locCheck.reset();
}
void MkTextDocument::identifyFormatData(QTextBlock &block)
{
    resetFormatLocation();
    FormatData *formatData = new FormatData;
    QString text(block.text());
    QString textLink = text;
    QString test;

    formatData->setMaskSize(block.text().size());

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
                    const QString linkUrl = textLink.mid(locLink.start+2, (locLink.end-locLink.start-1));
                    const QString linkTitle = textLink.mid(locLinkTitle.start+1, (locLink.start-locLinkTitle.start-2));
                    insertFormatLinkData(locLinkTitle,locLink, index1, index2, index3 , formatData, test, &linkUrl, &linkTitle);
                    resetAllLoc();
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
        for(auto it = formatData->formats_begin(); it != formatData->formats_end(); ++it){
            if((*it)->getStatus() == FragmentData::LINK_TITLE){
                const QString *linkText = formatData->getLinkUrlFromTitleStart((*it)->getStart());
                const QString *linkTitle = formatData->getLinkTitleFromTitleStart((*it)->getStart());
                if(linkText){
                    formatData->addHiddenFormat((*it)->getStart(),(*it)->getEnd(), FragmentData::LINK_TITLE,*linkText, *linkTitle);
                }
            }else if((*it)->getStatus() == FragmentData::CHECKED_END ||
                       (*it)->getStatus() == FragmentData::UNCHECKED_END){
                formatData->addHiddenFormat((*it)->getEnd(),(*it)->getEnd()+1,(*it)->getStatus(),NULL,NULL);
            }
        }
        block.setUserData(formatData);
    }
}

void MkTextDocument::identifyUnicode(QString &line)
{

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
            loc.start = (loc.start < 1)? 0 : loc.start;
            formatData->addFormat(loc.start, loc.end, test);
            loc.reset();
            resetAllLoc();
        }
    }
    incrementIndexes(index1, index2,index3, test.size());
}

void MkTextDocument::insertFormatCheckBoxData(FormatLocation &loc, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test)
{
    loc.end = index3;
    if(loc.end-loc.start==5){
        formatData->addFormat(loc.start, loc.end, test);
        loc.reset();
    }
    incrementIndexes(index1, index2,index3, test.size());
}

void MkTextDocument::insertFormatLinkData(FormatLocation &locTitle, FormatLocation &locLink, int &index1, int &index2, int &index3, FormatData *formatData, const QString &test, const QString * linkUrl, const QString *linkTitle)
{
    locLink.end = locLink.start + 2 + linkUrl->length() ;
    if(locLink.end-locLink.start>3){
        formatData->addFormat(locTitle.start, locTitle.end, QString(LINK_SYMBOL_TITLE_END),linkUrl,linkTitle);
        formatData->addFormat(locLink.start, locLink.end, QString(LINK_SYMBOL_URL_END), linkUrl,linkTitle);
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

void MkTextDocument::setCodeBlockMargin(QTextBlock &block, int leftMargin,int rightMargin, int topMargin, int bottomMargin)
{
    QTextCursor cursor(block);
    QTextBlockFormat blockFormat = cursor.blockFormat();
    blockFormat.setLeftMargin(leftMargin);
    blockFormat.setRightMargin(rightMargin);

    if(topMargin){
        QString content = block.text().mid(3);
        if(!content.isEmpty()){
            blockFormat.setTopMargin(topMargin);
            blockFormat.setBottomMargin(topMargin*2/3);
        }
        QTextCursor previousBlockCursor(block.previous());
        if(block.isValid()){
            QTextBlockFormat previousBlockFormat = previousBlockCursor.blockFormat();
            previousBlockFormat.setLineHeight(topMargin,QTextBlockFormat::LineDistanceHeight);
            previousBlockCursor.setBlockFormat(previousBlockFormat);
        }
        block.next();
    }

    if(bottomMargin){
        blockFormat.setBottomMargin(bottomMargin);
    }
    cursor.setBlockFormat(blockFormat);
}

void MkTextDocument::resetTextBlockFormat(QTextBlock block)
{
    QTextCursor cursor(block);
    QTextCharFormat format;
    cursor.setPosition(block.position());
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.setCharFormat(format);
}

void MkTextDocument::applyMkFormat(QTextBlock &block, int start, int end, FragmentData::FormatSymbol status, FormatCollection &formatCollection)
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
    case FragmentData::UNCHECKED_END: break;
    case FragmentData::LINK_TITLE: format = formatCollection.getLink(); break;
    default:break;
    }

    if(format == nullptr){
        return;
    }

    int startPoint = block.position()+start;
    int endPoint = (end>=block.length())? block.length()-1:block.position()+end;

    QTextCursor cursor(this);
    cursor.setPosition(startPoint);
    cursor.setPosition(endPoint, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(*format);
}

void MkTextDocument::applyCheckBoxLinkEffect(FormatData *data, QTextBlock &block, QTextCursor &cursor)
{
    for(QVector<FragmentData*>::Iterator it = data->hiddenFormats_begin(); it < data->hiddenFormats_end(); it++)
    {
        if((*it)->getStatus() == FragmentData::CHECKED_END ||
            (*it)->getStatus() == FragmentData::UNCHECKED_END) {

            // Add check mark position for mouse to hover and click
            const QPair<int,int> checkPos(block.blockNumber(), (*it)->getStart());
            if(!checkMarkPositions.contains(checkPos)){
                checkMarkPositions.append(checkPos);
            }

            // Insert the unicode character for checked box or unchecked box
            cursor.setPosition(block.position() + (*it)->getStart());
            cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);

            if((*it)->getStatus() == FragmentData::CHECKED_END){
                cursor.insertText(CHECKED_PIC);
            }else{
                cursor.insertText(UNCHECKED_PIC);
            }

        }else if((*it)->getStatus() == FragmentData::LINK_TITLE){
            // Add link position for mouse to hover and click
            const std::tuple<int,int,int, const QString*> linkPos(block.blockNumber(), (*it)->getStart(),(*it)->getEnd(), data->getHiddenTitle((*it)->getStart()));
            if(!linkPositions.contains(linkPos)){
                linkPositions.append(linkPos);
            }
        }
    }
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

void MkTextDocument::removeCheckBoxLinkMousePosition(QTextBlock &block, FormatData *formatData, SelectRange *range)
{
    QString textBlock = block.text();
    const int blockNo = block.blockNumber();
    QPair<int,int> checkPos;
    int index = 0;
    for(QString::Iterator cp = textBlock.begin(); cp != textBlock.end(); cp++){
        if(*cp == u'☑' || *cp == u'☐'){
            checkPos.first = blockNo; checkPos.second = index;
            checkMarkPositions.removeAll(checkPos);
        }

        auto newEnd = std::remove_if(linkPositions.begin(), linkPositions.end(),[blockNo,index](const std::tuple<int,int,int,const QString*> &linkPos){ return (std::get<0>(linkPos) == blockNo && std::get<1>(linkPos)== index);});
        linkPositions.erase(newEnd, linkPositions.end());
        index++;
    }
}

void MkTextDocument::showSymbolsAtPos(QString &text, int pos, const QString &symbol)
{
    if(symbol == CHECKED_SYMBOL_END ||symbol == UNCHECKED_SYMBOL_END){
        text.remove(pos,1);
    }
    text.insert(pos,symbol);
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

void MkTextDocument::enterKeyPressedHandle(int blockNumber, int &newCursorPos)
{
    numberListDetect(blockNumber, newCursorPos);
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

void MkTextDocument::saveEnterPressRawBlockHandler(int blockNumber, QString text)
{
    QTextCursor rawCursor(&this->rawDocument);
    rawCursor.setPosition(this->rawDocument.findBlockByNumber(blockNumber).position());
    rawCursor.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
    rawCursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
    rawCursor.insertText(text);
}

void MkTextDocument::hideMKSymbolsFromPreviousSelectedBlocks(SelectRange * const range)
{
    //hide raw text from old selection but dont hide blocks from current selection
    emit disconnectCursorPos();
    int fontSize =this->defaultFont().pointSize();
    FormatCollection formatCollection(fontSize);

    if(disableMarkdownState){
        range->hideBlocks.clear();
        return;
    }

    QSet<int> hiddenBlocks;
    QTextBlock block;
    foreach (int num, range->hideBlocks) {
        if(range->showBlocks.find(num) == range->showBlocks.end()){
            hiddenBlocks.insert(num);
            block = this->findBlockByNumber(num);
            if(!block.isValid()){
                continue;
            }

            QTextBlockUserData *data = block.userData();
            if(data == nullptr){
                resetTextBlockFormat(block);
                continue;
            }

            BlockData* blockData = dynamic_cast<BlockData*>(data);
            if(blockData)
            {
                resetTextBlockFormat(block);
                showHideCodeBlock(blockData, true, fontSize);
                if(blockData->getStatus()==BlockData::content){
                    setCodeBlockMargin(block,fontSize*10/3, fontSize, 0);
                }
                continue;
            }

            LineData* lineData = dynamic_cast<LineData*>(data);
            if(lineData && ! lineData->isHidden()){
                resetTextBlockFormat(block);
                lineData->setHidden(true);
                lineData->setDraw(true);
                hideSymbols(block, lineData->getSymbol());
                continue;
            }

            FormatData* formatData = dynamic_cast<FormatData*>(data);
            if(formatData && !formatData->isHidden()){
                formatData->setHidden(true);

                if(!formatData->isFormatted()){
                    for(QVector<FragmentData*>::Iterator it = formatData->formats_begin(); it < formatData->formats_end(); it++)
                    {
                        applyMkFormat(block, (*it)->getStart(), (*it)->getEnd(), (*it)->getStatus(), formatCollection);
                    }
                }

                const QBitArray &mask(formatData->getMask());
                if(block.text().size() != mask.size())
                    continue;

                QTextCursor cursor(block);
                cursor.movePosition(QTextCursor::EndOfBlock);
                for(int index = mask.size()-1; index >= 0; --index){
                    if(mask[index]){
                        cursor.deletePreviousChar();
                    }else{
                        cursor.movePosition(QTextCursor::PreviousCharacter);
                    }
                }

                applyCheckBoxLinkEffect(formatData, block, cursor);
           }
        }
    }

    foreach(int num, hiddenBlocks){
        range->hideBlocks.erase(num);
    }

    emit  connectCurosPos();
}

void MkTextDocument::showMKSymbolsFromCurrentSelectedBlocks( SelectRange * const range)
{
    emit disconnectCursorPos();
    //hide raw text from old selection but dont hide blocks from current selection
    int fontSize =this->defaultFont().pointSize();
    FormatCollection formatCollection(fontSize);
    QTextBlock block;

    foreach(int num, range->showBlocks){
        if(range->hideBlocks.find(num)==range->hideBlocks.end()){
            range->hideBlocks.insert(num);
            block = this->findBlockByNumber(num);
            if(!block.isValid()){
                continue;
            }

            QTextBlockUserData *data = block.userData();
            if(data == nullptr){
                resetTextBlockFormat(block);
                continue;
            }

            BlockData * blockData = dynamic_cast<BlockData*>(data);
            if(blockData){
                resetTextBlockFormat(block);
                showHideCodeBlock(blockData,false,fontSize);
                if(blockData->getStatus() == BlockData::content){
                    setCodeBlockMargin(block,fontSize*10/3, fontSize, 0);
                }
                continue;
            }

            LineData* lineData = dynamic_cast<LineData*>(data);
            if(lineData && lineData->isHidden()){
                lineData->setHidden(false);
                lineData->setDraw(false);
                showSymbols(block, lineData->getSymbol());
                continue;
            }

            FormatData* formatData = dynamic_cast<FormatData*>(data);
            if(formatData){
                bool isPosInBlockAtMax = (range->currentposInBlock == (block.length()-1))? true: false;
                bool isHidden = formatData->isHidden();
                resetTextBlockFormat(block);
                removeCheckBoxLinkMousePosition(block,formatData,range);

                //insert block from raw document
                QTextCursor cursor(block);
                cursor.movePosition(QTextCursor::StartOfBlock);
                cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
                QString line = rawDocument.findBlockByNumber(block.blockNumber()).text();
                cursor.insertText(line);
                identifyFormatData(block);

                FormatData * newData = dynamic_cast<FormatData*>(block.userData());
                newData->setFormatted(true);
                if(newData){
                    for(QVector<FragmentData*>::Iterator it = newData->formats_begin(); it < newData->formats_end(); it++)
                    {
                        applyMkFormat(block, (*it)->getStart(), (*it)->getEnd(), (*it)->getStatus(), formatCollection);
                    }

                    if(isHidden){
                        range->currentBlockNo = block.blockNumber();
                        range->currentposInBlock = (isPosInBlockAtMax)? block.length()-1: newData->getCalculatedCursorPos(range->currentposInBlock);
                    }
                }
            }
        }
    }
    range->showBlocks.clear();


    if(disableMarkdownState){
        range->hideBlocks.clear();
        return;
    }
    emit  connectCurosPos();
}

void MkTextDocument::showHideCodeBlock(BlockData *data, bool hide, int fontSize)
{
    QTextBlock startCode = this->findBlockByNumber(data->getStartBlock());
    QTextBlock endCode = this->findBlockByNumber(data->getEndBlock());

    if(hide){
        BlockData *startBlockData = dynamic_cast<BlockData*>(startCode.userData());
        if(startBlockData){

            setCodeBlockMargin(startCode,fontSize*7/4, fontSize, fontSize);
            if(!startBlockData->isHidden()){
                hideSymbols(startCode, CODEBLOCK_SYMBOL);
                startBlockData->setHidden(true);
            }
        }
        BlockData *endBlockData = dynamic_cast<BlockData*>(endCode.userData());
        if(endBlockData){
            setCodeBlockMargin(endCode,fontSize*7/4, fontSize, 0, fontSize);
            if(!endBlockData->isHidden()){
                hideSymbols(endCode,CODEBLOCK_SYMBOL);
                endBlockData->setHidden(true);
            }
        }
    }else{
        BlockData *startBlockData = dynamic_cast<BlockData*>(startCode.userData());
        if(startBlockData && startBlockData->isHidden()){
            setCodeBlockMargin(startCode,fontSize*7/4, fontSize, fontSize);
            if(startBlockData->isHidden()){
                showSymbols(startCode, CODEBLOCK_SYMBOL);
                startBlockData->setHidden(false);
            }
        }
        BlockData *endBlockData = dynamic_cast<BlockData*>(endCode.userData());
        if(endBlockData && endBlockData->isHidden()){
            showSymbols(endCode,CODEBLOCK_SYMBOL);
            if(endBlockData->isHidden()){
                endBlockData->setHidden(false);
                setCodeBlockMargin(endCode,fontSize*7/4, fontSize, 0, fontSize);
            }
        }
    }
}

void MkTextDocument::pushCheckBoxHandle(const int position)
{
    QTextBlock block = findBlock(position);
    int blockNo = block.blockNumber();
    QTextBlockUserData* data =block.userData();
    FormatData* formatData = dynamic_cast<FormatData*>(data);

    if(!formatData)
        return;

    // Calcutate the raw position of Check box in the block
    int renderedPosInBlock = position - block.position();
    int rawPosInBlock = renderedPosInBlock;
    const QBitArray &mask = formatData->getMask();
    for(int x = 0; x <= rawPosInBlock; ++x){
        if(mask[x]){
            ++rawPosInBlock;
        }
    }

    // replace the checked and unchecked symbols in the raw text
    // replace the checked and unchecked unicode in the rendered text
    int startOfCheckBoxInBlock = rawPosInBlock - 5;
    QString rawText = this->rawDocument.findBlockByNumber(blockNo).text();
    QTextCursor cursor(this);
    cursor.setPosition(position);
    cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);

    for(QVector<FragmentData*>::Iterator it = formatData->hiddenFormats_begin(); it < formatData->hiddenFormats_end(); it++)
    {
        if((*it)->getStatus() == FragmentData::CHECKED_END){
            if((*it)->getStart() == renderedPosInBlock){
                int replaceStartPos = rawText.indexOf("- [x]",startOfCheckBoxInBlock);
                if(replaceStartPos != -1){
                    rawText.replace(replaceStartPos, 5, "- [ ]");
                }
                (*it)->setStatus(FragmentData::UNCHECKED_END);
                cursor.insertText(UNCHECKED_PIC);
                break;
            }
        }else if((*it)->getStatus() == FragmentData::UNCHECKED_END){
            if((*it)->getStart() == renderedPosInBlock){
                int replaceStartPos = rawText.indexOf("- [ ]",startOfCheckBoxInBlock);
                if(replaceStartPos != -1){
                    rawText.replace(replaceStartPos, 5, "- [x]");
                }

                (*it)->setStatus(FragmentData::CHECKED_END);
                cursor.insertText(CHECKED_PIC);
                break;
            }
        }
    }

    // Update the raw document as well
    QTextCursor cursorRaw(&rawDocument);
    cursorRaw.setPosition(rawDocument.findBlockByLineNumber(blockNo).position());
    cursorRaw.movePosition(QTextCursor::StartOfBlock);
    cursorRaw.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursorRaw.insertText(rawText);
}

void MkTextDocument::pushLinkHandle(const int blockNo, const int posInBlock)
{
    QTextBlock block = findBlockByNumber(blockNo);
    QTextBlockUserData* data =block.userData();
    FormatData* formatData = dynamic_cast<FormatData*>(data);

    if(!formatData)
        return;

    for(QVector<FragmentData*>::Iterator it = formatData->hiddenFormats_begin(); it < formatData->hiddenFormats_end(); it++)
    {
        if((*it)->getStatus() == FragmentData::LINK_TITLE && (*it)->getStart() == posInBlock){
            const QString *link =formatData->getHiddenLinkUrl((*it)->getStart());
            if(link){
                QDesktopServices::openUrl(QUrl(*link));
            }
        }
    }
}

void MkTextDocument::autoInsertSymbolHandle(const int position, int &newPosition)
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
        cursor.setPosition(cursor.position()-2);
    }else{
        cursor.setPosition(position);
        cursor.insertText("    ");
    }
    newPosition = cursor.positionInBlock();
}

void MkTextDocument::setMarkdownHandle(bool state)
{
    disableMarkdownState = !state;

    if(disableMarkdownState){
        this->setPlainText(this->rawDocument.toPlainText());
        for(int num = 0; num < this->blockCount(); num++){
            this->selectRange.showBlocks.insert(num);
        }

        showMKSymbolsFromCurrentSelectedBlocks(&this->selectRange);
    }else{
        for(int num = 0; num < this->blockCount(); num++){
            this->selectRange.hideBlocks.insert(num);
        }

        hideMKSymbolsFromPreviousSelectedBlocks(&this->selectRange);
        this->selectRange.hideBlocks.clear();
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

void MkTextDocument::undoStackUndo(bool &success)
{
    if(undoStack.canUndo()){
        undoStack.undo();
        success = true;
    }
}

void MkTextDocument::undoStackRedo(bool &success)
{
    if(undoStack.canRedo()){
        undoStack.redo();
        success = true;
    }
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

void MkTextDocument::numberListDetect(int blockNumber, int &newCursorPosition)
{
    QTextCursor cursor(this->findBlockByNumber(blockNumber));
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
        cursor.insertText(QString("").leftJustified(spaces,' '));
        cursor.insertText(numberListGetNextNumber(matchNumbering.captured(0)));
    }else if(matchBulletCheckBox.hasMatch()){
        int spaces = numberListGetSpaces(matchBulletPoint.captured(0));
        cursor.insertText(QString("").leftJustified(spaces,' '));
        if(matchCheckBox.captured(0).contains("x")){
            cursor.insertText("- - [x]  ");
        }else{
            cursor.insertText("- - [ ]  ");
        }
    }else if(matchCheckBox.hasMatch()){
        int spaces = numberListGetSpaces(matchBulletPoint.captured(0));
        cursor.insertText(QString("").leftJustified(spaces,' '));
        if(matchCheckBox.captured(0).contains("x")){
            cursor.insertText("- [x]  ");
        }else{
            cursor.insertText("- [ ]  ");
        }
    }else if(matchBulletPoint.hasMatch()){
        int spaces = numberListGetSpaces(matchBulletPoint.captured(0));
        cursor.insertText(QString("").leftJustified(spaces,' '));
        cursor.insertText("- ");
    }
    newCursorPosition = cursor.positionInBlock();
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
    this->viewEditTypeStore = data.viewEditTypeStore;
    this->doc = dynamic_cast<MkTextDocument*>(data.doc);
    this->scrollValue = data.scrollValue;
    isConstructorRedo = true;
    this->undoSelectRange = data.oldSelectRange;
    this->viewSelectRangeStore = data.viewSelectRangeStore;
    this->blockNo = data.blockNo;
    this->posInBlock = data.posInBlock;
    this->editType = data.editType;
    this->undoSelectRange.scrollValue = data.scrollValue;

    switch(editType){
    case undoRedo: return;
    case singleEdit:
        this->undoText = data.oldBlock;
        this->redoText = data.doc->findBlockByNumber(data.blockNo).text();
        this->undoSelectRange.isCheckBox = false;
        break;
    case checkbox:  this->undoSelectRange.isCheckBox = true;
    case enterPressed:
    case multiEdit:
        this->undoText = data.oldText;
        this->redoText = (doc? doc->getRawDocument()->toPlainText(): data.doc->toPlainText());
        break;
    }
}

void EditCommand::undo()
{
    switch(editType){
    case undoRedo: return;
    case singleEdit: doc->setUndoRedoText(undoSelectRange.currentBlockNo, this->undoText); break;
    case checkbox:
    case enterPressed:
    case multiEdit: doc->setUndoRedoText(undoText); break;
    }
    *viewEditTypeStore = editType;
    *viewSelectRangeStore = undoSelectRange;
}

void EditCommand::redo()
{
    if(isConstructorRedo){
        isConstructorRedo = false;
    }else{
        switch(editType){
        case undoRedo: return;
        case singleEdit: doc->setUndoRedoText(this->blockNo, this->redoText);break;
        case checkbox:
        case enterPressed:
        case multiEdit: doc->setUndoRedoText(redoText); break;
        }

        *this->viewEditTypeStore = editType;

        viewSelectRangeStore->hasSelection = false;
        viewSelectRangeStore->currentBlockNo = this->blockNo;
        viewSelectRangeStore->currentposInBlock = this->posInBlock;
        viewSelectRangeStore->isCheckBox = this->undoSelectRange.isCheckBox;
        viewSelectRangeStore->scrollValue = this->undoSelectRange.scrollValue;
    }
}
