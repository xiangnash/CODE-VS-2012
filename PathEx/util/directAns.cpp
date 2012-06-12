/*
 *  
 */
#include "../rules.h"
#include "answer.h"

using namespace std;

int main()
{
    int Tsi = 0, Tansi = 0;
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
            /*
            for (int r = 0; r < H; ++r) {
                for (int c = 0; c < W; ++c) {
                    init_g[r][c] = basic_map[r][c];
                }
            }
            */
            
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
            
            

            //TODO: restore original map
            //1. read my answer
            //
            //2. my answer - input
            cout << Ts[Tsi] << endl;
            for (int i = 0; i < Ts[Tsi]; ++i) {
                cout << ans[Tansi][0] << " " << ans[Tansi][1] << " " << ans[Tansi][2] << " " << ans[Tansi][3] << endl;//
                ++Tansi;
            }
            ++Tsi;
            cout.flush();
        }
    }
    return 0;
}

