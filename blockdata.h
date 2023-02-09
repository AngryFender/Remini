#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <QMutex>
#include <QTextBlockUserData>

class BlockData : public QTextBlockUserData
{
public:
    BlockData();
    void insert(int cursorPosition, QString symbol);
    void setRange(int start, int finish);
    void setStart(int start);
    void setFinish(int finish);

    int getStart();
    int getFinish();
    QMap<int, QString> getMap();
private:
    QMap<int, QString> data;
    int start;
    int finish;
    QMutex mMutex;
};

#endif // BLOCKDATA_H
