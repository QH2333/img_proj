#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <map>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>

#define IMG_WIDTH    512
#define IMG_HEIGHT   512
#define BLOCK_HEIGHT 2
#define BLOCK_WIDTH  1
//#define BLOCK_SIZE  2

using namespace cv;
using namespace std;



//void CtrlBlock()
//{
//    char select;
//    cout << "Plz select block size:" << endl;
//    cout << "1. 1*2" << endl;
//    cout << "2. 2*1" << endl;
//    cout << "3. costom suqare" << endl;
//    select = getch();
//    switch (select)
//    {
//    case '1':static const int BLOCK_HEIGHT = 1; static const int BLOCK_WIDTH = 2; break;
//    case '2':static const int BLOCK_HEIGHT = 2; static const int BLOCK_WIDTH = 1; break;
//    case '3':static const int BLOCK_HEIGHT = select - '0'; static const int BLOCK_WIDTH = select - '0'; break;
//    }
//}

typedef struct _position
{
    int x;
    int y;
}Position;

struct posItem
{
    Position data;
    posItem* next;
};

class Block
{
private:
    uchar data[BLOCK_HEIGHT][BLOCK_WIDTH];
    Position pos;
public:
    Block(const uchar _matrix[IMG_WIDTH][IMG_HEIGHT], const Position _pos);
    Block(const uchar _matrix[BLOCK_HEIGHT][BLOCK_WIDTH]);
    Block(const Block& _block);
    void printBlock() const;
    void printBlockFile(FILE* fp) const;
    uchar getVal(int x, int y)const { return data[x][y]; }
    Block& operator=(const Block& _block);
    friend bool operator==(const Block & b1, const Block & b2);
    friend bool operator>(const Block & b1, const Block & b2);
    friend bool operator<(const Block & b1, const Block & b2);
};

Block::Block(const uchar _matrix[IMG_WIDTH][IMG_HEIGHT], const Position _pos)
{
    Position basePos = { _pos.x * BLOCK_HEIGHT ,_pos.y * BLOCK_WIDTH };
    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            data[i][j] = _matrix[basePos.x + i][basePos.y + j];
        }
    }
}

Block::Block(const uchar _matrix[BLOCK_HEIGHT][BLOCK_WIDTH])
{
    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            data[i][j] = _matrix[i][j];
        }
    }
}

Block::Block(const Block& _block)
{
    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            data[i][j] = _block.getVal(i, j);
        }
    }
}

Block& Block::operator=(const Block& _block)
{
    if (this == &_block) return *this;
    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            data[i][j] = _block.getVal(i, j);
        }
    }
    return *this;
}

void Block::printBlock() const
{
    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            printf("%d ", getVal(i, j));
        }
    }
}

void Block::printBlockFile(FILE* fp) const
{
    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            fprintf(fp, "%d ", getVal(i, j));
        }
    }
}

class Record
{
private:
    posItem * srcPos;
    posItem* tarPos;
    void freeList(posItem* head);
    void printList(posItem* head) const;
    void printListFile(posItem* head, FILE* fp) const;
public:
    Record();
    Record(const Record& rec);
    ~Record();
    void addSrcItem(Position* newItem);
    void addTargItem(Position* newItem);
    void printRelation() const;
    void printRelationFile(FILE* fp) const;
    bool isUnique();
    int getNumOfPoint();
};

void Record::freeList(posItem * head)
{
    if (head)
    {
        posItem *p1 = head;
        posItem *p2 = head->next;
        while (p2)
        {
            p1 = p2;
            p2 = p2->next;
            delete p1;
        }
    }
}

void Record::printList(posItem * head) const
{
    posItem* pTemp = head;
    while (pTemp)
    {
        printf("(%d,%d) ", pTemp->data.x, pTemp->data.y);
        pTemp = pTemp->next;
    }
}

void Record::printListFile(posItem* head, FILE* fp) const
{
    posItem* pTemp = head;
    while (pTemp)
    {
        fprintf(fp, "(%d,%d) ", pTemp->data.x, pTemp->data.y);
        pTemp = pTemp->next;
    }
}

Record::Record()
{
    srcPos = nullptr;
    tarPos = nullptr;
}

Record::~Record()
{
    freeList(srcPos);
    freeList(tarPos);
}

void Record::addSrcItem(Position * newItem)
{
    posItem* insert = new posItem();
    insert->data = *newItem;
    insert->next = srcPos;
    srcPos = insert;
}

void Record::addTargItem(Position * newItem)
{
    posItem* insert = new posItem();
    insert->data = *newItem;
    insert->next = tarPos;
    tarPos = insert;
}

void Record::printRelation() const
{
    printList(srcPos);
    printf("->");
    printList(tarPos);
}

void Record::printRelationFile(FILE* fp) const
{
    printListFile(srcPos, fp);
    fprintf(fp, "->");
    printListFile(tarPos, fp);
}

bool Record::isUnique()
{
    if (srcPos->next == nullptr) return true;
    else return false;
}

int Record::getNumOfPoint()
{
    int count = 0;
    posItem* pTemp = srcPos;
    while (pTemp)
    {
        count++;
        pTemp = pTemp->next;
    }
    return count;
}

bool getMatrix(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], const char* path);
int addSrcBlockToList(map<Block, Record> &_map, Block* _block, Position* _pos);
int addTargBlockToList(map<Block, Record> &_map, Block* _block, Position* _pos);
void Randomize(uchar Matrix[IMG_HEIGHT][IMG_WIDTH]);
void swapBlock(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], int x1, int y1, int x2, int y2);
void copyList(posItem const *srcHead, posItem **desHead);
void print(map<Block, Record> &_map);
void printFile(map<Block, Record> &_map);
void count(map<Block, Record> &_map);

Record::Record(const Record& rec)
{
    srcPos = nullptr;
    tarPos = nullptr;
    copyList(rec.srcPos, &srcPos);
    copyList(rec.tarPos, &tarPos);
}

int main()
{
    //CtrlBlock();
    uchar Matrix1[IMG_HEIGHT][IMG_WIDTH];
    uchar Matrix2[IMG_HEIGHT][IMG_WIDTH];

    getMatrix(Matrix1, "sample.bmp");
    getMatrix(Matrix2, "sample.bmp");

    Randomize(Matrix2);

    map<Block, Record> blockMap;

    uchar mTemp[2][1] = { 0,0 };
    Block bTemp(mTemp);

    for (int i = 0; i < IMG_HEIGHT / BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j < IMG_WIDTH / BLOCK_WIDTH; j++)
        {
            Position* pTemp = new Position{ i,j };
            Block* bTemp = new Block(Matrix1, *pTemp);
            addSrcBlockToList(blockMap, bTemp, pTemp);
        }
    }

    for (int i = 0; i < IMG_HEIGHT / BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j < IMG_WIDTH / BLOCK_WIDTH; j++)
        {
            Position* pTemp = new Position{ i,j };
            Block* bTemp = new Block(Matrix2, *pTemp);
            addTargBlockToList(blockMap, bTemp, pTemp);
        }
    }

    //print(blockMap);
    printFile(blockMap);
    count(blockMap);
    system("pause");
    return 0;
}

bool getMatrix(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], const char* path)
{
    Mat img = imread(path, 0);
    uchar* data;
    for (int i = 0; i < IMG_HEIGHT - 1; i++)
    {
        data = img.ptr<uchar>(i);
        for (int j = 0; j < IMG_WIDTH - 1; j++)
        {
            Matrix[i][j] = data[j];
        }
    }
    return true;
}

bool operator==(const Block & b1, const Block & b2)
{
    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            if (b1.getVal(i, j) != b2.getVal(i, j)) return false;
        }
    }
    return true;
}

bool newEq(const Block & b1, const Block & b2)
{
    int matchCount = 0;
    int matchFlag[BLOCK_HEIGHT][BLOCK_WIDTH] = { 0 };//标记的二维数组
    for (int i = 0; i < BLOCK_HEIGHT; i++)
        for (int j = 0; j < BLOCK_WIDTH; j++)
            for (int p = 0; i < BLOCK_HEIGHT; i++)
                for (int q = 0; j < BLOCK_WIDTH; j++)
                {
                    if (b1.getVal(i, j) == b2.getVal(p, q) && !matchFlag[p][q])//数值相同 而且 没有被认为相同过
                    {
                        matchCount++;
                        matchFlag[p][q] = 1;
                        break;//防止把后面相同的都一起标记了
                    }
                }

    if (BLOCK_HEIGHT*BLOCK_WIDTH == matchCount)
        return true;
}

bool operator>(const Block & b1, const Block & b2)
{
    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            if (b1.getVal(i, j) < b2.getVal(i, j)) return false;
            if (b1.getVal(i, j) > b2.getVal(i, j)) return true;
        }
    }
    return false;
}

bool operator<(const Block & b1, const Block & b2)
{

    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            if (b1.getVal(i, j) > b2.getVal(i, j)) return false;
            if (b1.getVal(i, j) < b2.getVal(i, j)) return true;
        }
    }
    return false;
}

int addSrcBlockToList(map<Block, Record> &_map, Block* _block, Position* _pos)
{
    map < Block, Record > ::iterator iter;
    Record* _templ;
    iter = _map.find(*_block);
    if (iter == _map.end())
    {
        _templ = new Record;
        _templ->addSrcItem(_pos);
        _map.insert(pair<Block, Record>(*_block, *_templ));
        return 1;
    }
    else
    {
        iter->second.addSrcItem(_pos);
        return 0;
    }
}

int addTargBlockToList(map<Block, Record> &_map, Block* _block, Position* _pos)
{
    map < Block, Record > ::iterator iter;
    Record* _templ;
    iter = _map.find(*_block);
    if (iter == _map.end())
    {
        cout << "ERROR:Mismatched block detected!" << endl;
        cout << "Block info:";
        _block->printBlock();
        cout << endl;
        cout << "Pos info:" << _pos->x << "," << _pos->y << endl;
        return 1;
    }
    else
    {
        iter->second.addTargItem(_pos);
        return 0;
    }
}

void Randomize(uchar Matrix[IMG_HEIGHT][IMG_WIDTH])
{
    int j;
    int xBlocks = IMG_HEIGHT / BLOCK_HEIGHT;
    int yBlocks = IMG_WIDTH / BLOCK_WIDTH;
    for (int i = 0; i < xBlocks * yBlocks; i++)
    {
        j = i + (double)rand() / (RAND_MAX - 0) * (xBlocks * yBlocks - 1 - i);
        swapBlock(Matrix, i / yBlocks, i % yBlocks, j / yBlocks, j % yBlocks);
    }
}

void swapBlock(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], int x1, int y1, int x2, int y2)
{
    uchar temp;
    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            temp = Matrix[x1 * BLOCK_HEIGHT + i][y1 * BLOCK_WIDTH + j];
            Matrix[x1 * BLOCK_HEIGHT + i][y1 * BLOCK_WIDTH + j] = Matrix[x2 * BLOCK_HEIGHT + i][y2 * BLOCK_WIDTH + j];
            Matrix[x2 * BLOCK_HEIGHT + i][y2 * BLOCK_WIDTH + j] = temp;
        }
    }
}

void copyList(posItem const *srcHead, posItem **desHead)
{
    const posItem* p1 = srcHead;
    posItem* p2 = *desHead;
    posItem* newDesHead = p2;
    if (p1)
    {
        p2 = new posItem;
        newDesHead = p2;
        p2->next = nullptr;
        p2->data = p1->data;
        p1 = p1->next;
    }
    while (p1)
    {
        p2->next = new posItem;
        p2->next->next = nullptr;
        p2 = p2->next;
        p2->data = p1->data;
        p1 = p1->next;
    }
    *desHead = newDesHead;
}

void print(map<Block, Record> &_map)
{
    map < Block, Record > ::iterator iter;
    for (iter = _map.begin(); iter != _map.end(); iter++)
    {
        printf("Block info:");
        iter->first.printBlock();
        printf("\n");
        iter->second.printRelation();
        printf("\n");
    }
}

void printFile(map<Block, Record> &_map)
{
    FILE* fp = fopen("data.txt", "w");
    map < Block, Record > ::iterator iter;
    for (iter = _map.begin(); iter != _map.end(); iter++)
    {
        fprintf(fp, "Block info:");
        iter->first.printBlockFile(fp);
        fprintf(fp, "\n");
        iter->second.printRelationFile(fp);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void count(map<Block, Record> &_map)
{
    long varieties = 0;
    long uniqueBlocks = 0;
    long equalBlocks = 0;
    map < Block, Record > ::iterator iter;
    for (iter = _map.begin(); iter != _map.end(); iter++)
    {
        varieties++;
        if (iter->second.isUnique()) uniqueBlocks++;
        else equalBlocks += iter->second.getNumOfPoint();
    }
    printf("方块种类：%ld\n", varieties);
    printf("unique块数量：%ld\n", uniqueBlocks);
    printf("equal块数量：%ld\n", equalBlocks);
}