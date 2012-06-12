#include "path.h"

//#define DEBUG
//#define MINIDEBUG

struct target_t {
    int id;
    int t; //enter time
    int at; //apear time
    bool operator<(const target_t &other) const {
        if (t > other.t) return true;
        else if (t < other.t) return false;
        if (at > other.at) return true;
        else if (at < other.at) return false;
        return (id < other.id);
    }
    bool operator==(const target_t &other) const {//FIXME:
        return id == other.t;
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

int get_life(int life) {
    memset(shooted, false, sizeof(shooted));
    //find ways
//    find_path();
//    get_path();
    //get towrs
#if 0
    my_towers.clear();
    for (int i = 0; i < init_towers.size(); ++i) {//FIXME: how about be canceled?
        tower_t tower;
        tower.x = init_towers[i].x, tower.y = init_towers[i].y, tower.type = init_towers[i].type, tower.A = init_towers[i].A;
        my_towers.push_back(tower);
    }
    for (int i = 0; i < solutions.size(); ++i) { //FIXME:
        tower_t tower;
        tower.x = solutions[i].x, tower.y = solutions[i].y, tower.type = solutions[i].type, tower.A = solutions[i].A;
        my_towers.push_back(tower);
    }
#endif
    priority_queue<event_t> q;
    //init targets
    for (int i = 0; i < my_towers.size(); ++i) {
        targets[i].clear();
        event_t e;
        e.t = 1;
        e.act = FIRE;
        e.id = i;
        q.push(e);
#ifdef DEBUG
        cerr << "tower " << my_towers[i].x << "," << my_towers[i].y << endl;///
#endif
    }
    //init enermys
    cur_enermys.clear();
    for (int i = 0; i < enermys.size(); ++i) {
        enermy_t enermy;
        enermy.x = enermys[i].x, enermy.y = enermys[i].y, enermy.Te = enermys[i].Te, enermy.Le = enermys[i].Le, enermy.Se = enermys[i].Se;
        cur_enermys.push_back(enermy);
        event_t e;
        e.t = enermy.Te;//
        e.act = MOVE;//
        e.id = i;
        q.push(e);
        fireholes[i].clear();
#ifdef DEBUG
        cerr << "enermy " << cur_enermys[i].x << "," << cur_enermys[i].y << " " << cur_enermys[i].Te << " " << cur_enermys[i].Se << endl;///
#endif
    }
    
    bool die[ cur_enermys.size() ];
    memset(die, false, sizeof(die));

    int clear_cnt = 0;
    vector<int> fires;
    fires.clear();
#ifdef DEBUG
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
#ifdef DEBUG
        cerr << ">>>>>>>>>>>>>>>  time=" << e.t << " move=" << moves.size() << " fire=" << fires.size() << endl;///
#endif
        for (int i = 0; i < moves.size(); ++i) {
            int id = moves[i];
#ifdef DEBUG
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
#ifdef DEBUG
            cerr << cur_enermys[id].Te << " move " << id << " (" << cur_enermys[id].x << "," << cur_enermys[id].y << ")"; ////
#endif
            //perform move to new position
            if (false/*e.t == enermys[id].Te*/) {//first appear
                
            } else {
                //
                pos_t np = path[ cur_enermys[id].x ][ cur_enermys[id].y ];
                if (e.t != enermys[id].Te) {
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
#ifdef DEBUG
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
#ifdef DEBUG
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
#ifdef DEBUG
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
                int id = targets[ fires[i] ].begin()->id;
                shooted[ fires[i] ] = true;
                ops.insert(id);
#ifdef DEBUG
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
        fires = new_fires;
        
        for (set<int>::iterator it = ops.begin(); it != ops.end(); ++it) {
            int id = *it;
            if (die[id]) continue;
            if (cur_enermys[id].Le > 0) {
                if (DEFEND == g[ cur_enermys[id].x ][ cur_enermys[id].y ]) {
#ifdef DEBUG
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
#ifdef DEBUG
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
#ifdef DEBUG
                cerr << "ALL CLEAR " <<  life << endl;///
#endif
                return life;
            }
        }
    }
    return life;
}


