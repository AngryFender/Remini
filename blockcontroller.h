#ifndef BLOCKCONTROLLER_H
#define BLOCKCONTROLLER_H

#define LOWERBLOCKLIMIT 10
#define UPPERBLOCKLIMIT 10

class BlockController
{
public:
    BlockController();

    int getLowerBlockNumber() const;
    void setLowerBlockNumber(int lowerBlockNumber);

    int getUpperBlockNumber() const;
    void setUpperBlockNumber(int blockCount);

    void scrollUpdated(int blockCount, int lastBlockNumber, double percent);

    bool isScrollingDown();
private:
    int upperBlockNumber;
    int lowerBlockNumber;
    double savedScrollValue;
    bool scrollingDown;
};

#endif // BLOCKCONTROLLER_H
