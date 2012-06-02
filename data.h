#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <queue>
#include <set>
#include <algorithm>
using namespace std;

const int MAXN = 50;
const int MAXN_TOWER = MAXN*MAXN;
const int MAXN_ENERMY = 22;

const int EMPTY = 0;
const int OBSTACLE = 1;
const int APPEAR = 2;
const int DEFEND = 3;
const int ENERMY = 5;
const int TOWER = 7;


const int RAPID = 0;
const int ATTACK = 1;
const int FREEZE = 2;
const int BLANK = 3;

const int DX[] = {0, -1, -1, -1,  0,  1, 1, 1};
const int DY[] = {1,  1,  0, -1, -1, -1, 0, 1};
const int COST[] = {10, 14, 10, 14, 10, 14, 10, 14};

const int CHARGED = 0;
const int RECHARGING = 1;

const int MOVE = 0;
const int FIRE = 1;

const int UNREACHABLE = MAXN * MAXN * MAXN;

struct pos_t {
    int x, y;
    pos_t(int _x, int _y): x(_x), y(_y) {}    
    pos_t(): x(0), y(0) {}    
};

struct tower_t {
    int x, y;
    int type, A;
    tower_t(int _x, int _y, int _A, int _type) : x(_x), y(_y), A(_A), type(_type) {}
    tower_t() {}
};

struct enermy_t {
    int x, y;
    int Te, Le, Se;
//    enermy_t(int _x, int _y, int _Te, int _Le, int _Se) : x(_x), y(_y), Te(_Te), Le(_Le), Se(_Se) {}
    enermy_t() {}
};

int step, lvl;

int W, H;
int basic_map[MAXN][MAXN];
//int n_defend,
//    n_appear,
//    n_enermy,
//    n_towers;
int Money,
    Life;
vector<pos_t> defends,
    appears;
vector<tower_t> init_towers; //we can enforce the init tower
vector<enermy_t> enermys;

vector<tower_t> solutions;

int CDIS[20][20];
int ipc[MAXN][MAXN][8];
int mydis[MAXN][MAXN];
