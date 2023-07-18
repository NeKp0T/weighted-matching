#include <algorithm>
#include <bitset>
#include <iostream>
#include <random>
#include <set>
#include <vector>
#include <cstdlib>

// #define FAST_ALLOCATOR_MEMORY 256000000
#include "optimization.h"
#include <bits/stdc++.h>

using namespace std;

// #define USE_INT128
// #define int int64_t

// #undef _DEBUG

#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()

#ifdef USE_INT128
#define lint __int128
#define ulint __int128
#else
#define lint int64_t
#define ulint int64_t
#endif

typedef long double ld;
// #define f first
// #define s second
typedef pair<int, int> pint;

const int inf = 1 << 30;

template <class T>
T mineq(T& a, const T& b) {
    return a = min(a, b);
}

template <class T>
T maxeq(T& a, const T& b) {
    return a = max(a, b);
}

mt19937 rd(533);

// ---------------------------------------------------------------

double eps = 1e-5;

// ---------- STRUCTS ----------

struct Vertex;

struct Blossom {
    double z;
    Vertex* base;
    bool removed = false;
};

struct Task {
    Vertex* v;
    bool s;
};

struct Label {
    Vertex* a = nullptr;
    Vertex* b = nullptr;

    void clear() {
        a = b = nullptr;
    }

    bool present() { return a != nullptr; }
};

struct Edge {
    double w;
    Vertex* p;
};

struct Vertex {
    vector<Edge> edges;

    vector<int> blossoms;
    double u = 0;

    Vertex* mate = nullptr;

    Label s;
    Label t;

    // iteration info
    Vertex* base();

    int pos; // for debug
};

struct Graph {

    Graph(int n)
        : v(n) {
        for (int i = 0; i < n; i++) v[i].pos = i + 1; // for debug
    }

    void add_edge(Vertex* a, Vertex* b, double w) {
        a->edges.push_back(Edge { w, b });
        b->edges.push_back(Edge { w, a });
    }

    vector<Vertex> v;

    vector<Blossom> blossoms;

    Vertex& operator[](size_t ind) { return v[ind]; }

    int pos(Vertex* vertex) {
        return (vertex - &v[0]);
    }

} g(0);

Vertex* Vertex::base() {
    if (blossoms.empty()) return this;
    return g.blossoms[blossoms.back()].base;
}

// ------ HELPER FUNCTIONS ------

double edge_sum(Vertex* v, Edge& e) {
    Vertex* u = e.p;
    return v->u + u->u - e.w;
}

bool edge_good(Vertex* v, Edge& e) {
    return abs(edge_sum(v, e)) < eps;
}

inline void queue_push_s(queue<Task>& q, Vertex* v) {
    for (Vertex& k : g.v) {
        if (k.base() == v)
            q.push(Task { &k, true });
    }
}

inline void queue_push_t(queue<Task>& q, Vertex* v) {
    q.push(Task { v, false });
}

// ------------ ROOT ------------

Vertex* root_t(Vertex* v);

Vertex* root_s(Vertex* v) {
    if (v->s.a == v) return v;
    return root_t(v->s.a);
}

Vertex* root_t(Vertex* v) {
    if (v->t.b == nullptr) {
        return root_s(v->t.a);
    } else {
        return root_s(v->t.a); // t.a in both cases
    }
}

// -------- BACKTRACING --------

void backtrack_t(vector<Vertex*>& res, Vertex* v, Vertex* stop);

// backtracks starting with S label (matching edge)
void backtrack_s(vector<Vertex*>& res, Vertex* v, Vertex* stop) {
    if (v == stop) return;
    res.push_back(v);
    if (v == v->s.a) return;
    backtrack_t(res, v->s.a, stop);
}

void backtrack_t(vector<Vertex*>& res, Vertex* v, Vertex* stop) {
    if (v == stop) return;
    res.push_back(v);

    if (v->t.b == nullptr) {
        backtrack_s(res, v->t.a, stop);
    } else {
        vector<Vertex*> t;
        backtrack_s(t, v->t.b, v);
        // add reversed t to  the back of res
        copy(t.rbegin(), t.rend(), insert_iterator<vector<Vertex*>>(res, res.end()));
        backtrack_s(res, v->t.a, stop);
        // POTENTIAL N^2 INSTEAD OF N
    }
}

// ------- AUGMENTATION --------

bool in_blossom(Vertex* v, int b_id) {
    return find(all(v->blossoms), b_id) != v->blossoms.end();
}

bool edge_inside(Vertex *v, Edge& edge, int b_id) {
    Vertex* u = edge.p;

    double whole_w = v->u + u->u;
    for (int b : v->blossoms) {
        if (in_blossom(u, b)) {
            whole_w += g.blossoms[b].z;
        }
    }

    return (abs(whole_w - edge.w) < eps)
        && in_blossom(v, b_id) && in_blossom(u, b_id);
}

void relabel(vector<Vertex*>& path, Vertex* mate) {
    map<int, int> old_base; // blossom id -> position in path
    map<int, int> new_base; // blossom id -> position in path
    map<int, Vertex*> old_b_mate; // blossom id -> Vertex
    map<int, Vertex*> new_b_mate; // blossom id -> Vertex
    for (size_t i = 0; i < path.size(); i++) {
        for (int b_id : path[i]->blossoms) {
            old_base[b_id] = i;
            old_b_mate[b_id] = i + 1 < path.size() ? path[i + 1] : nullptr;
            if (!new_base.count(b_id)) {
                new_base[b_id] = i;
                new_b_mate[b_id] = i >= 1 ? path[i-1] : mate;
            }
        }
    }

    vector<int> bloss;
    for (auto& p : old_base) bloss.push_back(p.first);

    for (auto& b_id : bloss) {
        if (path[old_base[b_id]] != g.blossoms[b_id].base) {
            swap(old_base[b_id], new_base[b_id]);
        }
        if (new_b_mate[b_id] != path[new_base[b_id]]->mate) {
            swap(old_b_mate[b_id], new_b_mate[b_id]);
        }
        if (new_b_mate[b_id] != path[new_base[b_id]]->mate) {
            exit(24);
        }
    }

    // *_base built, .base updated


    set<int> special; // all starts and ends of blossoms;
    for (auto p : old_base) {
        special.insert(p.second);
    }
    for (auto p : new_base) {
        special.insert(p.second);
    }

    vector<Label> news(path.size()), newt(path.size());
    for (int i = 0; i < static_cast<int>(path.size()); i++) {
        Vertex* v = path[i];
        if (!special.count(i)) {
            // makes sense if inside
            // otherwise it will be erased anyway
            news[i] = v->t;
            newt[i] = v->s; 
        } else {
            int blos = v->blossoms[0];
            Vertex* old_b = path[old_base[blos]];
            Vertex* new_b = path[new_base[blos]];

            if (new_base[blos] == i) {
                Vertex* ii = v->mate;
                news[i].a = ii;

                if (i != new_base[v->blossoms.back()]) {
                    int b_out = -1;
                    for (int bid : ii->blossoms) {
                        if (in_blossom(v, bid)) {
                            b_out = bid;
                            break;
                        }
                    }

                    if (b_out == -1) {
                        // new base node
                    } else {
                        if (old_base[b_out] != old_base[blos]) {
                            newt[i].a = old_b_mate[blos];
                            newt[i].b = path[old_base[blos]];
                        } else {
                            // if old base of b_out

                            Vertex* b_out_new_base_base = path[new_base[b_out]];
                            for (int iii = 0;; iii++) {
                                if (b_out_new_base_base->blossoms[iii] == b_out) {
                                    break;
                                }
                                b_out_new_base_base = g.blossoms[b_out_new_base_base->blossoms[iii]].base;
                            }
                            vector<Vertex*> pt;
                            backtrack_t(pt, b_out_new_base_base, old_b);

                            Vertex* last = old_b;
                            while (in_blossom(pt.back(), blos)) {
                                last = pt.back();
                                pt.pop_back();
                            }
                            Vertex* jj = pt.back();
                            newt[i].a = jj;
                            newt[i].b = last;
                        }
                    }
                }
            } else if (old_base[blos] == i) {
                news[i].a = v->mate;

                Vertex* newb_base = new_b;
                for (int i = 0;; i++) {
                    if (new_b->blossoms[i] == blos) {
                        break;
                    }
                    newb_base = g.blossoms[new_b->blossoms[i]].base;
                }

                vector<Vertex*> pt;
                backtrack_t(pt, newb_base, old_b);
                Vertex* jj = pt.back();
                newt[i].a = jj;

            } else {
                exit(20);
            }
        }
    }

    for (int i = 0; i < path.size(); i++) {
        path[i]->s = news[i];
        path[i]->t = newt[i];
    }
    for (auto& b_id : bloss) {
        g.blossoms[b_id].base = path[new_base[b_id]];
    }
}

void reverse_mate_s(Vertex* top, Vertex* mate) {
    vector<Vertex*> path;
    backtrack_s(path, top, nullptr);

    top->mate = mate;
    for (int i = 1; i < path.size(); i += 2) {
        if (i + 1 == path.size()) exit(10); // !@#
        path[i]->mate = path[i + 1];
        path[i + 1]->mate = path[i];
    }

    relabel(path, mate);
}

void reverse_mate_t(Vertex* top, Vertex* mate) {
    vector<Vertex*> path;
    backtrack_t(path, top, nullptr);

    for (int i = 0; i < path.size(); i += 2) {
        if (i + 1 == path.size()) exit(11); // !@#
        path[i]->mate = path[i + 1];
        path[i + 1]->mate = path[i];
    }

    relabel(path, mate); // works?? I dont think it sould use mate in this case, mb put checker
}

// -------- BASE RECALC --------

void set_base(vector<Vertex*> pathv, vector<Vertex*> pathu, int m) {
    vector<bool> was(g.v.size(), false);
    for (int i = m + 1; i < pathv.size(); i++) {
        was[g.pos(pathv[i]->base())] = true;
    }
    for (int i = m + 1; i < pathu.size(); i++) {
        was[g.pos(pathu[i]->base())] = true;
    }
    was[g.pos(pathv[m]->base())] = true;

    Blossom newB { 0, pathv[m]->base() };
    g.blossoms.push_back(newB);
    int new_b_idx = g.blossoms.size() - 1;

    for (Vertex& i : g.v) {
        if (was[g.pos(i.base())])
            i.blossoms.push_back(new_b_idx); // i.base = pathv[m];
    }

    // if (find(all(pathv[m]->base()->blossoms), new_b_idx) == pathv[m]->base()->blossoms.end()) {
    //     exit(210);
    // }
}

// --------- BLOSSOMING ---------

void add_labels_s(queue<Task>& q, Vertex* v, vector<Vertex*>& path, int m, Vertex* u) {
    if (m == path.size() - 1) return;

    for (int i = m + 1;;) {
        if (!path[i]->s.present()) {
            path[i]->s.a = path[i + 1];
            queue_push_s(q, path[i]);
        }

        i++;
        if (i == path.size() - 1) break;
        if (i + 1 >= path.size()) exit(12); // !@#

        if (!path[i]->t.present()) {
            if (!path[i + 1]->s.present()) {
                path[i]->t.a = path[i + 1];
            } else {
                int k = path.size() - 1;
                if (path[k]->base() == path[i]->base()) {
                    path[i]->t.a = u;
                    path[i]->t.b = v;
                } else {
                    while (path[k]->base() != path[i]->base()) k--;
                    path[i]->t.a = path[k + 1];
                    path[i]->t.b = path[k];
                }
            }
            queue_push_t(q, path[i]);
        }

        i++;
    }
}

void blossom_s(queue<Task>& q, Vertex* v, Vertex* u) {
    vector<Vertex*> pathv, pathu;
    backtrack_s(pathv, v, nullptr);
    backtrack_s(pathu, u, nullptr);
    reverse(all(pathv));
    reverse(all(pathu));
    int m = 0;
    while (m < pathv.size() && m < pathu.size() && pathv[m] == pathu[m]) m++;
    m--;

    add_labels_s(q, v, pathv, m, u);
    add_labels_s(q, u, pathu, m, v);

    Vertex* lca = pathv[m];

    if (!v->t.present() && v != lca) {
        v->t.a = u;
        queue_push_t(q, v);
    }
    if (!u->t.present() && u != lca) {
        u->t.a = v;
        queue_push_t(q, u);
    }

    set_base(pathv, pathu, m);
}

void add_labels_t(queue<Task>& q, Vertex* v, vector<Vertex*>& path, int m) {
    for (int i = m + 1; i < path.size() - 1;) {
        if (!path[i]->s.present()) {
            path[i]->s.a = path[i + 1];
            queue_push_s(q, path[i]);
        }

        i++;

        if (!path[i]->t.present()) {
            if (!path[i + 1]->s.present()) {
                path[i]->t.a = path[i + 1];
            } else {
                if (path[i] != path[i]->base()) exit(14); // !@#
                int k = path.size() - 1;
                while (path[k]->base() != path[i]->base()) k--;
                path[i]->t.a = path[k + 1];
                path[i]->t.b = path[k];
            }
            queue_push_t(q, path[i]);
        }

        i++;
    }
}

void blossom_t(queue<Task>& q, Vertex* v, Vertex* u) {
    vector<Vertex*> pathv, pathu;
    backtrack_t(pathv, v, nullptr);
    backtrack_t(pathu, u, nullptr);
    reverse(all(pathv));
    reverse(all(pathu));
    int m = 0;
    while (pathv[m] == pathu[m] && m < pathv.size() && m < pathu.size()) m++;
    m--;

    add_labels_t(q, v, pathv, m);
    add_labels_t(q, u, pathu, m);

    Vertex* lca = pathv[m];

    if (!v->s.present() && v != lca) {
        v->s.a = u;
        queue_push_s(q, v);
    }
    if (!u->s.present() && u != lca) {
        u->s.a = v;
        queue_push_s(q, u);
    }
    set_base(pathv, pathu, m);
}

// ------ EDGE RELAXATION ------

bool relax_s(std::queue<Task>& q, Vertex* v, double& Delta) {
    bool aug = false;

    for (auto edge : v->edges) {
        Vertex* u = edge.p;
        double w = edge_sum(v, edge);
        bool w_zero = edge_good(v, edge);

        if (u == v->mate || v->base() == u->base()) {
            continue;
        }

        if (u->base()->s.present() && w_zero) {
            Vertex* rootu = root_s(u);
            Vertex* rootv = root_s(v);
            if (rootu != rootv) {
                // aug path

                reverse_mate_s(v, u);
                reverse_mate_s(u, v);

                aug = true;

                return true;
            } else {
                // blossom

                blossom_s(q, u, v);

                continue;
            }
        }
        if (u->base()->s.present() && !w_zero) {
            mineq(Delta, w / 2);
            continue;
        }
        if (!u->base()->s.present() && !u->base()->t.present() && w_zero) {
            u->base()->t.a = v;
            u->base()->t.b = u;
            queue_push_t(q, u->base());
            continue;
        }
        if (!u->base()->s.present() && !u->base()->t.present() && !w_zero) {
            mineq(Delta, w);
            continue;
        }
    }
    return aug;
}

bool relax_t(std::queue<Task>& q, Vertex* v) {
    if (v->mate != nullptr) {
        Vertex* u = v->mate;
        if (u->base() == v->base()) {
            return false;
        }
        if (u->t.present()) {
            Vertex* rootu = root_t(u);
            Vertex* rootv = root_t(v);
            if (rootu != rootv) {
                // aug path

                reverse_mate_t(v, u);
                reverse_mate_t(u, v);

                return true;
            } else {
                // blossom

                blossom_t(q, u, v);

                return false;
            }
        } else {
            u->s.a = v;
            queue_push_s(q, u);
        }
        // ?? should realy overwrite label there ^ ? Does it really overwrite??
        // if (!u->s.present()) {
        //     u->s.a = v;
        //     queue_push_s(q, u);
        //     return false;
        // }
    }
    return false;
}

// ----------- CYCLE -----------

void debug_out() {
    for (Vertex& v : g.v) {
        cerr << v.pos << ": \nu = " << v.u << "\n";
        cerr << "Blossoms: ";
        for (int i : v.blossoms) 
            cerr << i << ' ';
        cerr << "\nMate: ";
        if (v.mate != nullptr) {
            cerr << v.mate->pos;
        } else {
            cerr << "no ";
        }
        cerr << endl << endl;
    }
    for (int b_id = 0; b_id < g.blossoms.size(); b_id++) {
        auto& b = g.blossoms[b_id];
        if (!b.removed) {
            cerr << "Blossom " << b_id << ": z = " << b.z << "\n";
        }
    }
}

inline bool bfs(std::queue<Task>& q, double& Delta) {

    while (!q.empty()) {
        Task t = q.front();
        q.pop();

        Vertex* v = t.v;
        if (t.s) {
            if (relax_s(q, v, Delta)) return true;
        } else {
            if (relax_t(q, v)) return true;
        }
    }

    return false;
}

bool is_inner(int bl_id) {
    bool inner = false;
    for (auto& v : g.v) {
        if (find(all(v.blossoms), bl_id) != v.blossoms.end()) {
            inner |= v.blossoms.back() != bl_id;
        }
    }
    return inner;
}

void init_queue(queue<Task> &q) {
    q = queue<Task>();
    
    for (auto& v : g.v) {
        while (!v.blossoms.empty() && g.blossoms[v.blossoms.back()].z < eps) {
            g.blossoms[v.blossoms.back()].removed = true;
            v.blossoms.pop_back();
        }
    }

    for (auto& v : g.v) {
        if (v.base() == &v) {
            v.s.clear();
            v.t.clear();
            if (v.mate == nullptr) {
                v.s.a = &v;
                queue_push_s(q, &v);
            }
        }
    }
}

void solve() {
#ifndef _DEBUG
    // match_greedy();
#endif

    int covered = 0;

    // 1.0
    double Delta = inf;
    queue<Task> q;

    while (true) {
        init_queue(q);

        // 1.1
        if (bfs(q, Delta)) {
            Delta = inf;

            // 1.0
            continue;
        } else {

            double d1 = inf;
            for (auto& v : g.v) {
                if (v.base()->s.present())
                mineq(d1, v.u);
            }
            double d2 = inf;
            for (int bl_id = 0; bl_id < g.blossoms.size(); bl_id++) {
                auto& bl = g.blossoms[bl_id];
                if (!is_inner(bl_id) && !bl.removed && bl.base->t.present() && !bl.base->s.present()) {
                    mineq(d2, bl.z / 2);
                }
            }

            double d = min(min(d1, d2), Delta);

            #ifdef INP
            debug_out();
            cerr << endl << endl << endl;
            #endif

            // d == d1
            if (d >= d1 - eps) {
                return;
            }

            for (auto& v : g.v) {
                if (v.base()->s.present()) v.u -= d;
                if (v.base()->t.present()) v.u += d;
            }

            for (int bl_id = 0; bl_id < g.blossoms.size(); bl_id++) {
                auto& bl = g.blossoms[bl_id];
                if (!bl.removed) {
                    if (!is_inner(bl_id)) {
                        if (bl.base->t.present() && !bl.base->s.present())
                            bl.z -= 2 * d;
                        if (bl.base->s.present() && !bl.base->t.present())
                            bl.z += 2 * d;
                    }
                }
            }

            // d == d2
            if (d >= d2 - eps) {
                for (auto& v : g.v) {

                    bool was_base = &v == v.base();
                    bool expanded = false;
                    while (!v.blossoms.empty()) {
                        auto& lst = g.blossoms[v.blossoms.back()];
                        if (lst.z < eps) {
                            expanded = true;
                            lst.removed = true;
                            v.blossoms.pop_back();
                        } else
                            break;
                    }

                    bool now_base = &v == v.base();
                    if (expanded && now_base) {
                        v.s.clear();
                        v.t.clear();
                    }
                }
            }

            Delta = inf;
        }
    }
}

// ------------ MAIN ------------

// DISCLAIMER 
// * positive edges only
// * max weight matching
// * don't forget to set each v.u to maxweight/2 for some reason
// * sorry for global graph g

int32_t main() {
    srand(5332017);
    ios::sync_with_stdio(false);
    cin.tie();
    cout.tie();
    cout << fixed;
    cout.precision(4);

#ifdef INP
    freopen("schedule.in", "r", stdin);
#endif

    g = Graph(readInt());

    double maxw = 0;
    while (!isEof()) {
        int a, b;
        double w = 1;
        a = readInt() - 1;
        b = readInt() - 1;
        #ifdef WEIGHT
        w = readDouble();
        #endif
        g.add_edge(&g[a], &g[b], w);
        maxeq(maxw, w);
    }
    for (auto& v : g.v) v.u = maxw / 2;

    solve();

    vector<pint> matching;
    double sum_w = 0;
    for (int i = 0; i < g.v.size(); i++) {
        if (g[i].mate != nullptr && i < g.pos(g[i].mate)) {
            matching.push_back({ i, g.pos(g[i].mate) });
            for (auto& e : g[i].edges)
                if (e.p == g[i].mate)
                    sum_w += e.w;
        }
    }

    cout << fixed;
    cout.precision(3);
    cout << matching.size() * 2;
#ifdef WEIGHT
    cout << ' ' << sum_w;
#endif

    cout << '\n';
    for (auto& p : matching) cout << p.first + 1 << ' ' << p.second + 1 << '\n';

    return 0;
}

/*
    TODO check Ctrl+F ??
*/
