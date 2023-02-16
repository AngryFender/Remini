#ifndef LINEDATA_H
#define LINEDATA_H

#include <QTextBlockUserData>

class LineData : public QTextBlockUserData
{
public:
    LineData();
    enum statusID{
        horizontalLine,
        heading1,
        heading2,
        heading3,
    } ;

    statusID getStatus() const;
    void setStatus(statusID newStatus);
    QString getSymbol();
    bool getDraw() const;
    void setDraw(bool newDraw);

private:
    statusID status;
    bool draw;
};

#endif // LINEDATA_H
