#pragma once
#include "defs.h"

class Block
{
private:
    uchar data[BLOCK_HEIGHT][BLOCK_WIDTH];
    Position pos;
public:
    Block() {}
    Block(const uchar _matrix[MAT_WIDTH][MAT_HEIGHT], const Position _pos);
    Block(const uchar _matrix[BLOCK_HEIGHT][BLOCK_WIDTH]);
    Block(const Block& _block);
    void updateBlock(const uchar _matrix[MAT_WIDTH][MAT_HEIGHT], const Position _pos);
    void saveBlockToImg(uchar _matrix[MAT_WIDTH][MAT_HEIGHT], const Position _pos);
    void printBlock() const;
    void printBlockFile(FILE* fp) const;
    uchar getVal(int x, int y)const { return data[x][y]; }
    void setVal(int x, int y, int _data) { data[x][y] = _data; }
    Block& operator=(const Block& _block);
    friend bool operator==(const Block & b1, const Block & b2);
    friend bool operator>(const Block & b1, const Block & b2);
    friend bool operator<(const Block & b1, const Block & b2);
};

Block::Block(const uchar _matrix[MAT_WIDTH][MAT_HEIGHT], const Position _pos)
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

void Block::updateBlock(const uchar _matrix[MAT_WIDTH][MAT_HEIGHT], const Position _pos)
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

void Block::saveBlockToImg(uchar _matrix[MAT_WIDTH][MAT_HEIGHT], const Position _pos)
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
    fprintf(fp, "(%d", getVal(0, 0));
    for (int j = 1; j <BLOCK_WIDTH; j++)
    {
        fprintf(fp, ",%d", getVal(1, j));
    }
    for (int i = 1; i <BLOCK_HEIGHT; i++)
    {
        for (int j = 0; j <BLOCK_WIDTH; j++)
        {
            fprintf(fp, ",%d", getVal(i, j));
        }
    }
    fprintf(fp, ")");
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
