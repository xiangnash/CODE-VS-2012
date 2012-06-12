/*
 * ./restoremap.out < ../data/output.txt > ../data/map.txt
 */
#include "../rules.h"

using namespace std;

int main()
{
    FILE *ansin = fopen("../data/input.txt", "r");///
    int S;
    vector<tower_t> my_towers;
    my_towers.clear();
    scanf("%d", &S);
    cout << S << endl;
    for (step = 1; step <= S; ++step) {
        //map
        scanf("%d %d\n", &W, &H);
        cout << W << " " << H << endl;
        defends.clear(); 
        appears.clear();
        
        for (int r = 0; r < H; ++r) {
            char str[W+3];
            scanf("%s\n", str);
            cout << str << endl;
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
                              basic_map[r][c] = APPEAR; 
#ifdef DEBUG
                              cerr << "APPEAR " << r << "," << c << endl;///
#endif
                              break;
                    case 'g': p.x = r, p.y = c; defends.push_back(p);
                              basic_map[r][c] = DEFEND; 
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
        cout << L << endl;
        char end_str[11];
        scanf("%s", end_str);
        cout << "END" << endl;
        for (lvl = 1; lvl <= L; ++lvl)  {
            int T, E;
            scanf("%d %d %d %d", &Life, &Money, &T, &E); 
            //tower
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
            
            if (lvl > 1) {//compare with previous towers
                int Tn = 0;
                for (int i = 0; i < T; ++i) {
                    bool flag = true;
                    for (int j = 0; j < my_towers.size(); ++j) {//FIXME:
                        if (init_towers[i].x == my_towers[j].x && init_towers[i].y == my_towers[j].y
                                && init_towers[i].A == my_towers[j].A && init_towers[i].type == my_towers[j].type) {
                            flag = false;
                        }
                    }
                    if (flag) {
                        ++Tn;
                    }
                }
                cout << Life << " " << Money << " " << Tn << " " << E << endl;
                for (int i = 0; i < T; ++i) {
                    bool flag = true;
                    for (int j = 0; j < my_towers.size(); ++j) {//FIXME:
                        if (init_towers[i].x == my_towers[j].x && init_towers[i].y == my_towers[j].y
                                && init_towers[i].A == my_towers[j].A && init_towers[i].type == my_towers[j].type) {
                            flag = false;
                        }
                    }
                    if (flag) {
                        cout << init_towers[i].y << " " << init_towers[i].x << " " << init_towers[i].A << " " << init_towers[i].type << endl;
                    }
                }
            } else {//
                cout << Life << " " << Money << " " << T << " " << E << endl;///
                for (int i = 0; i < T; ++i) {
                    cout << init_towers[i].y << " " << init_towers[i].x << " " << init_towers[i].A << " " << init_towers[i].type << endl;
                }
            }

            //enermy
            enermys.clear();
            for (int i = 0; i < E; ++i) {
                int x, y, te, le, se;
                scanf("%d %d %d %d %d\n", &y, &x, &te, &le, &se);
                cout << y << " " << x << " " << te << " " << le << " " << se << endl;
                enermy_t enermy;
                enermy.x = x, enermy.y = y, enermy.Te = te, enermy.Le = le, enermy.Se = se;
                enermys.push_back(enermy);
            }

            scanf("%s", end_str);
            cout << "END" << endl;///
            
            

            //TODO: restore original map
            //1. read my answer
            //
            //2. my answer - input
            int Ta;
            fscanf(ansin, "%d", &Ta);
            my_towers.clear();
            for (int i = 0; i < Ta; ++i) {
                tower_t tower;
                fscanf(ansin, "%d %d %d %d", &tower.y, &tower.x, &tower.A, &tower.type);
                my_towers.push_back(tower);
            }
        }
    }
    fclose(ansin);
    return 0;
}

