#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>

#define IMG_WIDTH   64
#define IMG_HEIGHT  64
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

class Template
{
private:
    Block data;
    posItem* srcPos;
    posItem* tarPos;
    void freeList(posItem* head);
    void printList(posItem* head);
public:
    Template(const Block& _block);
    ~Template();
    Block getData() { return data; }
    void addSrcItem(Position* newItem);
    void addTargItem(Position* newItem);
    void printRelation();
};

void Template::freeList(posItem * head)
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

void Template::printList(posItem * head)
{
    posItem* pTemp = head;
    while (pTemp)
    {
        printf("(%d,%d) ", pTemp->data.x, pTemp->data.y);
        pTemp = pTemp->next;
    }
}

Template::Template(const Block& _block):data(_block)
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
    printf("Template info:\n");
    printList(srcPos);
    printf("->");
    printList(tarPos);
    printf("\n");
}

class List
{
public:
    Template data;
    List* next;
    List(const Block& _block) :data(_block), next(nullptr) {}
    void printList();
};

void List::printList()
{
    data.printRelation();
    List* pTemp = next;
    while (pTemp)
    {
        pTemp->data.printRelation();
        pTemp = pTemp->next;
    }
}

bool getMatrix(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], const char* path);
bool addSrcBlockToList(List* head, Block* _block, Position* _pos);
bool addTargBlockToList(List* head, Block* _block, Position* _pos);
Template* locateTemplate(List* head, Block* _block);

int main()
{ 
    uchar Matrix1[IMG_HEIGHT][IMG_WIDTH];
    uchar Matrix2[IMG_HEIGHT][IMG_WIDTH];

    getMatrix(Matrix1, "C:\\Users\\qi_an\\OneDrive\\作业\\项目\\small_sample.bmp");
    getMatrix(Matrix2, "C:\\Users\\qi_an\\OneDrive\\作业\\项目\\small_sample.bmp");

    uchar mTemp[2][2] = { 0,0,0,0 };
    Block bTemp(mTemp);
    List* head = new List(bTemp);

    for (int i = 0; i < IMG_HEIGHT / BLOCK_SIZE; i++)
    {
        for (int j = 0; j < IMG_WIDTH / BLOCK_SIZE; j++)
        {
            Position* pTemp = new Position{ i,j };
            Block* bTemp = new Block(Matrix1, *pTemp);
            addSrcBlockToList(head, bTemp, pTemp);
        }
    }

    for (int i = 0; i < IMG_HEIGHT / BLOCK_SIZE; i++)
    {
        for (int j = 0; j < IMG_WIDTH / BLOCK_SIZE; j++)
        {
            Position* pTemp = new Position{ i,j };
            Block* bTemp = new Block(Matrix2, *pTemp);
            addTargBlockToList(head, bTemp, pTemp);
        }
    }

    int a;
    head->next->printList();

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
    return false;
}

bool operator<(const Block & b1, const Block & b2)
{
    return false;
}

Template* locateTemplate(List* head, Block* _block)
{
    List* pTemp = head->next;
    List* pLast = head;
    while (pTemp)
    {
        if (pTemp->data.getData() == *_block) return &pTemp->data;
        else
        {
            pLast = pTemp;
            pTemp = pTemp->next;
        }
    }
    pLast->next = new List(*_block);
    return &pLast->next->data;
}

bool addSrcBlockToList(List* head, Block* _block, Position* _pos)
{
    locateTemplate(head, _block)->addSrcItem(_pos);
    return true;
}

bool addTargBlockToList(List* head, Block* _block, Position* _pos)
{
    locateTemplate(head, _block)->addTargItem(_pos);
    return true;
}