#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <QTextBlockUserData>

#define CODEBLOCK__START_SYMBOL "```"
#define CODEBLOCK_END_SYMBOL "^```\\r?\\n?"

class BlockData : public QTextBlockUserData
{
public:
    enum statusID{
        start,
        content,
        end
    } ;

    BlockData();
    statusID getStatus() const;
    void setStatus(statusID newStatus);
    void setHidden(bool hidden);
    bool isHidden();

private:
    statusID status;
    bool hidden;
};

#endif // BLOCKDATA_H
