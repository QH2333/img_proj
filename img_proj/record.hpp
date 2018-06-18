#pragma once
#include "defs.h"

class Record
{
private:
    posItem* srcPos;
    posItem* tarPos;
    void freeList(posItem* head);
    void printList(posItem* head) const;
    void printListFile(posItem* head, FILE* fp) const;
    void copyList(posItem const *srcHead, posItem **desHead);
    int countList(posItem* head) const;
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

void Record::copyList(posItem const *srcHead, posItem **desHead)
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

inline int Record::countList(posItem * head) const
{
    int count = 0;
    posItem* pt = head;
    while (pt)
    {
        count++;
        pt = pt->next;
    }
    return count;
}

Record::Record()
{
    srcPos = nullptr;
    tarPos = nullptr;
}

Record::Record(const Record& rec)
{
    srcPos = nullptr;
    tarPos = nullptr;
    copyList(rec.srcPos, &srcPos);
    copyList(rec.tarPos, &tarPos);
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
    fprintf(fp, "%d\t", countList(srcPos));
    printListFile(srcPos, fp);
    fprintf(fp, "\t");
    fprintf(fp, "%d\t", countList(tarPos));
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