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
