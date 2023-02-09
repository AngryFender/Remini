#include "blockdata.h"

BlockData::BlockData()
{
    start = 0;
    finish = 0;
}

void BlockData::insert(int cursorPosition, QString symbol)
{
    data.insert(cursorPosition,symbol);
}

void BlockData::setRange(int start, int finish)
{
    this->start = start;
    this->finish = finish;
}

void BlockData::setStart(int start)
{
    this->start = start;
}
void BlockData::setFinish(int finish)
{
    this->finish = finish;
}

int BlockData::getStart()
{
    QMutexLocker ml(&mMutex);
    return start;
}

int BlockData::getFinish()
{
    QMutexLocker ml(&mMutex);
    return finish;
}

QMap<int, QString> BlockData::getMap()
{
    return data;
}

