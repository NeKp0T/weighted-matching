#include <bits/stdc++.h>

using namespace std;

struct E {
    int a, b;
    bool has;
};

int main(int argc, char** argv) {
    char* filename = "graph"; //argv[1];
    ifstream fin(filename);

    int n;
    fin >> n;
    vector<E> edges;


    map<pair<int, int>, int> epos;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            edges.push_back(E { i, j, false });
            epos[{ i, j }] = edges.size() - 1;
        }
    }

    int a, b;
    double w;
    int cnt = 0;
    while (fin >> a >> b >> w) {
        --a, --b;
        edges[epos[{a, b}]].has = true;
        cnt++;
    }

    fin.close();

    auto next_graph = [&]() {
        int i = 0;
        while (i < edges.size() && edges[i].has) {
            edges[i].has = false;
            i++;
        }
        if (i == edges.size()) {
            n++;
            edges.resize(0);
            epos.clear();
            edges.resize(n * (n - 1) / 2);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < i; j++) {
                    edges.push_back(E { i, j, false });
                    epos[{ i, j }] = edges.size() - 1;
                }
            }

        } else {
            edges[i].has = true;
        }
    };

    next_graph();

    ofstream fout(filename);

    std::uniform_real_distribution<double> unif(0.1, 0.9);
    std::default_random_engine re;
    re.seed(time(0));

    fout << n << endl;
    for (auto& e : edges) {
        if (e.has) {
            double a_random_double = unif(re);
            fout << e.a + 1 << ' ' << e.b + 1 << ' ' << unif(re) << '\n';
        }
    }
    fout.close();
}