#ifndef LINEDATA_H
#define LINEDATA_H

#include <QTextBlockUserData>

#define HORIZONTALLINE_SYMBOL "---"

class LineData : public QTextBlockUserData
{
public:
    LineData();
    enum statusID{
        horizontalLine,
    } ;

    statusID getStatus() const;
    void setStatus(statusID newStatus);
    QString getSymbol();
    bool getDraw() const;
    void setDraw(bool newDraw);
    void setHidden(bool hidden);
    bool isHidden();

private:
    statusID status;
    bool draw;
    bool hidden;
};

#endif // LINEDATA_H
