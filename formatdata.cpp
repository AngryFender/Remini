#include "formatdata.h"

FormatData::FormatData() : hidden(false), formatted(false) {}

FormatData::~FormatData()
{
    for (auto format : formats) delete format;
    formats.clear();

    for (auto format : hiddenFormats) delete format;
    hiddenFormats.clear();

    qDeleteAll(linkMap);
    linkMap.clear();

    qDeleteAll(linkMapHidden);
    linkMapHidden.clear();

    qDeleteAll(linkTitleMap);
    linkTitleMap.clear();

    qDeleteAll(linkTitleMapHidden);
    linkTitleMapHidden.clear();

    // Previously missing: linkUrlMap also owns heap-allocated QStrings.
    qDeleteAll(linkUrlMap);
    linkUrlMap.clear();
}

void FormatData::addFormat(const int start, const int end,
                           const QString &symbol, const QString *linkUrl,
                           const QString *linkTitle)
{
    FragmentData::FormatSymbol status = FragmentData::BOLD;

    if (symbol == HEADING1_SYMBOL)
    {
        formats.append(new FragmentData(start, end, FragmentData::HEADING1));
        mask.setBit(0, true);
        mask.setBit(1, true);
        return;
    }
    else if (symbol == HEADING2_SYMBOL)
    {
        formats.append(new FragmentData(start, end, FragmentData::HEADING2));
        mask.setBit(0, true);
        mask.setBit(1, true);
        mask.setBit(2, true);
        return;
    }
    else if (symbol == HEADING3_SYMBOL)
    {
        formats.append(new FragmentData(start, end, FragmentData::HEADING3));
        mask.setBit(0, true);
        mask.setBit(1, true);
        mask.setBit(2, true);
        mask.setBit(3, true);
        return;
    }
    else if (symbol == CHECKED_SYMBOL_END)
    {
        formats.append(new FragmentData(start, end, FragmentData::CHECKED_END));
        for (int x = start; x < end; ++x) mask.setBit(x, true);
        return;
    }
    else if (symbol == UNCHECKED_SYMBOL_END)
    {
        formats.append(
            new FragmentData(start, end, FragmentData::UNCHECKED_END));
        for (int x = start; x < end; ++x) mask.setBit(x, true);
        return;
    }
    else if (symbol == LINK_SYMBOL_TITLE_END)
    {
        formats.append(
            new FragmentData(start + 1, end, FragmentData::LINK_TITLE));
        addMaskBit(start, LINK_SYMBOL_TITLE_START, end, LINK_SYMBOL_TITLE_END);
        if (linkUrl && linkTitle)
        {
            linkUrlMap.insert(start + 1, new QString(*linkUrl));
            linkTitleMap.insert(start + 1, new QString(*linkTitle));
        }
        return;
    }
    else if (symbol == LINK_SYMBOL_URL_END)
    {
        for (int x = start; x < (end + symbol.length()); ++x)
            mask.setBit(x, true);
        return;
    }
    else if (symbol == BOLD_SYMBOL_A || symbol == BOLD_SYMBOL_U)
    {
        status = FragmentData::BOLD;
        formats.append(new FragmentData(start + 2, end, status));
    }
    else if (symbol == ITALIC_SYMBOL_A || symbol == ITALIC_SYMBOL_U)
    {
        status = FragmentData::ITALIC;
        formats.append(new FragmentData(start + 1, end, status));
    }
    else if (symbol == STRIKETHROUGH_SYMBOL)
    {
        status = FragmentData::STRIKETHROUGH;
        formats.append(new FragmentData(start + 2, end, status));
    }
    else if (symbol == CHECK_SYMBOL_START)
    {
        return;
    }

    addMaskBit(start, symbol, end, symbol);
}

bool FormatData::isEmpty() const { return formats.empty(); }

bool FormatData::isHidden() const { return hidden; }

bool FormatData::isFormatted() const { return formatted; }

void FormatData::setHidden(bool hide) { hidden = hide; }

void FormatData::setFormatted(bool state) { formatted = state; }

int FormatData::getFormatCounts() const { return formats.count(); }

int FormatData::hiddenFormatsCount() const { return hiddenFormats.count(); }

const QString &FormatData::getLinkUrl(int key) const
{
    // Avoid dereferencing a null pointer when the key is missing.
    static const QString empty;
    const QString *value = linkMap.value(key, nullptr);
    return value ? *value : empty;
}

const QString *FormatData::getHiddenLinkUrl(int key) const
{
    return linkMapHidden.value(key, nullptr);
}

const QString *FormatData::getHiddenTitle(int key) const
{
    return linkTitleMapHidden.value(key, nullptr);
}

const QString *FormatData::getLinkUrlFromTitleStart(int key) const
{
    return linkUrlMap.value(key, nullptr);
}

const QString *FormatData::getLinkTitleFromTitleStart(int key) const
{
    return linkTitleMap.value(key, nullptr);
}

void FormatData::insertLinkText(int key, QString *linkText)
{
    linkMap.insert(key, linkText);
}

void FormatData::setMaskSize(const int size) { mask.resize(size); }

const QBitArray &FormatData::getMask() const { return mask; }

QString FormatData::getOriginalText() const { return originalText; }

void FormatData::setOriginalText(const QString &originalText)
{
    this->originalText = originalText;
}

int FormatData::getCalculatedCursorPos(const int posInBlock)
{
    if (posInBlock >= mask.count())
        return posInBlock;

    int addedCharacters = 0;
    int dynamicPosInBlock = posInBlock;

    for (int i = 0; i <= dynamicPosInBlock; ++i)
    {
        if (dynamicPosInBlock >= mask.count())
            break;

        if (mask[i])
        {
            ++addedCharacters;
            ++dynamicPosInBlock;
        }
    }

    return posInBlock + addedCharacters;
}

void FormatData::addHiddenFormat(const int start, const int end,
                                 const FragmentData::FormatSymbol &status,
                                 const QString &linkUrl,
                                 const QString &linkTitle)
{
    int begin = 0;
    int last = 0;
    int hiddenBits = 0;

    // Clamp start so mask[x] cannot read out of bounds.
    const int scanEnd = qMin(start, mask.count() - 1);
    for (int x = 0; x <= scanEnd; ++x)
    {
        if (mask[x])
            ++hiddenBits;
    }

    begin = start - hiddenBits;
    last = end - hiddenBits;

    if (!linkUrl.isEmpty() && !linkTitle.isEmpty() &&
        status == FragmentData::LINK_TITLE)
    {
        linkMapHidden.insert(begin, new QString(linkUrl));
        linkTitleMapHidden.insert(begin, new QString(linkTitle));
    }

    hiddenFormats.append(new FragmentData(begin, last, status));
}

int FragmentData::getStart() const { return start; }

int FragmentData::getEnd() const { return end; }

FragmentData::FormatSymbol FragmentData::getStatus() const { return status; }

void FragmentData::setStatus(FormatSymbol status) { this->status = status; }

void FormatData::addMaskBit(const int start, const QString &startSymbol,
                            const int end, const QString &endSymbol)
{
    const int startLen = startSymbol.length();
    const int endLen = endSymbol.length();  // fixed: was startSymbol.length()

    for (int i = start; i < (startLen + start); ++i)
    {
        if (i >= 0 && i < mask.size())
            mask.setBit(i, true);
    }

    for (int j = end; j < (endLen + end); ++j)
    {
        if (j >= 0 && j < mask.size())
            mask.setBit(j, true);
    }
}

void FormatData::addMaskBitForLinkContent(const int start, const int end,
                                          const QString &endSymbol)
{
    const int endLen = endSymbol.length();

    for (int i = start; i < (endLen + end); ++i)
    {
        if (i >= 0 && i < mask.size())
            mask.setBit(i, true);
    }
}
