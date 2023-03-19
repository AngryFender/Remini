#ifndef BLOCKCONTROLLER_H
#define BLOCKCONTROLLER_H

#define LOWERBLOCKLIMIT 5
#define UPPERBLOCKLIMIT 5

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
    int getSavedUpperBlockNumber() const;
    void setSavedUpperBlockNumber(int newSavedUpperBlockNumber);

    int getSavedLowerBlockNumber() const;
    void setSavedLowerBlockNumber(int newSavedLowerBlockNumber);

private:
    int upperBlockNumber;
    int lowerBlockNumber;
    int savedUpperBlockNumber;
    int savedLowerBlockNumber;
    double savedScrollValue;
    bool scrollingDown;
};

#endif // BLOCKCONTROLLER_H
