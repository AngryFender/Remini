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
    statusID getStatus() const;
    void setStatus(statusID newStatus);

private:
    statusID status;
};

#endif // BLOCKDATA_H
