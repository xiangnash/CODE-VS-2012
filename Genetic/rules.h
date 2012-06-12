#include "data.h"

inline bool out_map(const int x, const int y) {
    return (x < 0 || x >= H || y < 0 || y >= W);
}

inline int get_cost(const int type, const int A) {
    int res;
    int i = 0;
    switch (type) {
        case RAPID: res = 10; i = 0; while (i++ < A) res *= 3;
                    return res;
        case ATTACK: res = 15; i = 0; while (i++ < A) res *= 4;
                     return res;
        case FREEZE: res = 20 * (1+A);
                     return res;
    }
    return 0;
}

inline int get_power(const int type, const int A) {
    int res = 0;
    int i = 0;
    switch (type) {
        case RAPID: return 10; 
        case ATTACK: res = 20; while (i++ < A) res *= 5;
                     return res;
        case FREEZE: return 3*(1+A);
    }
    return 0;
}

inline int get_range_d(const int type, const int A) {
    switch (type) {
        case RAPID: return 3+A;
        case ATTACK: return 2;
        case FREEZE: return 2+A;
    }
    return 0;
}

inline int get_range(const int type, const int A) {
    int d = get_range_d(type, A);
    return (d*d);
}

inline int get_recharge_time(const int type, const int A) {
    switch (type) {
        case RAPID: return 10-(2*A) + 1;
        case ATTACK: return 100   +1;
        case FREEZE: return 20 + 1;
    }
    return 0;
}

inline int get_walk_time(const int s, const int x1, const int y1, const int x2, const int y2) {
//cur_enermys[i].Se, cur_enermys[i].x, cur_enermys[i].y, nxt.x, nxt.y
    int d = 0;
    if (x1 == x2 || y1 == y2)  d = s;
    else d = s * 14 / 10;
    return d;
}

inline bool in_range(const tower_t & tower, const enermy_t & enermy) {
    return (get_range(tower.type, tower.A) >= (tower.x-enermy.x)*(tower.x-enermy.x)+(tower.y-enermy.y)*(tower.y-enermy.y));
}

