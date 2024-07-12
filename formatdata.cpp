#include "formatdata.h"

PositionData::PositionData(const int position, const QString &symbol){
    this->position = position;
    this->symbol = symbol;
    this->hidden = false;
}

int PositionData::getPos() const{
    return position;
}

const QString &PositionData::getSymbol() const{
    return symbol;
}

bool PositionData::isHidden() const
{
    return hidden;
}

void PositionData::setHidden(const bool hide)
{
    this->hidden = hide;
}

void PositionData::setSymbol(const QString &symbol)
{
    this->symbol = symbol;
}

FormatData::FormatData()
{
    hidden = false;
    formatted = false;
}

FormatData::~FormatData()
{
    for(auto format:formats){
        delete format;
    }
    formats.clear();

    for(auto format:hiddenFormats){
        delete format;
    }
    hiddenFormats.clear();

    qDeleteAll(linkMap);
    linkMap.clear();

    qDeleteAll(linkMapHidden);
    linkMapHidden.clear();

    for(auto pos:positions){
        delete pos;
    }
    positions.clear();
}

void FormatData::addFormat(const int start, const int end,const QString &symbol, QString* linkText)
{
    FragmentData::FormatSymbol status = FragmentData::BOLD ;
    if(symbol == HEADING1_SYMBOL){
        formats.append(new FragmentData(start,end,FragmentData::HEADING1));
        mask.setBit(0,true);
        mask.setBit(1,true);
        return;
    }else if(symbol == HEADING2_SYMBOL){
        formats.append(new FragmentData(start,end,FragmentData::HEADING2));
        mask.setBit(0,true);
        mask.setBit(1,true);
        mask.setBit(2,true);
        return;
    }else if(symbol == HEADING3_SYMBOL){
        formats.append(new FragmentData(start,end,FragmentData::HEADING3));
        mask.setBit(0,true);
        mask.setBit(1,true);
        mask.setBit(2,true);
        mask.setBit(3,true);
        return;
    }else if (symbol == CHECKED_SYMBOL_END){
        formats.append(new FragmentData(start,end,FragmentData::CHECKED_END));
        for(int x = start; x < end; ++x){
            mask.setBit(x,true);
        }
        return;
    }else if (symbol == UNCHECKED_SYMBOL_END){
        formats.append(new FragmentData(start,end,FragmentData::UNCHECKED_END));
        for(int x = start; x < end; ++x){
            mask.setBit(x,true);
        }
        return;
    }else if (symbol == LINK_SYMBOL_TITLE_END){
        formats.append(new FragmentData(start+1,end,FragmentData::LINK_TITLE));
        addMaskBit(start,LINK_SYMBOL_TITLE_START,end,LINK_SYMBOL_TITLE_END);
        if(linkText){
            linkUrlMap.insert(start+1, new QString(*linkText));
        }
        return;
    }else if (symbol == LINK_SYMBOL_URL_END){
        for(int x = start; x < (end + symbol.length()); ++x){
            mask.setBit(x,true);
        }
        return;
    }
    else if(symbol == BOLD_SYMBOL_A || symbol == BOLD_SYMBOL_U){
        status = FragmentData::BOLD;
        formats.append(new FragmentData(start+2,end,status));
    }else if(symbol == ITALIC_SYMBOL_A || symbol == ITALIC_SYMBOL_U){
        status = FragmentData::ITALIC;
        formats.append(new FragmentData(start+1,end,status));
    }else if (symbol == STRIKETHROUGH_SYMBOL){
        status = FragmentData::STRIKETHROUGH;
        formats.append(new FragmentData(start+2,end,status));
    }else if (symbol == CHECK_SYMBOL_START){
        return;
    }
    addMaskBit(start,symbol,end,symbol);
}

bool FormatData::isEmpty() const
{
    return formats.empty();
}

bool FormatData::isHidden() const
{
    return hidden;
}

bool FormatData::isFormatted() const
{
    return formatted;
}

void FormatData::setHidden(bool hide)
{
    this->hidden = hide;
}

void FormatData::setFormatted(bool state)
{
    this->formatted = state;
}

int FormatData::positionsCount() const{
    return positions.count();
}

int FormatData::getFormatCounts() const
{
    return formats.count();
}

int FormatData::hiddenFormatsCount() const
{
    return hiddenFormats.count();
}

const QString &FormatData::getLinkUrl(int key) const
{
    return *linkMap.value(key);
}

const QString *FormatData::getHiddenLinkUrl(int key) const
{
    return linkMapHidden.value(key);
}

const QString *FormatData::getLinkUrlFromTitleStart(int key) const
{
    return linkUrlMap.value(key);
}

void FormatData::insertLinkText(int key, QString *linkText)
{
    linkMap.insert(key, linkText);
}

void FormatData::clearAllLinkMapt()
{
    qDeleteAll(linkMap);
    linkMap.clear();
}

void FormatData::clearAllLinkMapHidden()
{
    qDeleteAll(linkMapHidden);
    linkMapHidden.clear();
}

void FormatData::clearElementLinkMapHidden(int key)
{
    linkMapHidden.remove(key);
}

void FormatData::sortAscendingPos()
{
    std::sort(positions.begin(), positions.end(),sortAscendingStartPos);
}

void FormatData::setMaskSize(const int size)
{
    mask.resize(size);
}

const QBitArray& FormatData::getMask()
{
    return mask;
}

QString FormatData::getOriginalText() const
{
    return originalText;
}

void FormatData::setOriginalText(const QString &originalText)
{
    this->originalText = originalText;
}

int FormatData::getCalculatedCursorPos(const int posInBlock)
{
    if(posInBlock>=mask.count()){
        return posInBlock;
    }

    int addedCharacters(0);
    int dynamicPosInBock = posInBlock;
    for(int i = 0; i <= (dynamicPosInBock); ++i){

        if(dynamicPosInBock >= mask.count()){
            break;
        }

        if(mask[i]){
            addedCharacters++;
            dynamicPosInBock++;
        }

    }
    return posInBlock+addedCharacters;
}

void FormatData::addHiddenFormat(const int start, const int end, const FragmentData::FormatSymbol &status,const QString &linkText)
{
    int begin(0), last(0), hiddenBits(0);
    for(int x = 0; x <= start; ++x){
        hiddenBits = (mask[x])? hiddenBits+1: hiddenBits;
    }

    begin = start - hiddenBits;
    last  = end - hiddenBits;
    qDebug()<<"start"<<start<<"end"<<end<<"hiddenBit"<<hiddenBits<<"begin"<<begin<<"last"<<last;

    if(!linkText.isEmpty() && status == FragmentData::LINK_TITLE){
        linkMapHidden.insert(begin, new QString(linkText));
    }

    hiddenFormats.append(new FragmentData(begin,last,status));
}

void FormatData::addHiddenFormat(const int start, const int end, const int length, const FragmentData::FormatSymbol status, QString*linkText )
{
    int accumulate = 0;
    if(!hiddenFormats.empty()){
        accumulate = hiddenFormats.last()->getAccumulate();
    }

    int begin = start-accumulate;
    begin =(begin<0)? start-length: begin;
    int last = end-accumulate-length;

    if(linkText && status == FragmentData::LINK_URL){
        linkMap.insert(start,new QString(*linkText));
        accumulate+=(linkText->length());
    }

    if(status == FragmentData::CHECKED_END ||status == FragmentData::UNCHECKED_END){
        accumulate += CHECKED_FULL_COUNT;
    }else if(status == FragmentData::HEADING1){
        accumulate += HEADING1_SYMBOL_COUNT;
        begin = last = 0;
    }else if(status == FragmentData::HEADING2){
        accumulate += HEADING2_SYMBOL_COUNT;
        begin = last = 0;
    }else if(status == FragmentData::HEADING3){
        accumulate += HEADING3_SYMBOL_COUNT;
        begin = last = 0;
    }
    else{
        accumulate = accumulate+2*length;
    }

    if(linkText && status == FragmentData::LINK_TITLE){
        linkMapHidden.insert(begin, new QString(*linkText));
    }

    hiddenFormats.append(new FragmentData(begin,last,status,accumulate));
}

int FragmentData::getStart() const
{
    return start;
}

int FragmentData::getEnd() const
{
    return end;
}

FragmentData::FormatSymbol FragmentData::getStatus() const
{
    return status;
}

int FragmentData::getAccumulate() const
{
    return accumulate;
}

void FragmentData::setStatus(FormatSymbol status)
{
    this->status = status;
}

void FormatData::addMaskBit(const int start,const QString &startSymbol,const int end,const QString &endSymbol)
{
    int startLen = startSymbol.length();
    int endLen = startSymbol.length();

    for(int i = start; i < (startLen+start); ++i){
        mask.setBit(i,true);
    }

    for(int j = end; j < (endLen+end); ++j){
        mask.setBit(j,true);
    }
}

void FormatData::addMaskBitForLinkContent(const int start, const int end, const QString &endSymbol)
{
    int endLen = endSymbol.length();

    for(int i = start; i < (endLen+end); ++i){
        mask.setBit(i,true);
    }
}

