#include "blockcontroller.h"

BlockController::BlockController()
{
    lowerBlockNumber = 0;
    upperBlockNumber = UPPERBLOCKLIMIT;
    savedScrollValue = 0.00;
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
    int blockNumber = static_cast<int>(((double)lastBlockNumber + 1.00) * (percent / 100.0));

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
    }
}

bool BlockController::isScrollingDown()
{
    return scrollingDown;
}
