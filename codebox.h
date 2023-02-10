#ifndef CODEBOX_H
#define CODEBOX_H


class CodeBox
{
public:
    CodeBox(int id);
    void setStart(int start);
    void setFinish(int finish);
    void setId(int id);


    int getStart();
    int getFinish();
    int getId();
 private:
    int start;
    int finish;
    int id;
};

#endif // CODEBOX_H
