#include <bits/stdc++.h>

using namespace std;

struct E {
    int a, b;
    double w;
};

vector<vector<double>> ew;
vector<int> p;
vector<int> p_best;
double maxw = 0;

void check() {
    double sumw = 0;
    for (int i = 0; i < p.size(); i++) {
        if (p[i] > i)
            sumw += ew[i][p[i]];
    }
    if (maxw < sumw) {
        maxw = sumw;
        p_best = p;
    }
}

void rec(int pos) {
    if (pos == p.size()) {
        check();
        return;
    }
    if (p[pos] < pos) {
        rec(pos + 1);
    } else {
        for (int i = pos + 1; i < p.size(); i++) {
            if (p[i] == i) {
                p[pos] = i;
                p[i] = pos;
                rec(pos + 1);
                p[i] = i;
            }
        }
        p[pos] = pos;
        rec(pos + 1);
    }
}

int main(int argc, char** argv) {
    ifstream fin(argv[1]);

    int n;
    fin >> n;

    ew.resize(n, vector<double>(n));

    {
        int a, b;
        double w;
        while (fin >> a >> b >> w) {
            --a, --b;
            ew[a][b] = w;
            ew[b][a] = w;
        }
    }

    p.resize(n);
    for (int i = 0; i < n; i++) p[i] = i;
    p_best = p;

    rec(0);

    // ------------------

    vector<pair<int, int>> their;
    {
        int k;
        double w_said = 0;
        cin >> k >> w_said;
        double w_res = 0;
        for (int i = 0; i * 2 < k; i++) {
            int a, b;
            cin >> a >> b;
            --a, --b;
            w_res += ew[a][b];
            their.push_back({a, b});
        }

        if (abs(w_said - w_res) > 1e-3) {
            cerr << "wrong sum weight";
            return 3;
        }

        if (w_res < maxw - 1e-5) {
            cerr << "NOT ENOUGH\n";
            cerr << "my was " << maxw << " but " << w_res << " given\n";
            cerr << "MY ANSWER:\n";
            for (int i = 0; i < p_best.size(); i++) {
                if (p_best[i] > i) {
                    cerr << i + 1 << ' ' << p_best[i] + 1 << '\n';
                }
            }
            cerr << "THEIR ANSWER:\n";
            for (auto& i : their) {
                cerr << i.first + 1 << ' ' << i.second + 1 << '\n';
            }

            return 1;
        }
        if (w_res > maxw + 1e-5) {
            cerr << "?????\n";
            return 2;
        }
    }
}