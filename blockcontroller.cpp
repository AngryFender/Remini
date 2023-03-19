#include "blockcontroller.h"

#include <qDebug>

BlockController::BlockController()
{
    lowerBlockNumber = 0;
    upperBlockNumber = UPPERBLOCKLIMIT;
    savedScrollValue = 0.00;
    savedUpperBlockNumber = 0;
    savedLowerBlockNumber = 0;
}

int BlockController::getLowerBlockNumber() const
{
    return lowerBlockNumber;
}

void BlockController::setLowerBlockNumber(int lowerBlockNumber)
{
    this->lowerBlockNumber = lowerBlockNumber;
}

int BlockController::getUpperBlockNumber() const
{
    return upperBlockNumber;
}

void BlockController::setUpperBlockNumber(int blockCount)
{
    if(blockCount < UPPERBLOCKLIMIT){
        this->upperBlockNumber = blockCount;
    }else{
        this->upperBlockNumber = UPPERBLOCKLIMIT;
    }
}

void BlockController::scrollUpdated(int blockCount, int lastBlockNumber, double percent)
{
    int blockNumber = static_cast<int>(((double)lastBlockNumber + 1.00) * (percent / 100.00));
     qDebug()<<"last "<<lastBlockNumber<<" blockcount "<<blockCount<<"selected block "<<blockNumber<<"percent"<<percent;
    if (blockNumber >= blockCount) {
        blockNumber = blockCount - 1; // ensure the block number is within the range of the document
    }

    lowerBlockNumber = blockNumber - LOWERBLOCKLIMIT;
    upperBlockNumber = blockNumber + UPPERBLOCKLIMIT;

    if(lowerBlockNumber<0){
        lowerBlockNumber = 0;
    }

    if(upperBlockNumber >blockNumber){
        upperBlockNumber = blockNumber;
    }

    if(percent > savedScrollValue){
        scrollingDown = true;
        savedScrollValue = percent;
    }else if (percent < savedScrollValue){
        scrollingDown = false;
        savedScrollValue = percent;
    }
}

bool BlockController::isScrollingDown()
{
    return scrollingDown;
}

int BlockController::getSavedUpperBlockNumber() const
{
    return savedUpperBlockNumber;
}

void BlockController::setSavedUpperBlockNumber(int newSavedUpperBlockNumber)
{
    savedUpperBlockNumber = newSavedUpperBlockNumber;
}

int BlockController::getSavedLowerBlockNumber() const
{
    return savedLowerBlockNumber;
}

void BlockController::setSavedLowerBlockNumber(int newSavedLowerBlockNumber)
{
    savedLowerBlockNumber = newSavedLowerBlockNumber;
}
