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

const int EMPTY = 0;
const int OBSTACLE = 1;
const int APPEAR = 2;
const int DEFEND = 3;
const int ENERMY = 5;
const int TOWER = 7;


const int RAPID = 0;
const int ATTACK = 1;
const int FREEZE = 2;

const int DX[] = {0, -1, -1, -1,  0,  1, 1, 1};
const int DY[] = {1,  1,  0, -1, -1, -1, 0, 1};
//const int DY[] = {0, -1, -1, -1,  0,  1, 1, 1};
//const int DX[] = {1,  1,  0, -1, -1, -1, 0, 1};
const int COST[] = {10, 14, 10, 14, 10, 14, 10, 14};

const int CHARGED = 0;
const int RECHARGING = 1;

const int MOVE = 0;
const int FIRE = 1;

const int MAXN = 50;

const int MAXN_TOWER = MAXN*MAXN;
const int MAXN_ENERMY = 22;

struct pos_t {
    int x, y;
    /*pos_t(int _x, int _y): x(_x), y(_y) {}*/    
};

struct tower_t {
    int x, y;
    int type, A;
};

struct enermy_t {
    int x, y;
    int Te, Le, Se;
};

struct queue_t {
    int x, y;
    int t;
    bool operator<(const queue_t &another) const {
        return t > another.t;
    }
};

//for path-cost strategy
struct location_t {
    int x, y;
    int weight;
    //number of path
    //number of enermy's
    //times of life of enermy(can attack)
    int path_dis;
};

int step, lvl;

int W, H;
int n_defend, 
    n_appear,
    n_enermy, n_towers;
int Money, Life;
int g0[MAXN][MAXN], g[MAXN][MAXN], basic_map[MAXN][MAXN];
vector<pos_t> defends, appears;
vector<tower_t> init_towers, my_towers; 
vector<enermy_t> enermys, cur_enermys;

int final_map[MAXN][MAXN];
vector<tower_t> solutions;

vector<int> edge[MAXN*MAXN+10];
pos_t path[MAXN][MAXN];
pos_t road[MAXN][MAXN];

bool in_path[MAXN][MAXN];
int vec[MAXN][MAXN];

bool in_road[MAXN][MAXN];
bool shooted[MAXN*MAXN];
