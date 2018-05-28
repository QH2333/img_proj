#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <algorithm>
#include <map>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <time.h>

#define IMG_WIDTH    512
#define IMG_HEIGHT   512
#define BLOCK_HEIGHT 2
#define BLOCK_WIDTH  2

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
    uchar data[BLOCK_HEIGHT][BLOCK_WIDTH];
    Position pos;
public:
    Block() {}
    Block(const uchar _matrix[IMG_WIDTH][IMG_HEIGHT], const Position _pos);
    Block(const uchar _matrix[BLOCK_HEIGHT][BLOCK_WIDTH]);
    Block(const Block& _block);
    void updateBlock(const uchar _matrix[IMG_WIDTH][IMG_HEIGHT], const Position _pos);
    void saveBlockToImg(uchar _matrix[IMG_WIDTH][IMG_HEIGHT], const Position _pos);
    void printBlock() const;
    void printBlockFile(FILE* fp) const;
    uchar getVal(int x, int y)const { return data[x][y]; }
    void setVal(int x, int y, int _data) { data[x][y] = _data; }
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

void Block::updateBlock(const uchar _matrix[IMG_WIDTH][IMG_HEIGHT], const Position _pos)
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

void Block::saveBlockToImg(uchar _matrix[IMG_WIDTH][IMG_HEIGHT], const Position _pos)
{
    Position basePos = { _pos.x * BLOCK_HEIGHT ,_pos.y * BLOCK_WIDTH };
    for (int i = 0; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            _matrix[basePos.x + i][basePos.y + j] = data[i][j];
        }
    }
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

struct Relation
{
    int keyID;
    Block blockInfo;
};

struct A
{
    int a;
    int b;
};

int* keyGen();
bool compareKey(const A& a1, const A& a2);
bool getMatrix(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], const char* path);
int addSrcBlockToList(map<Block, Record> &_map, Block* _block, Position* _pos);
int addTargBlockToList(map<Block, Record> &_map, Block* _block, Position* _pos);
void RandomizePic(uchar Matrix[IMG_HEIGHT][IMG_WIDTH]);
void encryptImg(uchar eMatrix[IMG_HEIGHT][IMG_WIDTH], uchar sMatrix[IMG_HEIGHT][IMG_WIDTH], int* key);
void decryptImg(uchar sMatrix[IMG_HEIGHT][IMG_WIDTH], uchar eMatrix[IMG_HEIGHT][IMG_WIDTH], int* key);
Relation* genRelation(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], int* key);
bool compareRelation(const Relation& r1, const Relation& r2);
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
    uchar Matrix1[IMG_HEIGHT][IMG_WIDTH];
    uchar Matrix2[IMG_HEIGHT][IMG_WIDTH];
    uchar Matrix3[IMG_HEIGHT][IMG_WIDTH];
    uchar Matrix4[IMG_HEIGHT][IMG_WIDTH];

    int* key = keyGen();

    //getMatrix(Matrix1, "tiny_sample0.bmp");
    //getMatrix(Matrix2, "tiny_sample0.bmp");

    //getMatrix(Matrix1, "small_sample.bmp");
    //getMatrix(Matrix2, "small_sample.bmp");

    getMatrix(Matrix1, "sample.bmp");
    getMatrix(Matrix2, "sample.bmp");

    //RandomizePic(Matrix2);

    encryptImg(Matrix3, Matrix2, key);
    decryptImg(Matrix4, Matrix3, key);

    map<Block, Record> blockMap;

    uchar mTemp[2][2] = { 0,0 };
    Block bTemp(mTemp);

    for (int i = 0; i < IMG_HEIGHT / BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j < IMG_WIDTH / BLOCK_WIDTH; j++)
        {
            Position* pTemp = new Position{ i,j };
            Block* bTemp = new Block(Matrix2, *pTemp);
            addSrcBlockToList(blockMap, bTemp, pTemp);
        }
    }

    for (int i = 0; i < IMG_HEIGHT / BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j < IMG_WIDTH / BLOCK_WIDTH; j++)
        {
            Position* pTemp = new Position{ i,j };
            Block* bTemp = new Block(Matrix4, *pTemp);
            addTargBlockToList(blockMap, bTemp, pTemp);
        }
    }

    //print(blockMap);
    printFile(blockMap);
    count(blockMap);
    system("pause");
    return 0;
}

int* keyGen()
{
    A* kTemp = new A[IMG_HEIGHT / BLOCK_HEIGHT * IMG_WIDTH / BLOCK_WIDTH];
    for (int i = 0; i < IMG_HEIGHT / BLOCK_HEIGHT * IMG_WIDTH / BLOCK_WIDTH; i++)
    {
        kTemp[i].a = rand();
        kTemp[i].b = i;
    }
    sort(kTemp, kTemp + IMG_HEIGHT / BLOCK_HEIGHT * IMG_WIDTH / BLOCK_WIDTH - 1, compareKey);
    int* key = new int[IMG_HEIGHT / BLOCK_HEIGHT * IMG_WIDTH / BLOCK_WIDTH];
    for (int i = 0; i < IMG_HEIGHT / BLOCK_HEIGHT * IMG_WIDTH / BLOCK_WIDTH; i++)
    {
        key[i] = kTemp[i].b;
    }
    delete[] kTemp;
    return key;
}

bool compareKey(const A& a1, const A& a2)
{
    return (a1.a > a2.a);
}

bool getMatrix(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], const char* path)
{
    Mat img = imread(path, 0);
    uchar* data;
    for (int i = 0; i < IMG_HEIGHT; i++)
    {
        data = img.ptr<uchar>(i);
        for (int j = 0; j < IMG_WIDTH; j++)
        {
            Matrix[i][j] = data[j];
        }
    }
    return true;
}

bool operator==(const Block & b1, const Block & b2)
{
    int sum1 = 0;
    int sum2 = 0;
    for (int i = 0; i < BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j < BLOCK_WIDTH; j++)
        {
            sum1 += b1.getVal(i, j);
            sum2 += b2.getVal(i, j);
        }
    }
    if (sum1 != sum2) return false;
    else
    {
        uchar mat1[BLOCK_HEIGHT*BLOCK_WIDTH];
        uchar mat2[BLOCK_HEIGHT*BLOCK_WIDTH];

        for (int i = 0; i < BLOCK_HEIGHT; i++)
        {
            for (int j = 0; j < BLOCK_WIDTH; j++)
            {
                mat1[i * BLOCK_WIDTH + j] = b1.getVal(i, j);
                mat2[i * BLOCK_WIDTH + j] = b2.getVal(i, j);
            }
        }
        sort(mat1, mat1 + BLOCK_HEIGHT * BLOCK_WIDTH - 1);
        sort(mat2, mat2 + BLOCK_HEIGHT * BLOCK_WIDTH - 1);
        for (int i = 0; i < BLOCK_HEIGHT*BLOCK_WIDTH; i++)
        {
            if (mat1[i] != mat2[i]) return false;
        }
        return true;
    }
}

bool operator>(const Block & b1, const Block & b2)
{
    uchar mat1[BLOCK_HEIGHT*BLOCK_WIDTH];
    uchar mat2[BLOCK_HEIGHT*BLOCK_WIDTH];
    for (int i = 0; i < BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j < BLOCK_WIDTH; j++)
        {
            mat1[i * BLOCK_WIDTH + j] = b1.getVal(i, j);
            mat2[i * BLOCK_WIDTH + j] = b2.getVal(i, j);
        }
    }
    sort(mat1, mat1 + BLOCK_HEIGHT * BLOCK_WIDTH - 1);
    sort(mat2, mat2 + BLOCK_HEIGHT * BLOCK_WIDTH - 1);
    for (int i = 0; i < BLOCK_HEIGHT*BLOCK_WIDTH; i++)
    {
        if (mat1[i] < mat2[i]) return false;
        if (mat1[i] > mat2[i]) return true;
    }
    return false;
}

bool operator<(const Block & b1, const Block & b2)
{
    uchar mat1[BLOCK_HEIGHT*BLOCK_WIDTH];
    uchar mat2[BLOCK_HEIGHT*BLOCK_WIDTH];
    for (int i = 0; i < BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j < BLOCK_WIDTH; j++)
        {
            mat1[i * BLOCK_WIDTH + j] = b1.getVal(i, j);
            mat2[i * BLOCK_WIDTH + j] = b2.getVal(i, j);
        }
    }
    sort(mat1, mat1 + BLOCK_HEIGHT * BLOCK_WIDTH - 1);
    sort(mat2, mat2 + BLOCK_HEIGHT * BLOCK_WIDTH - 1);
    for (int i = 0; i < BLOCK_HEIGHT*BLOCK_WIDTH; i++)
    {
        if (mat1[i] > mat2[i]) return false;
        if (mat1[i] < mat2[i]) return true;
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

void RandomizePic(uchar Matrix[IMG_HEIGHT][IMG_WIDTH])
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

void RandomizeBlock(Block & block)
{
    int j;
    int temp;
    int xPixels = BLOCK_HEIGHT;
    int yPixels = BLOCK_WIDTH;
    for (int i = 0; i < xPixels * yPixels; i++)
    {
        j = i + (double)rand() / (RAND_MAX - 0) * (xPixels * yPixels - 1 - i);
        temp = block.getVal(i / xPixels, i % yPixels);
        block.setVal(i / xPixels, i % yPixels, block.getVal(j / xPixels, j % yPixels));
        block.setVal(j / xPixels, j % yPixels, temp);
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

void encryptImg(uchar eMatrix[IMG_HEIGHT][IMG_WIDTH], uchar sMatrix[IMG_HEIGHT][IMG_WIDTH], int* key)
{
    Position temp;
    Relation* relat = genRelation(sMatrix, key);
    sort(relat, relat + IMG_HEIGHT / BLOCK_HEIGHT * IMG_WIDTH / BLOCK_WIDTH, compareRelation);
    for (int i = 0; i < IMG_HEIGHT / BLOCK_HEIGHT; i++)
        for (int j = 0; j < IMG_WIDTH / BLOCK_WIDTH; j++)
        {
            temp.x = i;
            temp.y = j;
            relat[i * IMG_HEIGHT / BLOCK_HEIGHT + j].blockInfo.saveBlockToImg(eMatrix, temp);
        }
    delete[] relat;
}

bool compareRelation(const Relation& r1, const Relation& r2)
{
    return (r1.keyID < r2.keyID);
}

Relation* genRelation(uchar Matrix[IMG_HEIGHT][IMG_WIDTH], int* key)
{
    Relation* relat = new Relation[IMG_HEIGHT / BLOCK_HEIGHT * IMG_WIDTH / BLOCK_WIDTH];
    Position temp;
    for (int i = 0; i < IMG_HEIGHT / BLOCK_HEIGHT; i++)
        for (int j = 0; j < IMG_WIDTH / BLOCK_WIDTH; j++)
        {
            temp.x = i;
            temp.y = j;
            relat[i * IMG_HEIGHT / BLOCK_HEIGHT + j].blockInfo.updateBlock(Matrix, temp);
            relat[i * IMG_HEIGHT / BLOCK_HEIGHT + j].keyID = key[i * IMG_HEIGHT / BLOCK_HEIGHT + j];
        }
    return relat;
}

void decryptImg(uchar sMatrix[IMG_HEIGHT][IMG_WIDTH], uchar eMatrix[IMG_HEIGHT][IMG_WIDTH], int* key)
{
    Position pTemp;
    Block bTemp;
    for (int i = 0; i < IMG_HEIGHT / BLOCK_HEIGHT * IMG_WIDTH / BLOCK_WIDTH; i++)
    {
        pTemp.x = key[i] / (IMG_HEIGHT / BLOCK_HEIGHT);
        pTemp.y = key[i] % (IMG_HEIGHT / BLOCK_HEIGHT);
        bTemp.updateBlock(eMatrix, pTemp);
        pTemp.x = i / (IMG_HEIGHT / BLOCK_HEIGHT);
        pTemp.y = i % (IMG_HEIGHT / BLOCK_HEIGHT);
        bTemp.saveBlockToImg(sMatrix, pTemp);
    }
}