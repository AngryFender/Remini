#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <QTextBlockUserData>

#define CODEBLOCK_SYMBOL "```"

class BlockData : public QTextBlockUserData
{
public:
    enum statusID{
        start,
        content,
        end
    } ;

    BlockData();
    BlockData(const statusID status);
    statusID getStatus() const;
    void setStatus(statusID newStatus);
    void setHidden(bool hidden);
    bool isHidden();

private:
    statusID status;
    bool hidden;
};

#endif // BLOCKDATA_H
