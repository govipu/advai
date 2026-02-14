#include <bits/stdc++.h>
using namespace std;

struct Task {
    int id;
    int cost;
    vector<int> prereq;
    int depth;
};

vector<Task> tasks;
int N, K, D;
long long found;

void buildDepth() {
    int m = tasks.size();
    vector<int> dp(m, 0);

    for (int r = 0; r < m; r++) {
        for (int i = 0; i < m; i++) {
            for (int p : tasks[i].prereq) {
                dp[i] = max(dp[i], dp[p] + 1);
            }
        }
    }

    for (int i = 0; i < m; i++)
        tasks[i].depth = dp[i];
}

bool allowed(int t, int day, vector<int>& doneDay) {
    for (int p : tasks[t].prereq) {
        if (doneDay[p] == -1) return false;
        if (doneDay[p] >= day) return false;
    }
    return true;
}

bool possible(vector<int>& doneDay, int day) {
    int need = 0;
    for (int i = 0; i < tasks.size(); i++)
        if (doneDay[i] == -1)
            need += tasks[i].cost;

    int left = D - day;
    return need <= left * N * K;
}

bool finished(vector<int>& doneDay) {
    for (int x : doneDay)
        if (x == -1) return false;
    return true;
}

void search(int day,
            vector<int>& doneDay,
            vector<int>& capacity) {

    if (found) return;

    if (finished(doneDay)) {
        found = 1;
        return;
    }

    if (day >= D) return;
    if (!possible(doneDay, day)) return;

    vector<int> ready;
    for (int i = 0; i < tasks.size(); i++)
        if (doneDay[i] == -1 && allowed(i, day, doneDay))
            ready.push_back(i);

    sort(ready.begin(), ready.end(),
        [](int a, int b) {
            if (tasks[a].depth != tasks[b].depth)
                return tasks[a].depth < tasks[b].depth;
            return tasks[a].cost < tasks[b].cost;
        });

    bool didWork = false;

    for (int t : ready) {
        for (int s = 0; s < N; s++) {
            if (capacity[s] >= tasks[t].cost) {

                didWork = true;
                doneDay[t] = day;
                capacity[s] -= tasks[t].cost;

                search(day, doneDay, capacity);

                doneDay[t] = -1;
                capacity[s] += tasks[t].cost;
            }
        }
    }

    if (!didWork ||
        any_of(capacity.begin(), capacity.end(),
               [](int x) { return x < K; })) {

        vector<int> nextCap(N, K);
        search(day + 1, doneDay, nextCap);
    }
}

bool canSchedule() {
    found = 0;
    vector<int> done(tasks.size(), -1);
    vector<int> cap(N, K);
    search(0, done, cap);
    return found;
}

void load(const string& file) {
    ifstream in(file);
    if (!in) {
        cerr << "Error: input file not found\n";
        exit(1);
    }

    string line;
    while (getline(in, line)) {
        if (line.empty() || line[0] == '%') continue;

        istringstream iss(line);
        char c;
        iss >> c;

        if (c == 'A') {
            Task t;
            iss >> t.id >> t.cost;

            int x;
            while (iss >> x && x != 0)
                t.prereq.push_back(x - 1);

            tasks.push_back(t);
        }
    }

    if (tasks.empty()) {
        cerr << "Error: no assignments in input\n";
        exit(1);
    }

    buildDepth();
}

int main(int argc, char* argv[]) {

    if (argc != 5) {
        cerr << "Usage:\n"
             << "  assg02 <input> earliest <N> <K>\n"
             << "  assg02 <input> bestK   <N> <DAYS>\n";
        return 1;
    }

    string file = argv[1];
    string mode = argv[2];
    N = stoi(argv[3]);

    load(file);

    if (mode == "earliest") {
        K = stoi(argv[4]);
        for (D = 1; D <= 100; D++) {
            if (canSchedule()) {
                cout << "Earliest completion: "
                     << D << " days\n";
                return 0;
            }
        }
        cout << "Not possible\n";
    }
    else if (mode == "bestK") {
        D = stoi(argv[4]);
        for (K = 1; K <= 100; K++) {
            if (canSchedule()) {
                cout << "Minimum prompts per student per day: "
                     << K << "\n";
                return 0;
            }
        }
        cout << "Not possible\n";
    }
    else {
        cerr << "Invalid mode\n";
        return 1;
    }

    return 0;
}
