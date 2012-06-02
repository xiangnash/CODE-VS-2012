#include "rules.h"

//#define PATH_DEBUG
//#define SIM_DEBUG

struct target_t {
    int id;
    int t; //enter time
    int at; //apear time
    bool operator<(const target_t &other) const {
        if (t < other.t) return true;
        else if (t > other.t) return false;
        if (at < other.at) return true;
        else if (at > other.at) return false;
        return (id < other.id);
    }
    bool operator==(const target_t &other) const {
        return id == other.t;
    }
};

struct queue_t {
    int x, y;
    int t;
    bool operator<(const queue_t &another) const {
        return t > another.t;
    }
};

struct event_t {
    int t;
    int act;
    int id;
    bool operator<(const event_t &other) const {
        return t > other.t;
    }
};

set<target_t> targets[MAXN_TOWER];
set<int> fireholes[MAXN_ENERMY];

inline set<target_t>::iterator find_in_set(const int id, const set<target_t> & tars) {
    set<target_t>::iterator it;
    for (it = tars.begin(); it != tars.end(); it++) {
        if (it->id == id) {
            return it;
        }
    }
    return tars.end();
}

struct codevs_t {
    int H, W;
    vector<tower_t> my_towers;
    
    int g[MAXN][MAXN];
    int dist[MAXN][MAXN];
    int vec[MAXN][MAXN];
    
    bool in_path[MAXN][MAXN];
    pos_t path[MAXN][MAXN];
    
    bool shooted[MAXN*MAXN];
    
    codevs_t(const int _H, const int _W, const int basic_map[MAXN][MAXN]) {
        H = _H, W = _W;
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                g[r][c] = basic_map[r][c];
            }
        }
    }
    
    void set_map(const int init_map[MAXN][MAXN]) {
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                g[r][c] = init_map[r][c];
            }
        }
    }
    
    void set_my_towers(const vector<tower_t> &towers) {
        my_towers = towers; //FIXME:
        // set NO tower first
        for (int i = 0; i < my_towers.size(); ++i) {
            g[ my_towers[i].x ][ my_towers[i].y ] = TOWER;
        }
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
                if (n_reach == appears.size()) return true;
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
    
    bool get_path() {
        memset(in_path, false, sizeof(in_path));
        for (int i = 0; i < appears.size(); ++i) {
            pos_t p(appears[i].x, appears[i].y);
            while (DEFEND != g[p.x][p.y]) {
#ifdef PATH_DEBUG
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
    
    int simulate(int life) {
#ifdef SIM_DEBUG
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                cerr << g[r][c];
            }
            cerr << endl;//
        }
#endif
        memset(shooted, false, sizeof(shooted));
        priority_queue<event_t> q;
        //init targets
        for (int i = 0; i < my_towers.size(); ++i) {
            targets[i].clear();
            event_t e;
            e.t = 1;
            e.act = FIRE;
            e.id = i;
            q.push(e);
#ifdef SIM_DEBUG
            cerr << "tower " << my_towers[i].x << "," << my_towers[i].y << endl;///
#endif
        }
        //init enermys
//        vector<enermy_t> cur_enermys(enermys);
        vector<enermy_t> cur_enermys;
        cur_enermys.clear();
        for (int i = 0; i < enermys.size(); ++i) {
            enermy_t enermy;
            enermy.x = enermys[i].x, enermy.y = enermys[i].y, enermy.Te = enermys[i].Te, enermy.Le = enermys[i].Le, enermy.Se = enermys[i].Se;
            cur_enermys.push_back(enermys[i]);
            event_t e;
            e.t = enermy.Te;//
            e.act = MOVE;//
            e.id = i;
            q.push(e);
            fireholes[i].clear();
#ifdef SIM_DEBUG
            cerr << "enermy " << cur_enermys[i].x << "," << cur_enermys[i].y << " " << cur_enermys[i].Te << " " << cur_enermys[i].Se << endl;///
            cerr << "enermy " << enermys[i].x << "," << enermys[i].y << " " << enermys[i].Te << " " << enermys[i].Se << endl;///
#endif
        }

        bool die[ cur_enermys.size() ];
        memset(die, false, sizeof(die));

        int clear_cnt = 0;
        vector<int> fires;
        fires.clear();
#ifdef SIM_DEBUG
        cerr << "enermy=" << cur_enermys.size() << "  tower=" << my_towers.size() << endl;///
#endif
        while (!q.empty()) {
            event_t e = q.top();
            vector<int> moves;
            moves.clear();
            while (!q.empty()) {
                event_t ce = q.top();
                if (ce.t > e.t) {
                    break;
                } else {
                    if (ce.act == MOVE) {//MOVE:
                        moves.push_back(ce.id);
                    }
                    else {//FIRE
                        fires.push_back(ce.id);
                    }
                }
                q.pop();
            }
            set<int> ops;
            ops.clear();
#ifdef SIM_DEBUG
            cerr << ">>>>>>>>>>>>>>>  time=" << e.t << " move=" << moves.size() << " fire=" << fires.size() << endl;///
#endif
            for (int i = 0; i < moves.size(); ++i) {
                int id = moves[i];
#ifdef SIM_DEBUG
                cerr << "move ? " << die[id] << " " << cur_enermys[id].Te << endl;///
#endif
                if (die[id]) continue;
                if (cur_enermys[id].Te > e.t) {
                    event_t ne;
                    ne.t = cur_enermys[id].Te;
                    ne.id = id;
                    ne.act = MOVE;
                    q.push(ne);
                    continue;
                }
                ops.insert(id);
#ifdef SIM_DEBUG
                cerr << cur_enermys[id].Te << " move " << id << " (" << cur_enermys[id].x << "," << cur_enermys[id].y << ")"; ////
#endif
                //perform move to new position
                if (false/*e.t == enermys[id].Te*/) {//first appear

                } else {
                    //
                    pos_t np = path[ cur_enermys[id].x ][ cur_enermys[id].y ];
                    if (e.t != enermys[id].Te) { //FIXME:
                        cur_enermys[id].x = np.x, cur_enermys[id].y = np.y;
                    }
                    np = path[ cur_enermys[id].x ][ cur_enermys[id].y ];
                    cur_enermys[id].Te += get_walk_time(cur_enermys[id].Se, cur_enermys[id].x, cur_enermys[id].y, np.x, np.y);
                    event_t ne;
                    ne.t = cur_enermys[id].Te;
                    ne.id = id;
                    ne.act = MOVE;
                    q.push(ne);
                }
#ifdef SIM_DEBUG
                cerr << "---->(" << cur_enermys[id].x << "," << cur_enermys[id].y << ")" << " " << cur_enermys[id].Te << endl; ///
#endif
                //update relations 
                for (int j = 0; j < my_towers.size(); ++j) {
                    set<target_t>::iterator it = find_in_set(id, targets[j]);
                    if (in_range(my_towers[j], cur_enermys[id])) {
                        //put in
                        if (it == targets[j].end()) {
                            target_t target;
                            target.id = id;
                            target.t = e.t;
                            target.at = enermys[id].Te;
                            targets[j].insert(target);
                            fireholes[id].insert(j);
#ifdef SIM_DEBUG
                            cerr << id << " enter the view of " << j << endl;///
#endif
                        }
                    } else {
                        //erase out
                        if (it == targets[j].end()) {
                        }
                        else {
                            targets[j].erase(it);
                            for (set<int>::iterator ih = fireholes[id].begin(); ih != fireholes[id].end(); ++ih) {
                                if (*ih == j) {
                                    fireholes[id].erase(ih);
                                    break;
                                }
                            }
#ifdef SIM_DEBUG
                            cerr << id <<  "out the view of " << j << endl;///
#endif
                        }
                    }
                }
            }

            vector<int> new_fires;
            new_fires.clear();
            for (int i = 0; i < fires.size(); ++i) {
                if (targets[ fires[i] ].size() > 0) {
                    //                targets[ fires[i] ].sort();
                    int id = targets[ fires[i] ].begin()->id;
                    shooted[ fires[i] ] = true;
                    ops.insert(id);
#ifdef SIM_DEBUG
                    cerr << "fire " << fires[i] << "(" << my_towers[ fires[i] ].x << "," << my_towers[ fires[i] ].y << ")  -->" << id << "(" << cur_enermys[id].x << "," << cur_enermys[id].y << ") " << get_power(my_towers[ fires[i] ].type, my_towers[ fires[i] ].A) << endl;///
#endif
                    cur_enermys[id].Le -= get_power(my_towers[ fires[i] ].type, my_towers[ fires[i] ].A);
                    if (my_towers[ fires[i] ].type == FREEZE) {
                        cur_enermys[id].Te += 2;
                    }
                    event_t ne;
                    ne.t = e.t + get_recharge_time(my_towers[ fires[i] ].type, my_towers[ fires[i] ].A);
                    ne.act = FIRE;
                    ne.id = fires[i];
                    q.push(ne);
                }
                else {
                    new_fires.push_back(fires[i]);
                }
            }
            fires.clear();
            for (int i = 0; i < new_fires.size(); ++i) {
                fires.push_back(new_fires[i]);
            }

            for (set<int>::iterator it = ops.begin(); it != ops.end(); ++it) {
                int id = *it;
                if (die[id]) continue;
                if (cur_enermys[id].Le > 0) {
                    if (DEFEND == g[ cur_enermys[id].x ][ cur_enermys[id].y ]) {
#ifdef SIM_DEBUG
                        cerr << "oops " << id << " reach (" << cur_enermys[id].x << "," << cur_enermys[id].y << ")" << endl;//
#endif
                        cur_enermys[id].Le = 0;
                        die[id] = true;
                        ++clear_cnt;
                        --life;
                        if (life <= 0) {
                            return life;
                        }
                        //delete from all the sets
                        for (set<int>::iterator jt = fireholes[id].begin(); jt != fireholes[id].end(); ++jt) {
                            //targets[ fireholes[id] ].erase();
                            int tid = *jt;
                            set<target_t>::iterator it = find_in_set(id, targets[tid]);
                            targets[tid].erase(it);
                        }
                        fireholes[id].clear();
                    }
                }
                else {
#ifdef SIM_DEBUG
                    cerr << "wolaa " << id << " be killed at (" << cur_enermys[id].x << "," << cur_enermys[id].y << ")" << endl;//
#endif
                    if (!die[id]) {
                        //TODO: strong corresponding tower
                        ++clear_cnt;
                        die[id] = true;
                    }
                    //delete from all the sets
                    for (set<int>::iterator jt = fireholes[id].begin(); jt != fireholes[id].end(); ++jt) {
                        //targets[ fireholes[id] ].erase();
                        int tid = *jt;
                        set<target_t>::iterator it = find_in_set(id, targets[tid]);
                        targets[tid].erase(it);
                    }
                    fireholes[id].clear();
                }
                if (clear_cnt == cur_enermys.size()) {
#ifdef SIM_DEBUG
                    cerr << "ALL CLEAR " <<  life << endl;///
#endif
                    return life;
                }
            }
        }
        return life;
    }
};
