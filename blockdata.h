#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <QMutex>
#include <QTextBlockUserData>
#include <QVectorIterator>

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
