#ifndef FORMATDATA_H
#define FORMATDATA_H

#include <QBitArray>
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
#define LINK_SYMBOL_TITLE_START "["
#define LINK_SYMBOL_TITLE_END "]"
#define LINK_SYMBOL_MID "](<"
#define LINK_SYMBOL_URL_START "(<"
#define LINK_SYMBOL_URL_END ">)"

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
        HEADING3,
        LINK_TITLE,
        LINK_URL,
    };
    FragmentData(const int start,const int end,const FormatSymbol status){
        this->start = start;
        this->end = end;
        this->status = status;
    }
    int getStart() const;
    int getEnd() const;
    FormatSymbol getStatus() const;
    void setStatus(FormatSymbol status);
private:
    int start;
    int end;
    FormatSymbol status;
};

class FormatData : public QTextBlockUserData
{
public:
    FormatData();
    ~FormatData();

    void addFormat(const int start, const int end, const QString& symbol, const QString* linkUrl = nullptr, const QString* linkTitle = nullptr);
    void addHiddenFormat(const int start, const int end, const FragmentData::FormatSymbol &status, const QString &linkUrl, const QString &linkTitle);

    QVector<FragmentData*>::Iterator formats_begin() {return formats.begin();};
    QVector<FragmentData*>::Iterator formats_end() {return formats.end();};
    QVector<FragmentData*>::reverse_iterator formats_rbegin() {return formats.rbegin();};
    QVector<FragmentData*>::reverse_iterator formats_rend() {return formats.rend();};

    QVector<FragmentData*>::Iterator hiddenFormats_begin() {return hiddenFormats.begin();};
    QVector<FragmentData*>::Iterator hiddenFormats_end() {return hiddenFormats.end();};

    bool isEmpty() const;
    bool isHidden() const;
    bool isFormatted() const;
    int getFormatCounts() const;
    int hiddenFormatsCount() const;
    const QString &getLinkUrl(int key) const;
    const QString *getHiddenLinkUrl(int key) const;
    const QString *getHiddenTitle(int key) const;
    const QString *getLinkUrlFromTitleStart(int key) const;
    const QString *getLinkTitleFromTitleStart(int key) const;

    void insertLinkText(int key, QString * linkText);

    void setHidden(bool hide);
    void setFormatted(bool state);

    void setMaskSize(const int size);
    const QBitArray &getMask();

    QString getOriginalText() const;
    void setOriginalText(const QString &newOriginalText);

    int getCalculatedCursorPos(const int posInBlock);
private:
    QVector<FragmentData*> formats;
    QVector<FragmentData*> hiddenFormats;
    QMap<int, QString*> linkMap;
    QMap<int, QString*> linkMapHidden;
    QMap<int, QString*> linkTitleMapHidden;
    QMap<int, QString*> linkUrlMap;
    QMap<int, QString*> linkTitleMap;
    QBitArray mask;
    QString originalText;
    bool hidden;
    bool formatted;
    void addMaskBit(const int start, const QString &startSymbol, const int end, const QString &endSymbol);
    void addMaskBitForLinkContent(const int start, const int end, const QString &endSymbol);
};

#endif // FORMATDATA_H
