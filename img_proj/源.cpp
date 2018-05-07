#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  

using namespace cv;
using namespace std;
int main()
{ 
    Mat img = imread("C:\\Users\\qi_an\\OneDrive\\作业\\项目\\sample.bmp", 0);

    // 等待10000 ms后窗口自动关闭  
    waitKey(10000);
    return 0;
}