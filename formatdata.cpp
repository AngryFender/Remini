#include "formatdata.h"

PositionData::PositionData(int position, QString symbol){
    this->position = position;
    this->symbol = symbol;
    this->hidden = false;
}

int PositionData::getPos() const{
    return position;
}

QString &PositionData::getSymbol(){
    return symbol;
}

bool &PositionData::isHidden()
{
    return hidden;
}

void PositionData::setHidden(bool hide)
{
    this->hidden = hide;
}

FormatData::FormatData()
{
    hidden = false;
}

FormatData::~FormatData()
{
    for(auto format:formats){
        delete format;
    }
    formats.clear();

    for(auto pos:positions){
        delete pos;
    }
    positions.clear();
}

void FormatData::addFormat(int start, int end, QString &symbol)
{
    FragmentData::FormatSymbol status = FragmentData::BOLD ;
    if(symbol == BOLD_SYMBOL_A || symbol == BOLD_SYMBOL_U){
        status = FragmentData::BOLD;
    }else if(symbol == ITALIC_SYMBOL_A || symbol == ITALIC_SYMBOL_U){
        status = FragmentData::ITALIC;
    }else if (symbol == STRIKETHROUGH_SYMBOL){
        status = FragmentData::STRIKETHROUGH;
    }else if (symbol == CHECK_SYMBOL_START){
        status = FragmentData::CHECK_START;
    }else if (symbol == CHECKED_SYMBOL_END){
        positions.append(new PositionData(start,CHECK_SYMBOL_START));
        positions.append(new PositionData(end,CHECKED_SYMBOL_END));
        return;
    }else if (symbol == UNCHECKED_SYMBOL_END){
        positions.append(new PositionData(start,CHECK_SYMBOL_START));
        positions.append(new PositionData(end,UNCHECKED_SYMBOL_END));
        return;
    }
    formats.append(new FragmentData(start,end,status));
    positions.append(new PositionData(start,symbol));
    positions.append(new PositionData(end,symbol));
}

bool FormatData::isEmpty()
{
    return positions.empty();
}

bool &FormatData::isHidden()
{
    return hidden;
}

void FormatData::setHidden(bool hide)
{
    this->hidden = hide;
}

void FormatData::sortAscendingPos()
{
    std::sort(positions.begin(), positions.end(),sortAscendingStartPos);
}

int FragmentData::getStart()
{
    return start;
}

int FragmentData::getEnd()
{
    return end;
}

FragmentData::FormatSymbol FragmentData::getStatus()
{
    return status;
}
