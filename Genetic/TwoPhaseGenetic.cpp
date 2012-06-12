#include "play.h"
#include <ctime>

//#define RESDEBUG
//#define DEBUG
//#define GENETICDEBUG

const int MAX_GENETIC_LOOP = 3;
const int MAX_GENETIC_CNT = 100;

const int DNA_PER = 16;
const int DNA_SKE = 4;
const int PER_MOD = (1<<DNA_SKE) - 1;
const int DNA_MOD = (1<<DNA_PER) - 1;

struct dna_t {
    int str[((MAXN*MAXN) >> DNA_SKE) + 5]; //digit compress, only 0~1 would be represented
    bool valid;
    int len;
    int best;
};

struct gen_t {
    int rep[MAXN*MAXN];
    int cost;
    int life;
};

struct res_t {
    dna_t dna;
    gen_t gen;
    int life, cost;
};

int GTYPE[16];
int GSTRE[16];
int GCOST[16];
int GRANG[16];

int dna_len, num_len;
int gen_len;
gen_t gens[MAX_GENETIC_CNT];
gen_t ans;
int ids[MAXN][MAXN];
vector<pos_t> emptys;
vector<pos_t> dnafires;

dna_t dna[MAX_GENETIC_CNT];
res_t res;

bool dna_cmp(const dna_t &a, const dna_t &b) {
    if (a.valid && !b.valid) return true;
    else if (b.valid && !a.valid) return false;
    if (a.best < b.best) return true;
    else if (a.best > b.best) return false;
    if (a.len <= b.len) return true;
    return false;
}

inline bool is_tower(const dna_t &dna, const int i) {
    return ((dna.str[i>>DNA_SKE] & (1 << (i&PER_MOD))) > 0);
}

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
    int cost = 0;
    my_towers.clear();
    for (int i = 0; i < gen_len; ++i) {
        int r = dnafires[i].x, c = dnafires[i].y;
        g[r][c] = TOWER;
        tower_t tower;
        tower.x = r, tower.y = c, tower.type = GTYPE[ gen.rep[i] ], tower.A = GSTRE[ gen.rep[i] ];
        cost += GCOST[ gen.rep[i] ];
        my_towers.push_back(tower);
    }
    return cost;
}

int init_map(const dna_t &dna) {
    int len = 0;
    for (int i = 0; i < dna_len; ++i) {
        int r = emptys[i].x, c = emptys[i].y;
        if (is_tower(dna, i)) {
            ++len;
            g[r][c] = TOWER;
        }
        else {
            g[r][c] = EMPTY;
        }
    }
    return len;
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
        ans.rep[i] = g.rep[i];
    }
#ifdef GENETICDEBUG
    cerr << "GET ANS " <<  ans.cost << " " << ans.life << endl;///
#endif
}

inline void copy_result(const dna_t &dna) {
    res.cost = ans.cost;
    res.life = ans.life;
    for (int i = 0; i < dna_len; ++i) {
        res.dna.str[i] = dna.str[i];
    }
    res.dna.len = dna.len;
    
    for (int i = 0; i < gen_len; ++i) {
        res.gen.rep[i] = ans.rep[i];
    }
#ifdef GENETICDEBUG
    cerr << "GET RES " <<  res.cost << " " << res.life << endl;///
#endif
}

void init_emptys() {
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
    dna_len = emptys.size();
}

void init_dnafires(const dna_t &dna) {
    dnafires.clear();
    gen_len = 0;
    for (int i = 0; i < dna_len; ++i) {
        if (is_tower(dna, i)) {
            pos_t p;
            p.x = emptys[i].x, p.y = emptys[i].y;
            dnafires.push_back(p);
            ++gen_len;
        }
    }
}

int CDIS[20][20];
    
int placeTowers(dna_t &dna) {
    //find path initial state(include towers)
    init_dnafires(dna);
    
    int mydis[H][W];
    //get distance
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            mydis[r][c] = 10;
        }
    }
    
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            if (in_path[r][c]) {
                for (int dx = -7; dx <= 7; ++dx) {
                    for (int dy = -7; dy <= 7; ++dy) {
                        if (CDIS[dx+10][dy+10] <= 7) {
                            int nx = r + dx, ny = c + dy;
                            if (out_map(nx, ny)) continue;
                            mydis[nx][ny] = min(mydis[nx][ny], CDIS[dx+10][dy+10]);
                        }
                    }
                }
            }
        }
    }
    
    //place tower
    for (int ig = 0; ig+4 < MAX_GENETIC_CNT; ig += 4) {
        for (int i = 0; i < gen_len; ++i) {
            int t = rand() % 15;
            gens[ ig+0 ].rep[i] = can_attack(dnafires[i].x, dnafires[i].y, t) ? 7 : t;
            gens[ ig+1 ].rep[i] = can_attack(dnafires[i].x, dnafires[i].y, t) ? t : 0;
            gens[ ig+2 ].rep[i] = min(gens[ ig+0 ].rep[i], gens[ ig+1 ].rep[i]);
            gens[ ig+3 ].rep[i] = max(gens[ ig+0 ].rep[i], gens[ ig+1 ].rep[i]);
        }
    }
    for (int i = 0; i < gen_len; ++i) gens[ MAX_GENETIC_CNT-1 ].rep[i] = 0;
    for (int i = 0; i < gen_len; ++i) gens[ MAX_GENETIC_CNT-2 ].rep[i] = 7;
    
    for (int i = 0; i < gen_len; i++) {
        int x = dnafires[i].x, y = dnafires[i].y;
        for (int ig = 0; ig < MAX_GENETIC_CNT; ++ig) {
            if (GRANG[ gens[ig].rep[i] ] < mydis[x][y]) {
                gens[ig].rep[i] = 0;
            }
        }
    }
    //initial ans
    bool yes = false;
    copy_answer(gens[ MAX_GENETIC_CNT-1 ]);
    ans.life = 0;
    ans.cost = 10000000;
#ifdef RESDEBUG
    cerr << "Genetic Loop Start " << endl;//
#endif
    
    int pre_cost = 0;
    //loop
    int pre_ans = ans.cost;
    for (int ig_loop_it = 0; true ; ++ig_loop_it) { 
        for (int ig = 0; ig < MAX_GENETIC_CNT; ++ig) {
            //get cost
            gens[ig].cost = make_map(gens[ig]);
            if (yes && gens[ig].cost > ans.cost) {
                gens[ig].life = max(2, Life-2);
                continue;
            }
            int tmp_life = get_life(Life);
            gens[ig].life = tmp_life;
            //clear all unnessary towers
            for (int i = 0; i < my_towers.size(); ++i) {
                if (!shooted[i]) {
                    gens[ig].rep[ ids[my_towers[i].x][my_towers[i].y] ] = 0;
                }
            }
            gens[ig].cost = make_map(gens[ig]);
            gens[ig].life = get_life(Life);//

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
            }
            else {
                //record maximum life left & reinforce
                if (!yes && (gens[ig].life > ans.life 
                            || (gens[ig].life == ans.life && gens[ig].cost < ans.cost))) {
                    copy_answer(gens[ig]);
                }
            } 
        }
#ifdef RESDEBUG
    cerr << "if out? " << ig_loop_it << endl;//
#endif
        if (yes && pre_ans == ans.cost && ans.cost < 3500 && ig_loop_it > 10) break;
        if (pre_ans == ans.cost && ans.cost < 3000 && ig_loop_it > 20) break;
        if (pre_ans == ans.cost && ig_loop_it > 30) break;
        pre_ans = ans.cost;
        //genere next generation
        sort(gens, gens+MAX_GENETIC_CNT, gen_cmp);
        int st = MAX_GENETIC_CNT / 2, h_len = gen_len / 2;

        gens[0].cost = make_map(gens[0]);
        int ux = st;
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
                int a = rand() % 15, b = rand() % 15;
                gens[ig].rep[li] = a;
                gens[ig].rep[ri] = b;
                gens[80-ig].rep[li] = 14-a;
                gens[80-ig].rep[ri] = 14-b;
            }
        }
#ifdef RESDEBUG
    cerr << "coitus end " << ig_loop_it << endl;//
#endif
    }
    return ans.cost;
}


inline void set_dna_str(const int ig, const int ix) {
    dna[ig].str[ix>>DNA_SKE] |= 1 << (ix&PER_MOD);
}

inline void clear_dna_str(const int ig, const int ix) {
    dna[ig].str[ix>>DNA_SKE] &= 0 ^ (1 << (ix&PER_MOD));
}

bool can_ig_loop_end(int rnd) {
    if (res.life != Life) return false;
    if (res.cost < 100 && rnd > 1) return true;
    if (step > 1 && res.cost < 200) return true;
    if (step > 5 && res.cost < 300) return true;
    if (step > 10 && res.cost < 500) return true;
    if (step > 20 && res.cost < 800) return true;
    if (step > 30 && res.cost < 1000) return true;
    if (step > 40 && res.cost < 1200) return true;
    if (step > 50 && res.cost < 1500) return true;
    if (step > 60 && res.cost < 1900) return true;
    if (step > 70 && res.cost < 2200) return true;
    if (step > 80 && res.cost < 2600) return true;
    if (step > 90 && res.cost < 3000) return true;
    if (step > 100 && res.cost < 3200) return true;
    if (step > 120 && res.cost < 4000) return true;
    return false;
}


void DNA() {
#ifdef RESDEBUG
    cerr << "map " << step << "-" << lvl << endl;//
#endif
    init_emptys();
    num_len = dna_len >> DNA_SKE;
    if ((dna_len & PER_MOD) > 0) ++num_len;
    if (lvl > 1) {
        //current map
        for (int i = 0; i < init_towers.size(); ++i) {
            set_dna_str(0, ids[ init_towers[i].x ][ init_towers[i].y ]);
        }
        dna[0].len = init_map(dna[0]);
        init_dnafires(dna[0]);
        for (int i = 0; i < gen_len; ++i) {
            gens[0].rep[i] = 15;
            for (int j = 0; j < init_towers.size(); ++j) {
                if (dnafires[i].x == init_towers[j].x && dnafires[i].y == init_towers[j].y) {
                    gens[0].rep[i] = init_towers[j].type * 5 + init_towers[j].A;
                    break;
                }
            }
        }
        gens[0].cost = make_map(gens[0]);
        if (find_path()) {
            get_path();
            if (get_life(Life) == Life) {
                copy_answer(gens[0]);
                copy_result(dna[0]);
                return ;
            }
        }
    }
    
    find_road();
    get_road();
    
    //init DNA
    for (int ig = 0; ig < MAX_GENETIC_CNT; ig += 2) {
        for (int i = num_len-1; i >= 0; --i) {
            dna[ig].str[i] = rand() & DNA_MOD; 
        }
    }
    
    for (int ig = 1; ig < MAX_GENETIC_CNT; ig += 2) {
        int i = rand() % 3;
        for (int i = 0; i < dna_len; ++i) {
            if (ig*2 < MAX_GENETIC_CNT) { //row
                if (emptys[i].x % ig == 0) {
                    clear_dna_str(ig, i);
                }
                else {
                    if (i) set_dna_str(ig, i);
                    else clear_dna_str(ig, i);
                }
            }
            else { //column
                if (emptys[i].y % ig == 0) {
                    clear_dna_str(ig, i);
                }
                else {
                    if (i) set_dna_str(ig, i);
                    else clear_dna_str(ig, i);
                }
            }
        }
    }
    //dna[0] = crrent_map
    if (find_path()) {
        get_path();
        memset(dna[0].str, 0, sizeof(dna[0].str));
        for (int i = 0; i < dna_len; ++i) {
            if (!in_path[ emptys[i].x ][ emptys[i].y ]) {
                set_dna_str(0, i);
            }
        }
    }
    for (int ig = 0; ig < MAX_GENETIC_CNT; ++ig) {
        dna[ig].valid = false;
        dna[ig].len = 0;
        dna[ig].best = ig * 100000;
    }
    
    //init result
    res.cost = 100000, res.life = 0;
    
    bool have_road = false;
    int pre_res = res.cost;
    for (int ig_loop_it = 0; true; ++ig_loop_it) {
        int valid_cnt = 0;
        for (int ig = 0; ig < MAX_GENETIC_CNT; ++ig) {
            dna[ig].len = init_map(dna[ig]);
            if (is_linked()) {
                find_path();
                ++valid_cnt;
                dna[ig].valid = true; 
                int t_cost = placeTowers(dna[ig]);
                if (ans.life == Life) {
                    dna[ig].best = t_cost;
                    have_road = true;
                }
                else {
                    continue;
                }
                if (ans.life > res.life || (ans.life == res.life && ans.cost < res.cost)) {
                    copy_result(dna[ig]);
                }
            } else {
                dna[ig].valid = false;
            }
        }
        if (have_road && pre_res == res.cost && can_ig_loop_end(ig_loop_it)) break;
        if (have_road && pre_res == res.cost && ig_loop_it > 30) break;
        pre_res = res.cost;
        //generate next generation
        //must be some towers in path
        sort(dna, dna+MAX_GENETIC_CNT, dna_cmp);
        int ix = valid_cnt;
        for (int ig = 0; ig < valid_cnt; ++ig) {
            for (int ss = valid_cnt-1; ss >= 0; --ss) {
                for (int i = num_len-1; i >= 0; --i) {
                    if (ix == MAX_GENETIC_CNT) goto CONJ_IT_END;
                    dna[ix].str[i] = dna[ig].str[i] ^ dna[ss].str[i];
                    ++ix;
                }
            }
        }
CONJ_IT_END:
#ifdef RESDEBUG
    cerr << "CONJ END " << ig_loop_it << " num_len=" << num_len << " dna_len=" << dna_len << endl;//
#endif
        for (int i = 0; i < 20; ++i) {
            int a = rand() % MAX_GENETIC_CNT, b = rand() % MAX_GENETIC_CNT;
            for (int j = 0; j < 4; ++j) {
                int c = rand() % num_len;
                swap(dna[a].str[c], dna[b].str[c]);
            }
        }
    }
}

void solve() {
#ifdef RESDEBUG
    cerr << "map " << step << "-" << lvl << endl;//
#endif
    DNA();
    vector<tower_t> solutions;
    solutions.clear();
    
    res.dna.len = init_map(res.dna);
    init_dnafires(res.dna);
    ans.cost = make_map(res.gen);//FIXME:
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
    
    for (int dx = -7; dx <= 7; ++dx) {
        for (int dy = -7; dy <= 7; ++dy) {
            int r = 0;
            while (r*r <= dx*dx + dy*dy) {
                ++r;
            }
            CDIS[dx+10][dy+10] = r-1;
        }
    }
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
#ifdef RESDEBUG
                              cerr << "OBSTACLE " << r << "," << c << endl;///
#endif
                    case 's': p.x = r, p.y = c; appears.push_back(p);
                              basic_map[r][c] = APPEAR; ++n_appear;
#ifdef RESDEBUG
                              cerr << "APPEAR " << r << "," << c << endl;///
#endif
                              break;
                    case 'g': p.x = r, p.y = c; defends.push_back(p);
                              basic_map[r][c] = DEFEND; ++n_defend; 
#ifdef RESDEBUG
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

