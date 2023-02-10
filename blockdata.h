#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <QMutex>
#include <QTextBlockUserData>
#include <QVectorIterator>
#include <codebox.h>

class BlockData : public QTextBlockUserData
{
public:
    BlockData();
    int addCodeBox();
    CodeBox *addCodeBoxBack();
    CodeBox *getCodeBox(int id);
    CodeBox *getGuiCodeBox();

    void removeCodeBox(int id);

    void incrementGuiId();
    void clear();

    QVector<CodeBox> *getBoxes();


private:
    QVector<CodeBox> codeBoxes;
    int guiId;


};

#endif // BLOCKDATA_H
