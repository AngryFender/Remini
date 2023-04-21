#ifndef FORMATDATA_H
#define FORMATDATA_H

#include <QTextBlockUserData>
#include <QVector>

#define BOLD_SYMBOL_A "**"
#define BOLD_SYMBOL_U "__"
#define ITALIC_SYMBOL_A "*"
#define ITALIC_SYMBOL_U "_"
#define STRIKETHROUGH_SYMBOL "~~"
#define CHECK_SYMBOL_START "- ["
#define CHECKED_SYMBOL_END "x] "
#define UNCHECKED_SYMBOL_END " ] "

#define CHECKED_PIC "☑"
#define UNCHECKED_PIC "☐"

class FragmentData{
public:
    enum FormatSymbol{
        BOLD,
        ITALIC,
        STRIKETHROUGH,
        CHECK_START,
        CHECKED_END,
        UNCHECKED_END
    };
    FragmentData(int start, int end, FormatSymbol status){
        this->start = start;
        this->end = end;
        this->status = status;
    }
    FragmentData(int start, int end, FormatSymbol status, int accumulate){
        this->start = start;
        this->end = end;
        this->status = status;
        this->accumulate = accumulate;
    }

    int getStart();
    int getEnd();
    FormatSymbol getStatus();
    int getAccumulate();
private:
    int start;
    int end;
    int accumulate;
    FormatSymbol status;
};

class PositionData{
public:
    PositionData(int position, QString symbol);

    int getPos() const;
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

    QVector<FragmentData*>::Iterator hiddenFormats_begin() {return hiddenFormats.begin();};
    QVector<FragmentData*>::Iterator hiddenFormats_next(QVector<FragmentData*>::Iterator it) {return it++;};
    QVector<FragmentData*>::Iterator hiddenFormats_end() {return hiddenFormats.end();};

    QVector<PositionData*>::Iterator pos_begin() {return positions.begin();};
    QVector<PositionData*>::Iterator pos_next(QVector<PositionData*>::Iterator it) {return it++;};
    QVector<PositionData*>::Iterator pos_previous(QVector<PositionData*>::Iterator it) {return it--;};
    QVector<PositionData*>::Iterator pos_end() {return positions.end();};

    bool isEmpty();
    bool& isHidden();
    void setHidden(bool hide);
    int positionsCount(){
        return positions.count();
    }
    void sortAscendingPos();

    static bool sortAscendingStartPos(const PositionData* f1, const PositionData* f2)
    {
        return(f1->getPos() < f2->getPos());
    }

private:
    QVector<FragmentData*> formats;
    QVector<FragmentData*> hiddenFormats;
    QVector<PositionData*> positions;
    bool hidden;
};

#endif // FORMATDATA_H
