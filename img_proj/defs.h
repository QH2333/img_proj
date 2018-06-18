#pragma once
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
#define MAT_WIDTH    513
#define MAT_HEIGHT   513
#define BLOCK_HEIGHT 3
#define BLOCK_WIDTH  3

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

struct A
{
    int a;
    int b;
};