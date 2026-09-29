#include "linedata.h"

LineData::LineData() : status(horizontalLine), draw(true), hidden(false) {}

LineData::statusID LineData::getStatus() const { return status; }

void LineData::setStatus(statusID newStatus) { status = newStatus; }

const QString &LineData::getSymbol() const
{
    // Static so we return a reference and avoid re-constructing "---" on
    // every call. The string is immutable, so this is safe.
    static const QString symbol = QStringLiteral("---");
    return symbol;
}

bool LineData::getDraw() const { return draw; }

void LineData::setDraw(bool newDraw) { draw = newDraw; }

void LineData::setHidden(bool hidden) { this->hidden = hidden; }

bool LineData::isHidden() const { return hidden; }
