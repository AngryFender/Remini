#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <QMutex>
#include <QTextBlockUserData>
#include <QVectorIterator>
#include <codebox.h>

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
    bool isShowing();
    void showNow(bool show);

private:
    statusID status;
    bool show;
};

#endif // BLOCKDATA_H
