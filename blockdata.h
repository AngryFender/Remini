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

    void setStartBlock(const int start);
    void setEndBlock(const int end);
    int getStartBlock() const;
    int getEndBlock() const;
private:
    statusID status;
    bool hidden;
    int startBlock;
    int endBlock;
};

#endif // BLOCKDATA_H
