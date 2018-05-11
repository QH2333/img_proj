#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <map>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>

#define IMG_WIDTH   512
#define IMG_HEIGHT  512
#define BLOCK_SIZE  2

using namespace cv;
using namespace std;

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
    uchar data[BLOCK_SIZE][BLOCK_SIZE];
    Position pos;
public:
    Block(const uchar _matrix[IMG_WIDTH][IMG_HEIGHT], const Position _pos);
    Block(const uchar _matrix[BLOCK_SIZE][BLOCK_SIZE]);
    Block(const Block& _block);
    uchar getVal(int x, int y)const { return data[x][y]; }
    Block& operator=(const Block& _block);
    void printBlock() const;
    friend bool operator==(const Block & b1, const Block & b2);
    friend bool operator>(const Block & b1, const Block & b2);
    friend bool operator<(const Block & b1, const Block & b2);
};

Block::Block(const uchar _matrix[IMG_WIDTH][IMG_HEIGHT], const Position _pos)
{
    Position basePos = { _pos.x * 2 ,_pos.y * 2 };
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            data[i][j] = _matrix[basePos.x + i][basePos.y + j];
        }
    }
}

Block::Block(const uchar _matrix[BLOCK_SIZE][BLOCK_SIZE])
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            data[i][j] = _matrix[i][j];
        }
    }
}

Block::Block(const Block& _block)
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            data[i][j] = _block.getVal(i, j);
        }
    }
}

Block& Block::operator=(const Block& _block)
{
    if (this == &_block) return *this;
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            data[i][j] = _block.getVal(i, j);
        }
    }
    return *this;
}

void Block::printBlock() const
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            printf("%d ", getVal(i, j));
        }
    }
}

class Template
{
private:
    posItem* srcPos;
    posItem* tarPos;
    void freeList(posItem* head);
    void printList(posItem* head);
public:
    Template();
    ~Template();
    void addSrcItem(Position* newItem);
    void addTargItem(Position* newItem);
    void printRelation();
};

void Template::freeList(posItem * head)
{
    if (head)
    {
        //posItem *p1 = head;
        //posItem *p2 = head->next;
        //while (p2)
        //{
        //    p1 = p2;
        //    p2 = p2->next;
        //    delete p1;
        //}
    }
}

void Template::printList(posItem * head)
{
    posItem* pTemp = head;
    while (pTemp)
    {
        printf("(%d,%d) ", pTemp->data.x, pTemp->data.y);
        pTemp = pTemp->next;
    }
}

Template::Template()
{
    srcPos = nullptr;
    tarPos = nullptr;
}

Template::~Template()
{
    freeList(srcPos);
    freeList(tarPos);
}

void Template::addSrcItem(Position * newItem)
{
    posItem* insert = new posItem();
    insert->data = *newItem;
    insert->next = nullptr;

    if (srcPos)
    {
        posItem* pTemp = srcPos;
        while (pTemp->next) pTemp = pTemp->next;
        pTemp->next = insert;
        insert->next = nullptr;
    }
    else
    {
        srcPos = insert;
    }
}

void Template::addTargItem(Position * newItem)
{
    posItem* insert = new posItem();
    insert->data = *newItem;
    insert->next = nullptr;

    if (tarPos)
    {
        posItem* pTemp = tarPos;
        while (pTemp->next) pTemp = pTemp->next;
        pTemp->next = insert;
        insert->next = nullptr;
    }
    else
    {
        tarPos = insert;
    }
}

void Template::printRelation()
{
    printList(srcPos);
    printf("->");
    printList(tarPos);
}

bool getMatrix(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], const char* path);
bool addSrcBlockToList(map<Block, Template> &_map, Block* _block, Position* _pos);
bool addTargBlockToList(map<Block, Template> &_map, Block* _block, Position* _pos);
void Randomize(uchar Matrix[IMG_HEIGHT][IMG_WIDTH]);
void swapBlock(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], int x1, int y1, int x2, int y2);

int main()
{
    uchar Matrix1[IMG_HEIGHT][IMG_WIDTH];
    uchar Matrix2[IMG_HEIGHT][IMG_WIDTH];

    getMatrix(Matrix1, "C:\\Users\\qi_an\\OneDrive\\作业\\项目\\sample.bmp");
    getMatrix(Matrix2, "C:\\Users\\qi_an\\OneDrive\\作业\\项目\\sample.bmp");

    Randomize(Matrix2);

    map<Block, Template> blockMap;

    uchar mTemp[2][2] = { 0,0,0,0 };
    Block bTemp(mTemp);

    for (int i = 0; i < IMG_HEIGHT / BLOCK_SIZE; i++)
    {
        for (int j = 0; j < IMG_WIDTH / BLOCK_SIZE; j++)
        {
            Position* pTemp = new Position{ i,j };
            Block* bTemp = new Block(Matrix1, *pTemp);
            addSrcBlockToList(blockMap, bTemp, pTemp);
        }
    }

    for (int i = 0; i < IMG_HEIGHT / BLOCK_SIZE; i++)
    {
        for (int j = 0; j < IMG_WIDTH / BLOCK_SIZE; j++)
        {
            Position* pTemp = new Position{ i,j };
            Block* bTemp = new Block(Matrix2, *pTemp);
            addTargBlockToList(blockMap, bTemp, pTemp);
        }
    }

    map < Block, Template > ::iterator iter;

    for (iter = blockMap.begin(); iter != blockMap.end(); iter++)
    {
        printf("Template info:");
        iter->first.printBlock();
        printf("\n");
        iter->second.printRelation();
        printf("\n");
    }

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
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            if (b1.getVal(i, j) != b2.getVal(i, j)) return false;
        }
    }
    return true;
}

bool operator>(const Block & b1, const Block & b2)
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            if (b1.getVal(i, j) < b2.getVal(i, j)) return false;
            if (b1.getVal(i, j) > b2.getVal(i, j)) return true;
        }
    }
    return false;
}

bool operator<(const Block & b1, const Block & b2)
{

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            if (b1.getVal(i, j) > b2.getVal(i, j)) return false;
            if (b1.getVal(i, j) < b2.getVal(i, j)) return true;
        }
    }
    return false;
}

bool addSrcBlockToList(map<Block, Template> &_map, Block* _block, Position* _pos)
{
    map < Block, Template > ::iterator iter;
    Template* _templ;
    iter = _map.find(*_block);
    if (iter == _map.end())
    {
        _templ = new Template;
        _templ->addSrcItem(_pos);
        _map.insert(pair<Block, Template>(*_block, *_templ));
    }
    else
    {
        iter->second.addSrcItem(_pos);
    }
    return true;
}

bool addTargBlockToList(map<Block, Template> &_map, Block* _block, Position* _pos)
{
    map < Block, Template > ::iterator iter;
    Template* _templ;
    iter = _map.find(*_block);
    if (iter == _map.end())
    {
        _templ = new Template;
        _templ->addTargItem(_pos);
        _map.insert(pair<Block, Template>(*_block, *_templ));
    }
    else
    {
        iter->second.addTargItem(_pos);
    }
    return true;
}

void Randomize(uchar Matrix[IMG_HEIGHT][IMG_WIDTH])
{
    int j;
    int xBlocks = IMG_HEIGHT / BLOCK_SIZE;
    int yBlocks = IMG_WIDTH / BLOCK_SIZE;
    for (int i = 0; i < xBlocks*yBlocks; i++)
    {
        j = i + (double)rand() / (RAND_MAX - 0) * (xBlocks*yBlocks - i);
        swapBlock(Matrix, i / yBlocks, i%yBlocks, j / yBlocks, j%yBlocks);
    }
}

void swapBlock(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], int x1, int y1, int x2, int y2)
{
    uchar temp;
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            temp = Matrix[x1 * 2 + i][y1 * 2 + j];
            Matrix[x1 * 2 + i][y1 * 2 + j] = Matrix[x2 * 2 + i][y2 * 2 + j];
            Matrix[x2 * 2 + i][y2 * 2 + j] = temp;
        }
    }
}