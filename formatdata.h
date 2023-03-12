#ifndef FORMATDATA_H
#define FORMATDATA_H

#include <QTextBlockUserData>
#include <QVector>

#define BOLD_SYMBOL_A "**"
#define BOLD_SYMBOL_U "__"
#define ITALIC_SYMBOL_A "*"
#define ITALIC_SYMBOL_B "_"
#define STRIKETHROUGH_SYMBOL "~~"

class FragmentData{
public:
    FragmentData(int start, int end, QString symbol){
        this->start = start;
        this->end = end;
        this->symbol = symbol;
    }
private:
    int start;
    int end;
    QString symbol;
};

class PositionData{
public:
    PositionData(int position, QString symbol);

    int getPos();
    QString &getSymbol();

    bool& isHidden();
    void setHidden(bool hide);
private:
    int position;
    QString symbol;
    bool hidden;
};

class FormatData : public QTextBlockUserData
{
public:
    FormatData();
    ~FormatData();

    void addFormat(int start, int end, QString& symbol);
    QVector<FragmentData*>::Iterator formats_begin() {return formats.begin();};
    QVector<FragmentData*>::Iterator formats_next(QVector<FragmentData*>::Iterator it) {return it++;};
    QVector<FragmentData*>::Iterator formats_end() {return formats.end();};

    QVector<PositionData*>::Iterator pos_begin() {return positions.begin();};
    QVector<PositionData*>::Iterator pos_next(QVector<PositionData*>::Iterator it) {return it++;};
    QVector<PositionData*>::Iterator pos_previous(QVector<PositionData*>::Iterator it) {return it--;};
    QVector<PositionData*>::Iterator pos_end() {return positions.end();};

    bool isEmpty();
    bool& isHidden();
    void setHidden(bool hide);

private:
    QVector<FragmentData*> formats;
    QVector<PositionData*> positions;
    bool hidden;
};

#endif // FORMATDATA_H
