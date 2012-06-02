/*
 * ./writeAns.out < ../data/map.txt  > ./answer.h
 */
#include <iostream>
#include <cstdio>

using namespace std;

int Ts[250*4];
int ans[100000000][4];

int main()
{
    int T;
    int n = 0, m = 0;
    while (scanf("%d", &T) != EOF) {
        Ts[n] = T;
        for (int i = 0; i < T; ++i) {
            int x, y, A, type;
            scanf("%d %d %d %d", &x, &y, &A, &type);
            ans[m][0] = x, ans[m][1] = y, ans[m][2] = A, ans[m][3] = type;
            ++m;
        }
        ++n;
    }
    cout << "int Ts[] = {";
    for (int i = 0; i < n; ++i) {
        cout << Ts[i] << ",";
    }
    for (int i = n; i < 1000; ++i) {
        cout << 0 << ",";
    }
    cout << "0};" << endl;
    cout << endl;
    
    cout << "int ans[][4] = {" << endl;
    for (int i = 0; i < m; ++i) {
        cout << "{" << ans[i][0] << "," << ans[i][1] << "," << ans[i][2] << "," << ans[i][3] << "}," << endl;
    }
    cout << "};" << endl;
    return 0;
}
