#include "pathEx.h"
//#include "simple.h"

//#define INPUTDEBUG

void output() {
    vector<tower_t> outs(solutions);
    for (int i = 0; i < init_towers.size(); ++i) {
        bool flag = true;
        for (int j = 0; j < solutions.size(); ++j) {
            if (init_towers[i].x == solutions[j].x
                    && init_towers[i].y == solutions[j].y) {
                flag = false;
                break;
            }
        }
        if (flag) {
            init_towers[i].A = 0, init_towers[i].type = BLANK;
            outs.push_back(init_towers[i]);
        }
    }
#ifdef INPUTDEBUG
    cerr << "Res " << outs.size() << endl;
#endif
    cout << outs.size() << endl;///
    for (int i = 0; i < outs.size(); ++i) {
        cout << outs[i].y << " " << outs[i].x << " " << outs[i].A << " " << outs[i].type << endl;
    }
    cout.flush();
}

int get_map() {
    //map
    scanf("%d %d\n", &W, &H);
    defends.clear(); //n_defend = 0;
    appears.clear(); //n_appear = 0;

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
#ifdef INPUTDEBUG
                          cerr << "OBSTACLE " << r << "," << c << endl;///
#endif
                case 's': p.x = r, p.y = c; appears.push_back(p);
                          basic_map[r][c] = APPEAR; //++n_appear;
#ifdef INPUTDEBUG
                          cerr << "APPEAR " << r << "," << c << endl;///
#endif
                          break;
                case 'g': p.x = r, p.y = c; defends.push_back(p);
                          basic_map[r][c] = DEFEND; //++n_defend; 
#ifdef INPUTDEBUG
                          cerr << "DEFEND " << r << "," << c << endl;///
#endif
                          break; //XXX
                default: cerr << "error" << endl;
            }
        }
    }
    //init
}

void get_level() {
    int T, E;
    scanf("%d %d %d %d", &Life, &Money, &T, &E); 
    //tower
    //        n_towers = T;
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
    //        n_enermy = E;
    enermys.clear();
    for (int i = 0; i < E; ++i) {
        int x, y, te, le, se;
        scanf("%d %d %d %d %d\n", &y, &x, &te, &le, &se);
        enermy_t enermy;
        enermy.x = x, enermy.y = y, enermy.Te = te, enermy.Le = le, enermy.Se = se;
        enermys.push_back(enermy);
    }

    char end_str[111];
    scanf("%s", end_str);
}

int main(int argc, char *argv[])
{
    int S;
    scanf("%d", &S);
    for (step = 1; step <= S; ++step) {
        get_map();
        int L;
        scanf("%d\n", &L);
        char end_str[11];
        scanf("%s", end_str);
        for (lvl = 1; lvl <= L; ++lvl) {
            get_level();
#ifdef INPUTDEBUG
            cerr << "map " << step << "-" << lvl << endl;///
#endif
            solve();
            output();
        }
    }
    return 0;
}


