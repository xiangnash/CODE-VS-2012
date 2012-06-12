#include "play.h"
#include <ctime>

//#define DEBUG
//#define GENETICDEBUG

const int MAX_GENETIC_LOOP = 50;
const int MAX_GENETIC_CNT = 100;

struct gen_t {
    int rep[MAXN*MAXN];
    int cost;
    int life;
};

int GTYPE[16];
int GSTRE[16];
int GCOST[16];
int GRANG[16];

int gen_len;
gen_t gens[MAX_GENETIC_CNT];
gen_t ans;
int ids[MAXN][MAXN];
vector<pos_t> emptys;
//FILE *logfp;

bool in_limit_range(int x, int y, int d) {
    for (int dx = -d; dx <= d; ++dx) {
        for (int dy = -d; dy <= d; ++dy) {
            if (dx*dx + dy*dy != d*d) continue;
            int nx = x + dx, ny = y + dy;
            if (out_map(nx, ny)) continue;
            if (in_path[nx][ny]) return true;
        }
    }
    return false;
}

bool in_ranges(int x, int y, int d) {
    return (in_limit_range(x, y, d) || in_limit_range(x, y, d));
}

int make_map(const gen_t &gen) {
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            g[r][c] = basic_map[r][c];
        }
    }
    int cost = 0;
    my_towers.clear();
    for (int i = 0; i < gen_len; ++i) {
        int r = emptys[i].x, c = emptys[i].y;
        if (gen.rep[i] < 15) {
            g[r][c] = TOWER;
            tower_t tower;
            tower.x = r, tower.y = c, tower.type = GTYPE[ gen.rep[i] ], tower.A = GSTRE[ gen.rep[i] ];
            cost += GCOST[ gen.rep[i] ];
            my_towers.push_back(tower);
        }
        else {
            g[r][c] = EMPTY;
        }
    }
    return cost;
}

bool gen_cmp(const gen_t &a, const gen_t &b) {
    if (a.life > b.life) return true;
    else if (a.life < b.life) return false;
    return a.cost < b.cost;
}

bool can_attack(const int x, const int y, const int t) {
    if (t == 15) return false;
    int type, A;
    type = GTYPE[t], A = GSTRE[t];
    int d = GRANG[t];
    for (int dx = -d; dx <= d; ++dx) {
        for (int dy = -d; dy <= d; ++dy) {
            if (dx*dx + dy*dy > d*d || (dx == 0 && dy == 0)) continue;
            int nx = x + dx, ny = y + dy;
            if (out_map(nx, ny)) continue;
            if (in_road[nx][ny]) return true;
        }
    }
    return false;
}

inline void copy_answer(const gen_t &g) {
    ans.cost = g.cost;
    ans.life = g.life;
    for (int i = 0; i < gen_len; ++i) {
        ans.rep[i] = g.rep[i]; //TODO: replace with memcopy?
    }
#ifdef GENETICDEBUG
    cerr << "GET ANS " <<  ans.cost << " " << ans.life << endl;///
#endif
}

void Genetic() {
    emptys.clear();
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            g[r][c] = basic_map[r][c];
            if (EMPTY == basic_map[r][c]) {
                ids[r][c] = emptys.size();
                pos_t p;
                p.x = r, p.y = c;
                emptys.push_back(p);
            }
        }
    }
    
    
    
    //DNA length: emptys.size()
    gen_len = emptys.size();
    
    //TODO: find path initial state(include towers)
    //initial genetic
    find_road();
    get_road();
    for (int i = 0; i < gen_len; ++i) {
        gens[0].rep[i] = 15;
        gens[1].rep[i] = 15;
    }
    for (int i = 0; i < init_towers.size(); ++i) {
        gens[0].rep[ ids[ init_towers[i].x ][ init_towers[i].y ] ] = init_towers[i].type * 5 + init_towers[i].A;
        gens[1].rep[ ids[ init_towers[i].x ][ init_towers[i].y ] ] = 5 + init_towers[i].A;
    }
    gens[0].cost = make_map(gens[0]);
    if (find_path()) {
        get_path();
        if (get_life(Life) == Life) {
            copy_answer(gens[0]);
            return ;
        }
    }
    
    
    int ix = 2;
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            if (EMPTY == g[r][c] && !in_path[r][c] && in_limit_range(r, c, 1)) {
                if (ix > 20) break;
                for (int i = 2; i < ix; ++i) {
                    gens[i].rep[ ids[r][c] ] = 7;
                }
                ++ix;
            }
        }
    }
    
    //no rnodes
    //all tower
    //some rnodes
    for (int ig = ix; ig+4 < MAX_GENETIC_CNT; ig += 4) {
        for (int i = 0; i < gen_len; ++i) {
            int t = rand() % 16;
            gens[ ig+0 ].rep[i] = in_road[ emptys[i].x ][ emptys[i].y ] ? 15 : t;
            gens[ ig+1 ].rep[i] = can_attack(emptys[i].x, emptys[i].y, t) ? t : 15;
            gens[ ig+2 ].rep[i] = in_road[ emptys[i].x ][ emptys[i].y ] ? (t *3 % 7 == 0 ? t : 15) : t;
            gens[ ig+3 ].rep[i] = max(gens[ ig+0 ].rep[i], gens[ ig+1 ].rep[i]);
        }
    }
    for (int i = 0; i < gen_len; ++i) gens[ MAX_GENETIC_CNT-1 ].rep[i] = 15;
    for (int i = 0; i < gen_len; ++i) gens[ MAX_GENETIC_CNT-2 ].rep[i] = 7;
    //initial ans
    bool yes = false;
    copy_answer(gens[ MAX_GENETIC_CNT-1 ]);
    ans.life = 0;
    ans.cost = 10000000;
    
    int pre_ans = 0;
    //loop
    for (int ig_loop_it = 0; true ; ++ig_loop_it) { 
        for (int ig = 0; ig < MAX_GENETIC_CNT; ++ig) {
            //get cost
            gens[ig].cost = make_map(gens[ig]);
            if (is_linked()) {
                find_path();
                get_path();
                int tmp_life = get_life(Life);
                gens[ig].life = tmp_life;
                //clear all unnessary towers
                for (int i = 0; i < my_towers.size(); ++i) {
                    if (!shooted[i]) {
                        gens[ig].rep[ ids[my_towers[i].x][my_towers[i].y] ] = 0;
                    }
                }
                gens[ig].cost = make_map(gens[ig]);
                gens[ig].life = tmp_life;//
                
                
                if (tmp_life == Life) {
                    if (!yes) {
                        yes = true;
                        //record answer
                        copy_answer(gens[ig]);
                    }
                    else {
                        if (gens[ig].cost < ans.cost) {
                            //record answer
                            copy_answer(gens[ig]);
                        }
                    }
                    //a reinforce help
                }
                else {
                    //record maximum life left & reinforce
                    if (!yes && (gens[ig].life > ans.life 
                                || (gens[ig].life == ans.life && gens[ig].cost < ans.cost))) {
                        copy_answer(gens[ig]);
                    }
                }
            }
            else {
                gens[ig].life = min(Life, 2); //FIXME:
            }
        }
        if (yes) {
            bool can_out = true;
            if (pre_ans != ans.cost) can_out = false;
            
            if (step < 3 && ans.cost > 100) can_out = false;
            if (step < 5 && ans.cost > 200) can_out = false;
            if (step < 10 && ans.cost > 300) can_out = false;
            if (step < 20 && ans.cost > 500) can_out = false;
            if (step < 30 && ans.cost > 800) can_out = false;
            if (step < 40 && ans.cost > 1000) can_out = false;
            if (step < 50 && ans.cost > 1400) can_out = false;
            if (step < 60 && ans.cost > 1700) can_out = false;
            if (step < 70 && ans.cost > 2000) can_out = false;
            if (step < 80 && ans.cost > 2300) can_out = false;
            if (step < 90 && ans.cost > 2700) can_out = false;
            if (step < 100 && ans.cost > 3000) can_out = false;
            if (step < 120 && ans.cost > 3400) can_out = false;
            
            if (can_out) {
                if (ans.cost < 100 && ig_loop_it > 1) break;
                if (ans.cost < 200 && ig_loop_it > 2) break;
                if (ans.cost < 300 && ig_loop_it > 5) break;
                if (ans.cost < 500 && ig_loop_it > 20) break;
                if (ans.cost < 800 && ig_loop_it > 30) break;
                if (ans.cost < 1100 && ig_loop_it > 50) break;
                if (ans.cost < 1400 && ig_loop_it > 100) break;
                if (ans.cost < 1700 && ig_loop_it > 150) break;
                if (ans.cost < 2000 && ig_loop_it > 200) break;
                if (ig_loop_it > 400) break;
            }
            if (pre_ans == ans.cost && ans.cost < 3000 && ig_loop_it > 500) break;
            if (pre_ans == ans.cost && ig_loop_it > 750) break;
        } else {
           if (pre_ans == ans.cost && ans.cost < 4000 && ig_loop_it > 750) break;
           if (pre_ans == ans.cost && ig_loop_it > 1000) break;
        } 
        pre_ans = ans.cost;
        
        //genere next generation
        sort(gens, gens+MAX_GENETIC_CNT, gen_cmp);
        int st = MAX_GENETIC_CNT / 2, h_len = gen_len / 2;
        
        gens[0].cost = make_map(gens[0]);
        int ux = st;
        if (find_path()) {
            ux = 20;
            get_path();
            int sx = 20;
            for (int r = 0; r < H; ++r) {
                for (int c = 0; c < W; ++c) {
                    if (EMPTY == g[r][c] && !in_path[r][c] && in_limit_range(r, c, 1)) {
                        if (sx >= st) break;
                        for (int i = 20; i < sx; ++i) {
                            gens[i].rep[ ids[r][c] ] = 7;
                        }
                        ++sx;
                    }
                }
            }
        }
        
        
        //coitus
        for (int ig = st; ig < MAX_GENETIC_CNT; ++ig) {
            //TODO:
            for (int i = 0; i < gen_len; ++i) {
                if (i < h_len) gens[ig].rep[i] = gens[ig-st].rep[i];
                else gens[ig].rep[i] = gens[st-(ig-st)-1].rep[i];
            }
            //
        }
       
        
        for (int i = 0; i < 20; ++i) {
            int li = rand() % st, ri = st + rand() % st;
            for (int ig = 0; ig < ux; ++ig) {
                swap(gens[ig].rep[li], gens[ig].rep[ri]);
            }
            for (int ig = st; ig < 40; ++ig) {
                int a = rand() % 16, b = rand() % 16;
                gens[ig].rep[li] = a;
                gens[ig].rep[ri] = b;
                gens[80-ig].rep[li] = 15-a;
                gens[80-ig].rep[ri] = 15-b;
            }
        }
    }
}

void solve() {
#ifdef DEBUG
    cerr << "map " << step << "-" << lvl << endl;//
#endif
    Genetic();
    vector<tower_t> solutions;
    solutions.clear();
    ans.cost = make_map(ans);
    find_path();
    get_path();
    int T = 0;
    for (int i = 0; i < init_towers.size(); ++i) {
        if (g[ init_towers[i].x ][ init_towers[i].y ] != TOWER) {
            ++T;
        }
    }
#ifdef DEBUG
    cerr << "clear cnt=" << T << endl;//
#endif
    
    cout << T+ my_towers.size() << endl;//
    cout.flush();
    for (int i = 0; i < init_towers.size(); ++i) {
        if (g[ init_towers[i].x ][ init_towers[i].y ] != TOWER) {
            cout << init_towers[i].y << " " << init_towers[i].x << " 0 3" << endl;
        }
    }
    for (int i = 0; i < my_towers.size(); ++i) {
        cout << my_towers[i].y << " " << my_towers[i].x << " " << my_towers[i].A << " " << my_towers[i].type << endl;
    }
}

void init() {
    for (int i = 0; i < 15; ++i) {      
        GTYPE[i] = i / 5;
        GSTRE[i] = i % 5;
        GCOST[i] = get_cost(GTYPE[i], GSTRE[i]);
        GRANG[i] = get_range_d(GTYPE[i], GSTRE[i]);
    }
    GTYPE[15] = 3;
    GCOST[15] = 0;
    GRANG[15] = 0;
}


int main(int argc, char *argv[])
{
    init();
    
    int S;
    scanf("%d", &S);
    for (step = 1; step <= S; ++step) {
        //map
        scanf("%d %d\n", &W, &H);
        defends.clear(); n_defend = 0;
        appears.clear(); n_appear = 0;
        
        for (int r = 0; r < H; ++r) {
            char str[W+3];
            scanf("%s\n", str);
            pos_t p;
            for (int c = 0; c < W; ++c) {
                switch (str[c]) {
                    case '0': basic_map[r][c] = EMPTY; 
                              break;
                    case '1': basic_map[r][c] = OBSTACLE; 
                              break;
#ifdef DEBUG
                              cerr << "OBSTACLE " << r << "," << c << endl;///
#endif
                    case 's': p.x = r, p.y = c; appears.push_back(p);
                              basic_map[r][c] = APPEAR; ++n_appear;
#ifdef DEBUG
                              cerr << "APPEAR " << r << "," << c << endl;///
#endif
                              break;
                    case 'g': p.x = r, p.y = c; defends.push_back(p);
                              basic_map[r][c] = DEFEND; ++n_defend; 
#ifdef DEBUG
                              cerr << "DEFEND " << r << "," << c << endl;///
#endif
                              break; //XXX
                    default: cerr << "error" << endl;
                }
            }
        }
        //init

        int L;
        scanf("%d\n", &L);
        char end_str[11];
        scanf("%s", end_str);
        for (lvl = 1; lvl <= L; ++lvl)  {
            
            int T, E;
            scanf("%d %d %d %d", &Life, &Money, &T, &E); 
            //tower
            n_towers = T;
            init_towers.clear();
            for (int i = 0; i < T; ++i) {
                int x, y, A, ty;
                scanf("%d %d %d %d\n", &y, &x, &A, &ty);
                tower_t tmp_tower;
                tmp_tower.x = x, tmp_tower.y = y, tmp_tower.A = A;
                switch (ty) {
                    case 0: tmp_tower.type = RAPID; break; //rapid
                    case 1: tmp_tower.type = ATTACK; break; //attack
                    case 2: tmp_tower.type = FREEZE; break; //freezing
                }
                init_towers.push_back(tmp_tower);
            }

            //enermy
            n_enermy = E;
            enermys.clear();
            for (int i = 0; i < E; ++i) {
                int x, y, te, le, se;
                scanf("%d %d %d %d %d\n", &y, &x, &te, &le, &se);
                enermy_t enermy;
                enermy.x = x, enermy.y = y, enermy.Te = te, enermy.Le = le, enermy.Se = se;
                enermys.push_back(enermy);
            }

            scanf("%s", end_str);
            
            solve();
            cout.flush();
        }
    }
    return 0;
}

