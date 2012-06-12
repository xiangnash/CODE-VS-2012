#include "codevs.h"

//#define SOL_DEBUG

bool in_limit_range(int x, int y, int d, const codevs_t &vs) {
    for (int dx = -d; dx <= d; ++dx) {
        for (int dy = -d; dy <= d; ++dy) {
            if (dx*dx + dy*dy != d*d) continue;
            int nx = x + dx, ny = y + dy;
            if (out_map(nx, ny)) continue;
            if (vs.in_path[nx][ny]) return true;
        }
    }
    return false;
}

bool in_ranges(int x, int y, int d, const codevs_t &vs) {
    for (int i = 1; i <= d; ++i) {
        if (in_limit_range(x, y, i, vs)) {
            return true;
        }
    }
    return false;
}


void solve() {
#ifdef SOL_DEBUG
    cerr << "map=" << step << " lvl=" << lvl << endl;
#endif
    codevs_t vs(H, W, basic_map);
    
    vector<tower_t> mytowers;
    mytowers.clear();
    
    vs.find_path();
#ifdef SOL_DEBUG
    cerr << "start " << endl;
#endif
    vs.get_path();
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            if (EMPTY == basic_map[r][c] && !vs.in_path[r][c] && in_limit_range(r, c, 1, vs)) {
                tower_t t(r, c, 2, ATTACK);
                mytowers.push_back(t);
                vs.set_my_towers(mytowers);
#ifdef SOL_DEBUG
            cerr << "r=" << ",c=" << c << endl;
#endif
                if (vs.simulate(Life) == Life) {
                    goto RES;
                }
            }
        }
    }
RES:
    solutions = mytowers;
}

