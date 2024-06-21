#include "blockdata.h"

BlockData::BlockData()
{
    status = content;
}

BlockData::BlockData(const statusID status)
{
    this->status = status;
}

BlockData::statusID BlockData::getStatus() const
{
    return status;
}

void BlockData::setStatus(statusID newStatus)
{
    status = newStatus;
}

void BlockData::setHidden(bool hidden)
{
    this->hidden = hidden;
}

bool BlockData::isHidden()
{
    return hidden;
}

void BlockData::setStartBlock(const int start)
{
    this->startBlock = start;
}

void BlockData::setEndBlock(const int end)
{
    this->endBlock = end;
}

int BlockData::getStartBlock() const
{
    return this->startBlock;
}

int BlockData::getEndBlock() const
{
    return this->endBlock;
}
