#define _CRT_SECURE_NO_WARNINGS
#include "block.hpp"
#include "record.hpp"

struct Relation//将密钥中的每一位都与一个block对应起来，用于解密图像
{
    int keyID;
    Block blockInfo;
};

int* keyGen();//生成密钥
bool compareKey(const A& a1, const A& a2) { return (a1.a > a2.a); }//比较密钥的位，在生成密钥时用作排序函数的回调函数
bool getMatrix(uchar Matrix[MAT_HEIGHT][MAT_WIDTH], const char* path);//将图像读取至Matrix
int addSrcBlockToList(map<Block, Record> &_map, Block* _block, Position* _pos);//向关系表中添加一个iS的块
int addTargBlockToList(map<Block, Record> &_map, Block* _block, Position* _pos);//向关系表中添加一个iE的块
void RandomizePic(uchar Matrix[MAT_HEIGHT][MAT_WIDTH]);//随机化Matrix
void encryptImg(uchar eMatrix[MAT_HEIGHT][MAT_WIDTH], uchar sMatrix[MAT_HEIGHT][MAT_WIDTH], int* key);//使用key将sMatrix加密为eMatrix
void decryptImg(uchar sMatrix[MAT_HEIGHT][MAT_WIDTH], uchar eMatrix[MAT_HEIGHT][MAT_WIDTH], int* key);//使用key将eMatrix解密为sMatrix
Relation* genRelation(uchar Matrix[MAT_HEIGHT][MAT_WIDTH], int* key);//生成关系，用于解密图像
bool compareRelation(const Relation& r1, const Relation& r2) { return (r1.keyID < r2.keyID); }//比较关系中每一个Block的密钥位，在解密时用作排序函数的回调函数
void swapBlock(uchar Matrix[MAT_HEIGHT][MAT_WIDTH], int x1, int y1, int x2, int y2);//交换Matrix中两个坐标上的Block
void print(map<Block, Record> &_map);//打印信息
void printFile(map<Block, Record> &_map, const char* path);//打印信息至文件
void count(map<Block, Record> &_map, const char* path);//计数

int main()
{
    uchar Matrix1[MAT_HEIGHT][MAT_WIDTH];
    uchar Matrix2[MAT_HEIGHT][MAT_WIDTH];

    char* imgPath = new char[100];
    char* dataPath = new char[100];

    for (int i = 1; i <= 12; i++)
    {
        sprintf(imgPath, "test_images\\CImg%d.bmp", i);
        sprintf(dataPath, "data\\CImg%d with %dx%d partition.txt", i, BLOCK_HEIGHT, BLOCK_WIDTH);
        
        getMatrix(Matrix1, imgPath);
        int* key = keyGen();
        encryptImg(Matrix2, Matrix1, key);

        map<Block, Record> blockMap;

        uchar mTemp[BLOCK_HEIGHT][BLOCK_WIDTH] = { 0 };
        Block bTemp(mTemp);

        for (int i = 0; i < MAT_HEIGHT / BLOCK_HEIGHT; i++)
        {
            for (int j = 0; j < MAT_WIDTH / BLOCK_WIDTH; j++)
            {
                Position* pTemp = new Position{ i,j };
                Block* bTemp = new Block(Matrix1, *pTemp);
                addSrcBlockToList(blockMap, bTemp, pTemp);
            }
        }

        for (int i = 0; i < MAT_HEIGHT / BLOCK_HEIGHT; i++)
        {
            for (int j = 0; j < MAT_WIDTH / BLOCK_WIDTH; j++)
            {
                Position* pTemp = new Position{ i,j };
                Block* bTemp = new Block(Matrix2, *pTemp);
                addTargBlockToList(blockMap, bTemp, pTemp);
            }
        }
        printFile(blockMap, dataPath);
        sprintf(dataPath, "data\\Block distribution with %dx%d partition.txt", BLOCK_HEIGHT, BLOCK_WIDTH);
        count(blockMap, dataPath);
        delete[] key;
    }
    delete[] imgPath;
    delete[] dataPath;
    system("pause");
    return 0;
}

int* keyGen()
{
    A* kTemp = new A[MAT_HEIGHT / BLOCK_HEIGHT * MAT_WIDTH / BLOCK_WIDTH];
    for (int i = 0; i < MAT_HEIGHT / BLOCK_HEIGHT * MAT_WIDTH / BLOCK_WIDTH; i++)
    {
        kTemp[i].a = rand();
        kTemp[i].b = i;
    }
    sort(kTemp, kTemp + MAT_HEIGHT / BLOCK_HEIGHT * MAT_WIDTH / BLOCK_WIDTH - 1, compareKey);
    int* key = new int[MAT_HEIGHT / BLOCK_HEIGHT * MAT_WIDTH / BLOCK_WIDTH];
    for (int i = 0; i < MAT_HEIGHT / BLOCK_HEIGHT * MAT_WIDTH / BLOCK_WIDTH; i++)
    {
        key[i] = kTemp[i].b;
    }
    delete[] kTemp;
    return key;
}


bool getMatrix(uchar Matrix[MAT_HEIGHT][MAT_WIDTH], const char* path)
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

void RandomizePic(uchar Matrix[MAT_HEIGHT][MAT_WIDTH])
{
    int j;
    int xBlocks = MAT_HEIGHT / BLOCK_HEIGHT;
    int yBlocks = MAT_WIDTH / BLOCK_WIDTH;
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

void swapBlock(uchar Matrix[MAT_HEIGHT][MAT_WIDTH], int x1, int y1, int x2, int y2)
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

void printFile(map<Block, Record> &_map, const char* path)
{
    FILE* fp = fopen(path, "w");
    map < Block, Record > ::iterator iter;
    fprintf(fp, "图像块\t个数\t位置\t个数\t位置\n");
    for (iter = _map.begin(); iter != _map.end(); iter++)
    {
        iter->first.printBlockFile(fp);
        fprintf(fp, "\t");

        iter->second.printRelationFile(fp);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void count(map<Block, Record> &_map, const char* path)
{
    FILE* fp = fopen(path, "a");
    int pointCount;
    long varieties = 0;
    long uniqueBlocks = 0;
    long equalBlocks = 0;
    long distribution[13] = {0};
    map < Block, Record > ::iterator iter;
    for (iter = _map.begin(); iter != _map.end(); iter++)
    {
        varieties++;
        pointCount = iter->second.getNumOfPoint();
        if (iter->second.isUnique()) uniqueBlocks++; else equalBlocks += pointCount;
        if (pointCount <= 13) distribution[pointCount - 1]++;
    }
    for (int i = 0; i < 12; i++)
    {
        fprintf(fp, "%d\t", distribution[i]);
    }
    fprintf(fp, "%d", distribution[12]);
    fprintf(fp, "\n");
    fclose(fp);
    printf("方块种类：%ld\n", varieties);
    printf("unique块数量：%ld\n", uniqueBlocks);
    printf("equal块数量：%ld\n", equalBlocks);
}

void encryptImg(uchar eMatrix[MAT_HEIGHT][MAT_WIDTH], uchar sMatrix[MAT_HEIGHT][MAT_WIDTH], int* key)
{
    Position temp;
    Relation* relat = genRelation(sMatrix, key);
    sort(relat, relat + MAT_HEIGHT / BLOCK_HEIGHT * MAT_WIDTH / BLOCK_WIDTH, compareRelation);
    for (int i = 0; i < MAT_HEIGHT / BLOCK_HEIGHT; i++)
        for (int j = 0; j < MAT_WIDTH / BLOCK_WIDTH; j++)
        {
            temp.x = i;
            temp.y = j;
            relat[i * MAT_WIDTH / BLOCK_WIDTH + j].blockInfo.saveBlockToImg(eMatrix, temp);
        }
    delete[] relat;
}

Relation* genRelation(uchar Matrix[MAT_HEIGHT][MAT_WIDTH], int* key)
{
    Relation* relat = new Relation[MAT_HEIGHT / BLOCK_HEIGHT * MAT_WIDTH / BLOCK_WIDTH];
    Position temp;
    for (int i = 0; i < MAT_HEIGHT / BLOCK_HEIGHT; i++)
        for (int j = 0; j < MAT_WIDTH / BLOCK_WIDTH; j++)
        {
            temp.x = i;
            temp.y = j;
            relat[i * MAT_WIDTH / BLOCK_WIDTH + j].blockInfo.updateBlock(Matrix, temp);
            relat[i * MAT_WIDTH / BLOCK_WIDTH + j].keyID = key[i * MAT_WIDTH / BLOCK_WIDTH + j];
        }
    return relat;
}

void decryptImg(uchar sMatrix[MAT_HEIGHT][MAT_WIDTH], uchar eMatrix[MAT_HEIGHT][MAT_WIDTH], int* key)
{
    Position pTemp;
    Block bTemp;
    for (int i = 0; i < MAT_HEIGHT / BLOCK_HEIGHT * MAT_WIDTH / BLOCK_WIDTH; i++)
    {
        pTemp.x = key[i] / (MAT_HEIGHT / BLOCK_HEIGHT);
        pTemp.y = key[i] % (MAT_HEIGHT / BLOCK_HEIGHT);
        bTemp.updateBlock(eMatrix, pTemp);
        pTemp.x = i / (MAT_HEIGHT / BLOCK_HEIGHT);
        pTemp.y = i % (MAT_HEIGHT / BLOCK_HEIGHT);
        bTemp.saveBlockToImg(sMatrix, pTemp);
    }
}