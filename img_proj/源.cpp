#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  

using namespace cv;
using namespace std;
int main()
{ 
    Mat img = imread("C:\\Users\\qi_an\\OneDrive\\��ҵ\\��Ŀ\\sample.bmp", 0);

    // �ȴ�10000 ms�󴰿��Զ��ر�  
    waitKey(10000);
    return 0;
}