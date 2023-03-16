#include "blockdata.h"

BlockData::BlockData()
{
    status = content;
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
