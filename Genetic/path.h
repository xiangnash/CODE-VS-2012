#include "rules.h"

//#define PATHDEBUG
//#define ROADDEBUG

struct road_t {
    int x, y;
    int d;
    int cnt;
    int weight;
    bool operator<(const road_t &other) const {
        if (weight < other.weight) return true;
        else if (weight > other.weight) return false; 
        return (x + y < other.x + other.y);
    } 
    bool operator==(const road_t &other) const {
        return (x == other.x && y == other.y);
    }
};

int rcnt[MAXN][MAXN];
set<road_t> rnodes;
int dis[MAXN][MAXN];

bool is_linked() {
    queue<pos_t> q;
    int vis[H][W];
    memset(vis, false, sizeof(vis));
    pos_t now, nxt;
    for (int i = 0; i < n_defend; ++i) {
        now.x = defends[i].x, now.y = defends[i].y;
        vis[now.x][now.y] = true;
        q.push(now);
    }
    int n_reach = 0;
    while (!q.empty()) {
        now = q.front(); q.pop();
        if (APPEAR == g[ now.x ][ now.y ]) {
            ++n_reach;
            if (n_reach == n_appear) return true;
        }
        pos_t p;//(x, y);
        p.x = now.x, p.y = now.y;
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

inline void add_rnode(const int x, const int y) {
    road_t rn;
    rn.x = x, rn.y = y, rn.d = dis[x][y], rn.weight = 1;
     for (set<road_t>::iterator it = rnodes.begin(); it != rnodes.end(); ++it) {
        if (it->x == x && it->y == y) {
            rn.weight += it->weight;
            rnodes.erase(it);
            break;
        }
    }
    rnodes.insert(rn);
}

void get_road() {
    rnodes.clear();
    memset(in_road, false, sizeof(in_road));
    for (int i = 0; i < n_appear; ++i) {
        pos_t p;
        p.x = appears[i].x, p.y = appears[i].y;
        while (DEFEND != g[p.x][p.y]) {
#ifdef ROADDEBUG
            cerr << "r\t" << p.x << "," << p.y << endl;///
#endif
            in_road[p.x][p.y] = true;
            add_rnode(p.x, p.y);
            if (vec[p.x][p.y] % 2 == 1) {
                in_road[p.x][ path[p.x][p.y].y ] = true;
                add_rnode(p.x, path[p.x][p.y].y);
                in_road[ path[p.x][p.y].x ][p.y] = true;
                add_rnode(path[p.x][p.y].x, p.y);
            }
            int nx = road[p.x][p.y].x, ny = road[p.x][p.y].y;
            p.x = nx, p.y = ny;
        }
    }
}

void get_path() {
    memset(in_path, false, sizeof(in_path));
    for (int i = 0; i < n_appear; ++i) {
        pos_t p;
        p.x = appears[i].x, p.y = appears[i].y;
        while (DEFEND != g[p.x][p.y]) {
#ifdef PATHDEBUG
            cerr << "p\t" << p.x << "," << p.y << endl;///
#endif
            in_path[p.x][p.y] = true;
            if (vec[p.x][p.y] % 2 == 1) {
                in_path[p.x][ path[p.x][p.y].y ] = true;
                in_path[ path[p.x][p.y].x ][p.y] = true;
            }
            int nx = path[p.x][p.y].x, ny = path[p.x][p.y].y;
            p.x = nx, p.y = ny;
        }
    }
}


bool find_path() {
    priority_queue<queue_t> q;
    int dist[H][W];
    int max_steps = H*W*14;
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            dist[r][c] = max_steps;
            vec[r][c] = 9;
        }
    }
    queue_t now, nxt;
    for (int i = 0; i < n_defend; ++i) {
        queue_t now;
        now.t = 0, now.x = defends[i].x, now.y = defends[i].y;
        dist[now.x][now.y] = now.t;
        q.push(now);
    }
    int n_reach = 0;
    while (!q.empty()) {
        now = q.top(); q.pop();
        if (now.t > dist[now.x][now.y]) continue;
//        cerr << now.t << " reach " << now.x << "," << now.y << endl;///
        if (APPEAR == g[ now.x ][ now.y ]) {
            ++n_reach;
//            cerr << n_reach << "  target=" << n_appear << endl;///
            if (n_reach == n_appear) return true;
        }
        pos_t p;//(x, y);
        p.x = now.x, p.y = now.y;
        for (int i = 0; i < 8; ++i) {//replace with edges
            nxt.x = now.x - DX[i], nxt.y = now.y - DY[i]; 
            if (out_map(nxt.x, nxt.y)) continue;
            if (OBSTACLE == g[nxt.x][nxt.y] || TOWER == g[nxt.x][nxt.y]) continue;
            if (1 == i%2 && 
                    (OBSTACLE == g[now.x][nxt.y] || TOWER == g[now.x][nxt.y]
                     || OBSTACLE == g[nxt.x][now.y] || TOWER == g[nxt.x][now.y])) continue;
            nxt.t = now.t + COST[i];
            if (nxt.t < dist[nxt.x][nxt.y] || (nxt.t == dist[nxt.x][nxt.y] && i < vec[nxt.x][nxt.y])) {
                if (nxt.t != dist[nxt.x][nxt.y]) q.push(nxt);
                dist[nxt.x][nxt.y] = nxt.t;
                vec[nxt.x][nxt.y] = i;
                path[nxt.x][nxt.y].x = p.x;//XXX:
                path[nxt.x][nxt.y].y = p.y;
            }
        }
    }
    return false;
}

bool find_road() {
    priority_queue<queue_t> q;
    int max_steps = H*W*14;
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            dis[r][c] = max_steps;
            vec[r][c] = 7;
        }
    }
    queue_t now, nxt;
    for (int i = 0; i < n_defend; ++i) {
        now.t = 0, now.x = defends[i].x, now.y = defends[i].y;
        dis[now.x][now.y] = now.t;
        q.push(now);
    }
    int n_reach = 0;
    while (!q.empty()) {
        now = q.top(); q.pop();
        if (now.t > dis[now.x][now.y]) continue;
        if (APPEAR == g[ now.x ][ now.y ]) {
            ++n_reach;
//            cerr << n_reach << "  target=" << n_appear << endl;///
            if (n_reach == n_appear) return true;
        }
        pos_t p;//(x, y);
        p.x = now.x, p.y = now.y;
        for (int i = 0; i < 8; ++i) {//replace with edges
            nxt.x = now.x - DX[i], nxt.y = now.y - DY[i]; 
            if (out_map(nxt.x, nxt.y)) continue;
            if (OBSTACLE == g[nxt.x][nxt.y] || TOWER == g[nxt.x][nxt.y]) continue;
            if (1 == i%2 && 
                    (OBSTACLE == g[now.x][nxt.y] || TOWER == g[now.x][nxt.y]
                     || OBSTACLE == g[nxt.x][now.y] || TOWER == g[nxt.x][now.y])) continue;
            nxt.t = now.t + COST[i];
            if (nxt.t < dis[nxt.x][nxt.y] || (nxt.t == dis[nxt.x][nxt.y] && i < vec[nxt.x][nxt.y])) {
                if (nxt.t != dis[nxt.x][nxt.y]) q.push(nxt);
                dis[nxt.x][nxt.y] = nxt.t;
                vec[nxt.x][nxt.y] = i;
                road[nxt.x][nxt.y].x = p.x,
                road[nxt.x][nxt.y].y = p.y;
            }
        }
    }
    return false;
}
