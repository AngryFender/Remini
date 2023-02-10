#include "codebox.h"


CodeBox::CodeBox(int id)
{
    this->id = id;
}

void CodeBox::setStart(int start)
{
    this->start = start;
}

void CodeBox::setFinish(int finish)
{
    this->finish = finish;
}

void CodeBox::setId(int id)
{
    this->id = id;
}

int CodeBox::getStart()
{
    return start;
}

int CodeBox::getFinish()
{
    return finish;
}

int CodeBox::getId()
{
    return id;
}
