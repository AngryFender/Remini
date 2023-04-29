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
#define HEADING1_SYMBOL "# "
#define HEADING2_SYMBOL "## "
#define HEADING3_SYMBOL "### "


#define CHECKED_PIC "☑"
#define UNCHECKED_PIC "☐"

#define CHECKED_FULL_COUNT 5
#define HEADING1_SYMBOL_COUNT 2
#define HEADING2_SYMBOL_COUNT 3
#define HEADING3_SYMBOL_COUNT 4

class FragmentData{
public:
    enum FormatSymbol{
        BOLD,
        ITALIC,
        STRIKETHROUGH,
        CHECK_START,
        CHECKED_END,
        UNCHECKED_END,
        HEADING1,
        HEADING2,
        HEADING3
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

    void addFormat(int start, int end, const QString& symbol);
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

    bool isHiddenFormatsEmpty() const;
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

    void addHiddenFormat(int start, int end,int length, FragmentData::FormatSymbol status);
};

#endif // FORMATDATA_H
