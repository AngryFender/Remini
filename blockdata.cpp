#include "blockdata.h"

BlockData::BlockData()
    : status(content), hidden(false), startBlock(0), endBlock(0)
{
}

BlockData::BlockData(const statusID status)
    : status(status), hidden(false), startBlock(0), endBlock(0)
{
}

BlockData::statusID BlockData::getStatus() const { return status; }

void BlockData::setStatus(statusID newStatus) { status = newStatus; }

void BlockData::setHidden(bool hidden) { this->hidden = hidden; }

bool BlockData::isHidden() const { return hidden; }

void BlockData::setStartBlock(const int start) { startBlock = start; }

void BlockData::setEndBlock(const int end) { endBlock = end; }

int BlockData::getStartBlock() const { return startBlock; }

int BlockData::getEndBlock() const { return endBlock; }
