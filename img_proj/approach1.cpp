//#include <iostream>
//#include <cstdlib> 
//using namespace std;
//
//bool compare(int**a, int**b, int x, int y, int p, int t, int z)
//{
//    bool temp = true;
//    for (int i = 0; i < z; i++)
//        for (int j = 0; j < z; j++)
//            if (a[x + i][y + j] != b[p + i][t + j])
//            {
//                temp = false;
//                break;
//            }
//    return temp;
//}
//
//int main()
//{
//    int m, n, z, q = 0;
//    int **a, **b, *c;
//    cout << "input the array number:";
//    cin >> m >> n;
//    cout << "intput the size:";
//    cin >> z;
//    int i, j;
//    a = (int **)malloc(sizeof(int *) * m);
//    for (i = 0; i < m; i++)
//        a[i] = (int *)malloc(sizeof(int)*n);
//    b = (int **)malloc(sizeof(int *) * m);
//    for (i = 0; i < m; i++)
//        b[i] = (int *)malloc(sizeof(int)*n);
//    c = (int *)malloc(sizeof(int) * m * n);
//    for (i = 0; i < m; i++)
//        for (j = 0; j < n; j++)
//            cin >> a[i][j];
//    for (i = 0; i < m; i++)
//        for (j = 0; j < n; j++)
//            cin >> b[i][j];
//    for (i = 0; i < m; i++)
//    {
//        for (j = 0; j < n; j++)
//            cout << a[i][j] << ' ';
//        cout << endl;
//    }
//
//    for (i = 0; i < m; i++)
//    {
//        for (j = 0; j < n; j++)
//            cout << b[i][j] << ' ';
//        cout << endl;
//    }
//    for (i = 0; i < m; i += z)
//        for (j = 0; j < n; j += z)
//        {
//            for (int t = 0; t < m; t += z)
//                for (int p = 0; p < n; p += z)
//                {
//                    if (compare(a, b, i, j, t, p, z))
//                    {
//                        c[q++] = i;
//                        c[q++] = j;
//                        c[q++] = t;
//                        c[q++] = p;
//                        /*cout << t << c[q-1] << endl;
//                        cout << "found one!" << endl;*/
//                    }
//                }
//        }
//    for (i = 0; i < q; i += 4)
//        cout << c[i] << ',' << c[i + 1] << "->" << c[i + 2] << ',' << c[i + 3] << endl;
//
//    system("pause");
//    return 0;
//}
