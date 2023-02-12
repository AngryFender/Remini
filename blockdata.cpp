#include "blockdata.h"

BlockData::BlockData()
{
    status = content;
    show = false;
}

BlockData::statusID BlockData::getStatus() const
{
    return status;
}

void BlockData::setStatus(statusID newStatus)
{
    status = newStatus;
}

bool BlockData::isShowing()
{
    return show;
}

void BlockData::showNow(bool show)
{
    this->show = show;
}


