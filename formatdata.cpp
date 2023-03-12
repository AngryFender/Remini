#include "formatdata.h"

PositionData::PositionData(int position, QString symbol){
    this->position = position;
    this->symbol = symbol;
    this->hidden = false;
}

int PositionData::getPos(){
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
    FragmentData::FormatSymbol status;
    if(symbol == BOLD_SYMBOL_A || symbol == BOLD_SYMBOL_U){
        status = FragmentData::BOLD;
    }else if(symbol == ITALIC_SYMBOL_A || symbol == ITALIC_SYMBOL_U){
        status = FragmentData::ITALIC;
    }else{
        status = FragmentData::STRIKETHROUGH;
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
