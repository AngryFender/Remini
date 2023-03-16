#include "linedata.h"

LineData::LineData()
{
    status = horizontalLine;
    draw = true;
}

LineData::statusID LineData::getStatus() const
{
    return status;
}

void LineData::setStatus(statusID newStatus)
{
    this->status = newStatus;
}

QString LineData::getSymbol()
{
    switch(status){
    case horizontalLine: return "---"; break;
    case heading1:return "#"; break;
    case heading2:return "##"; break;
    case heading3:return "###"; break;
    default:return "";
    }
}

bool LineData::getDraw() const
{
    return draw;
}

void LineData::setDraw(bool newDraw)
{
    draw = newDraw;
}

void LineData::setHidden(bool hidden)
{
    this->hidden = hidden;
}

bool LineData::isHidden()
{
    return hidden;
}
