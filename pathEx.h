#include "codevs.h"

//#define NODEDEBUG
//#define WT_DEBUG
//#define SOL_DEBUG

struct ans_t {
    vector<tower_t> mytowers;
    int life, cost;
};

struct node_t {
    int x, y;
    int weight, d;
    node_t(const int _x, const int _y, const int _w) : x(_x), y(_y), weight(_w) {}
    bool operator<(const node_t &other) const {
        if (weight > other.weight) return true;
        else if (weight < other.weight) return false;
        return (x > other.x || (x == other.x && y > other.y));
    }
};

ans_t ans;

class PathFinder {
    
    int update_weighted_path() {
        memset(in_path, false, sizeof(in_path));
        int wt[H][W];
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
//                edges[r][c].clear();
                wt[r][c] = 0;
            }
        }
        
        int now_len = 0;
        for (int i = 0; i < appears.size(); ++i) {
            pos_t p(appears[i].x, appears[i].y);
            while (DEFEND != g[p.x][p.y]) {
#ifdef WT_DEBUG
                cerr << "p\t" << p.x << "," << p.y << endl;///
#endif
                in_path[p.x][p.y] = true;
                wt[p.x][p.y] += 1;
                if (vec[p.x][p.y] % 2 == 1) {
                    in_path[p.x][ path[p.x][p.y].y ] = true;
                    in_path[ path[p.x][p.y].x ][p.y] = true;
//                    edges[p.x][ path[p.x][p.y].y ].push_back(p);
                    ++wt[p.x][ path[p.x][p.y].y ];
//                    edges[ path[p.x][p.y].x ][p.y].push_back(p);
                    ++wt[ path[p.x][p.y].x ][p.y];
                }
                 int nx = path[p.x][p.y].x, ny = path[p.x][p.y].y;
                p.x = nx, p.y = ny;
                ++now_len;
            }
        }
        
        bool flag = false;
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                if (wt[r][c] != weight[r][c] && g[r][c] == EMPTY) {
                    node_t node(r, c, weight[r][c]);
                    weight[r][c] = wt[r][c];
                }
                if (vec[r][c] < 8) {
                    pos_t p(r, c);
                    int nx = r+DX[ vec[r][c] ], ny = c+DY[ vec[r][c] ];
//                    edges[nx][ny].push_back(p);
                }
            }
        }
        return now_len;
    }
    
    bool is_linked() {
        queue<pos_t> q;
        int vis[H][W];
        memset(vis, false, sizeof(vis));
        pos_t now, nxt;
        for (int i = 0; i < defends.size(); ++i) {
            now.x = defends[i].x, now.y = defends[i].y;
            vis[now.x][now.y] = true;
            q.push(now);
        }
        int n_reach = 0;
        while (!q.empty()) {
            now = q.front(); q.pop();
            if (APPEAR == g[ now.x ][ now.y ]) {
                ++n_reach;
                if (n_reach == appears.size()) return true;
            }
            for (int i = 0; i < 8; i += 2) {//replace with edges
                nxt.x = now.x - DX[i], nxt.y = now.y - DY[i]; 
                if (out_map(nxt.x, nxt.y)) continue;
                if (vis[nxt.x][nxt.y] || OBSTACLE == g[nxt.x][nxt.y] || TOWER == g[nxt.x][nxt.y]) continue;
                vis[nxt.x][nxt.y] = true;
                q.push(nxt);
            }
        }
        return false;
    }
    
    int get_weighted_path() {
        memset(in_path, false, sizeof(in_path));
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                edges[r][c].clear();
                weight[r][c] = 0;
            }
        }
        
        init_len = 0;
        for (int i = 0; i < appears.size(); ++i) {
            pos_t p(appears[i].x, appears[i].y);
            while (DEFEND != g[p.x][p.y]) {
#ifdef WT_DEBUG
                cerr << "p\t" << p.x << "," << p.y << endl;///
#endif
                in_path[p.x][p.y] = true;
                ++weight[p.x][p.y];
                if (vec[p.x][p.y] % 2 == 1) {
                    in_path[p.x][ path[p.x][p.y].y ] = true;
                    in_path[ path[p.x][p.y].x ][p.y] = true;
//                    edges[p.x][ path[p.x][p.y].y ].push_back(p);
                    ++weight[p.x][ path[p.x][p.y].y ];
//                    edges[ path[p.x][p.y].x ][p.y].push_back(p);
                    ++weight[ path[p.x][p.y].x ][p.y];
                }
                int nx = path[p.x][p.y].x, ny = path[p.x][p.y].y;
                p.x = nx, p.y = ny;
                ++init_len;
            }
        }
        
        innodes.clear();
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                if (weight[r][c] && g[r][c] == EMPTY) {
                    node_t node(r, c, weight[r][c]);
                    innodes.insert(node);
                }
                if (vec[r][c] < 8) {
                    pos_t p(r, c);
                    int nx = r+DX[ vec[r][c] ], ny = c+DY[ vec[r][c] ];
                    edges[nx][ny].push_back(p);
                }
            }
        }
        return init_len;
    }
    
    bool find_path() {
        priority_queue<queue_t> q;
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                dist[r][c] = UNREACHABLE;
                vec[r][c] = 9;
            }
        }
        queue_t now, nxt;
        for (int i = 0; i < defends.size(); ++i) {
            queue_t now;
            now.t = 0, now.x = defends[i].x, now.y = defends[i].y;
            dist[now.x][now.y] = now.t;
            q.push(now);
        }
        int n_reach = 0;
        while (!q.empty()) {
            now = q.top(); q.pop();
//            cout << "-->" << now.x << "," << now.y << ": " << now.t << endl;//
            if (now.t > dist[now.x][now.y]) continue;
            if (APPEAR == g[ now.x ][ now.y ]) {
                ++n_reach;
//                if (n_reach == appears.size()) return true;
            }
            for (int i = 0; i < 8; ++i) {
                nxt.x = now.x - DX[i], nxt.y = now.y - DY[i]; 
                if (out_map(nxt.x, nxt.y)) continue;
                if (OBSTACLE == g[nxt.x][nxt.y] || TOWER == g[nxt.x][nxt.y]) continue;
                if (1 == i%2 && 
                        (OBSTACLE == g[now.x][nxt.y] || TOWER == g[now.x][nxt.y]
                         || OBSTACLE == g[nxt.x][now.y] || TOWER == g[nxt.x][now.y])) continue;
                nxt.t = now.t + COST[i];
                if (nxt.t < dist[nxt.x][nxt.y] || (nxt.t == dist[nxt.x][nxt.y] && i < vec[nxt.x][nxt.y])) {
                    if (nxt.t != dist[nxt.x][nxt.y]) {
                        q.push(nxt);
                    }
                    dist[nxt.x][nxt.y] = nxt.t;
                    vec[nxt.x][nxt.y] = i;
                    path[nxt.x][nxt.y].x = now.x;//XXX:
                    path[nxt.x][nxt.y].y = now.y;
                }
            }
        }
        return false;
    }
    
public:
    int H, W;
    int g[MAXN][MAXN];
    int dist[MAXN][MAXN];
    int vec[MAXN][MAXN];
    
    bool in_path[MAXN][MAXN];
    pos_t path[MAXN][MAXN];
    vector<pos_t> edges[MAXN][MAXN];
    
    set<node_t> innodes;
    int weight[MAXN][MAXN];
    
    int init_len;
    
    PathFinder(const int _H, const int _W, const int basic_map[MAXN][MAXN]) {
        H = _H, W = _W;
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                g[r][c] = basic_map[r][c];
            }
        }
    }
    
    tower_t place_tower(const int x, const int y, const int d) {
        tower_t tower(x, y, 0, RAPID);
        if ((x+y) % 29 == 1) tower.type = ATTACK;
        if (d <= 2) {
            tower.type = ATTACK, tower.A = 2;
        }
        else if (d < 6) {//FIXME: <  7
            tower.A = d-2, tower.type = FREEZE;
            if (ipc[x][y][d] > 4 && d > 2) {
                tower.type = RAPID;
            }
        }
        return tower;
    }
    
    void build_tower(codevs_t &vs, const vector<pos_t> & ptowers) {
        int R = 7;
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                mydis[r][c] = 10;
            }
        }
        memset(ipc, 0, sizeof(ipc));
        
        for (int i = 0; i < appears.size(); ++i) {
            pos_t p(appears[i].x, appears[i].y);
            do {
                
                for (int dx = -R; dx <= R; ++dx) {
                    for (int dy = -R; dy <= R; ++dy) {
                        int nx = p.x+dx, ny = p.y+dy;
                        if (out_map(nx, ny) || in_path[nx][ny]) continue;
                        if (CDIS[dx+10][dy+10] < 8) {
                            ++ipc[nx][ny][ CDIS[dx+10][dy+10] ];
                            mydis[nx][ny] = min(mydis[nx][ny], CDIS[dx+10][dy+10]);
                        }
                    }
                }
                
                if (DEFEND == g[p.x][p.y]) break;
#ifdef WT_DEBUG
                cerr << "p\t" << p.x << "," << p.y << endl;///
#endif
                int nx = path[p.x][p.y].x, ny = path[p.x][p.y].y;
                p.x = nx, p.y = ny;
            } while (true);
        }
        
        int vital[H][W];
        int vv[] = {0, 240, 240, 100, 50, 14, 5, 1};
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                vital[r][c] = 0;
                for (int i = 1; i <= 7; ++i) {
                    vital[r][c] += ipc[r][c][i] * vv[i];
                }
            }
        }
        
        priority_queue<node_t> q;
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                if (EMPTY == g[r][c]) {
                    node_t node(r, c, vital[r][c]);
                    q.push(node);
                }
            }
        }
        
        vector<tower_t> mytowers;
        mytowers.clear();
        int mycost = 0;
        for (int i = 0; i < ptowers.size(); ++i) {
            int x = ptowers[i].x, y = ptowers[i].y;
            int d = mydis[x][y];
            tower_t tower = place_tower(x, y, d);
            mytowers.push_back(tower);
            mycost += get_cost(tower.type, tower.A);
            vs.set_map(basic_map);
            vs.set_my_towers(mytowers);
            vs.find_path();
            vs.get_path();
            int t_life = vs.simulate(Life);
#if 0
            if (t_life > ans.life || (t_life == ans.life && mycost < ans.cost)) {
                ans.life = t_life, ans.cost = mycost;
                ans.mytowers = mytowers;
            }
#endif
//#if 0
            if (t_life >= ans.life) {
                vector<tower_t> cur_towers;
                cur_towers.clear();
                for (int i = 0; i < mytowers.size(); ++i) {
                    tower_t tower(mytowers[i].x, mytowers[i].y, mytowers[i].A, mytowers[i].type);
                    cur_towers.push_back(tower);
                }
                for (int i = 0; i < cur_towers.size(); ++i) {
                    if (!vs.shooted[i]) {
                        cur_towers[i].type = 0, cur_towers[i].A = 0;
                    }
                }
                int t_cost = 0;
                for (int i = 0; i < cur_towers.size(); ++i) {
                    t_cost += get_cost(cur_towers[i].type, cur_towers[i].A);
                }

                if (t_life > ans.life || (t_life == ans.life && t_cost < ans.cost)) {
                    ans.life = t_life, ans.cost = t_cost;
                    ans.mytowers = cur_towers;
                }
            }
//#endif
        }
        if (ans.life == Life && mycost > ans.life) {
            return ;
        }
        vs.set_map(basic_map);
            vs.set_my_towers(mytowers);
            vs.find_path();
            vs.get_path();
        vs.set_my_towers(mytowers);
        int t_life = vs.simulate(Life);
        if (t_life > ans.life || (t_life == ans.life && mycost < ans.cost)) {
            ans.life = t_life, ans.cost = mycost;
            ans.mytowers = mytowers;
        }
        
        while (!q.empty()) {
            node_t now = q.top(); q.pop();
            tower_t tower = place_tower(now.x, now.y, mydis[now.x][now.y]);
            mytowers.push_back(tower);
            mycost += get_cost(tower.type, tower.A);
            if (ans.life == Life && mycost > ans.life*4) {
                return ;
            }
            vs.set_map(basic_map);
            vs.set_my_towers(mytowers);
            if (!vs.find_path()) {
                mytowers.erase(mytowers.begin() + (mytowers.size()+1));
                continue;
            }
            vs.get_path();
            vs.set_my_towers(mytowers);
            t_life = vs.simulate(Life);
            if (t_life == 0) continue;
#if 0
            if (t_life > ans.life || (t_life == ans.life && mycost < ans.cost)) {
                ans.life = t_life, ans.cost = mycost;
                ans.mytowers = mytowers;
            }
#endif
//#if 0
            if (t_life >= ans.life) {
                vector<tower_t> cur_towers;
                cur_towers.clear();
                for (int i = 0; i < mytowers.size(); ++i) {
                    tower_t tower(mytowers[i].x, mytowers[i].y, mytowers[i].A, mytowers[i].type);
                    cur_towers.push_back(tower);
                }
                for (int i = 0; i < cur_towers.size(); ++i) {
                    if (!vs.shooted[i]) {
                        cur_towers[i].type = 0, cur_towers[i].A = 0;
                    }
                }
                int t_cost = 0;
                for (int i = 0; i < cur_towers.size(); ++i) {
                    t_cost += get_cost(cur_towers[i].type, cur_towers[i].A);
                }

                if (t_life > ans.life || (t_life == ans.life && t_cost < ans.cost)) {
                    ans.life = t_life, ans.cost = t_cost;
                    ans.mytowers = cur_towers;
                }
            }
//#endif
        }
    }
    
    vector<pos_t> get_locations() {
        codevs_t vs(H, W, basic_map);

        vector<pos_t> res;
        res.clear();
        int tot = 2500;
        for (int i = 0; i < tot; ++i) {
            find_path();
            get_weighted_path();
            bool flag = false;
            pos_t cur;
            int max_gain = 0;

            for (set<node_t>::iterator it = innodes.begin(); it != innodes.end(); ++it) {
                node_t now = *it;
                g[now.x][now.y] = TOWER;
                if (is_linked()) {
                    find_path();
                    int gain = update_weighted_path();
                    if (!flag || (flag && gain > max_gain)) {
                        flag = true;
                        max_gain = gain;
                        cur.x = now.x, cur.y = now.y;
                    }
                }
                g[now.x][now.y] = EMPTY;
            }
            if (!flag) break;
            res.push_back(cur);
            g[cur.x][cur.y] = TOWER;
            
            vs.set_map(g);
            vs.find_path();//
            vs.get_path();
            build_tower(vs, res);
            vs.set_map(g);
            vs.find_path();//
            vs.get_path();
            build_tower(vs, res);
        }
        return res;
    }
   
};

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

void solve() {
#ifdef EXDEBUG
    cerr << "map=" << step << " lvl=" << lvl << endl;//
#endif
    if (lvl > 1) {
        codevs_t vs(H, W, basic_map);
        int tg[MAXN][MAXN];
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                tg[r][c] = basic_map[r][c];
            }
        }
        for (int i = 0; i < ans.mytowers.size(); ++i) {
            tg[ ans.mytowers[i].x ][ ans.mytowers[i].y ] = TOWER;
        }
        vs.set_map(tg);
        vs.set_my_towers(ans.mytowers);
        vs.find_path();
        vs.get_path();
        if (vs.simulate(Life) == Life) {
            solutions = ans.mytowers;
            return ;
        }
    }
    ans.life = 0, ans.cost = 1000000;
    
    for (int dx = -7; dx <= 7; ++dx) {
        for (int dy = -7; dy <= 7; ++dy) {
            int r = 0;
            while (r*r <= dx*dx + dy*dy) {
                ++r;
            }
            CDIS[dx+10][dy+10] = r-1;
        }
    }
    
    PathFinder pfinder(H, W, basic_map);
    vector<pos_t> loc = pfinder.get_locations();
    //clear
    solutions = ans.mytowers;
}

