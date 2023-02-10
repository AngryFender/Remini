#include "blockdata.h"

BlockData::BlockData()
{
    guiId = 1;
}

int BlockData::addCodeBox()
{
    int id = codeBoxes.size() +1;
    CodeBox box(id);
    codeBoxes.push_back(box);
    return id;
}

CodeBox *BlockData::addCodeBoxBack()
{
    int id = codeBoxes.size() +1;
    CodeBox box(id);
    codeBoxes.push_back(box);
    return &codeBoxes.back();
}

CodeBox *BlockData::getCodeBox(int id)
{
    for(auto &box: codeBoxes){
        if(id == box.getId()){
            return &box;
        }
    }
    return nullptr;
}

CodeBox *BlockData::getGuiCodeBox()
{
    return getCodeBox(guiId);
}

void BlockData::removeCodeBox(int id)
{

}

void BlockData::incrementGuiId()
{
    if(++guiId){
        guiId = 1;
    }
}

void BlockData::clear()
{
    codeBoxes.clear();
}

QVector<CodeBox> *BlockData::getBoxes()
{
    return &codeBoxes;
}


