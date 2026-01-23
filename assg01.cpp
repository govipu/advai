#include <bits/stdc++.h>
using namespace std;

struct Assignment {
    int id;
    int promptCount;
    vector<int> dependencies;
    vector<int> dependents;
    int remainingDeps;
    bool completed;
};

unordered_map<int, Assignment> assignments;
int N, K, M;
int totalAssignments = 0;
int completedCount = 0;
int totalValidSchedules = 0; 

struct StudentDay {
    int promptsLeft;
    vector<int> tasks;
};

vector<vector<vector<int>>> currentSchedule; 

void printSchedule(const vector<vector<vector<int>>>& sched) {
    cout << "\nVALID SCHEDULE #" << totalValidSchedules << ":" << endl;
    for (int d = 0; d < sched.size(); ++d) {
        cout << "Day " << d + 1 << ":" << endl;
        for (int s = 0; s < N; ++s) {
            cout << "  Student " << s + 1 << ": ";
            if (sched[d][s].empty()) cout << "-";
            else {
                for (size_t i = 0; i < sched[d][s].size(); ++i)
                    cout << "A" << sched[d][s][i] << (i == sched[d][s].size() - 1 ? "" : ", ");
            }
            cout << endl;
        }
    }
    cout << "-----------------------" << endl;
}

void solve(int day, vector<int> available) {
    if (completedCount == totalAssignments) {
        totalValidSchedules++; 
        printSchedule(currentSchedule);
        return;
    }
    
    if (day > M) return;

    int numAvailable = available.size();
    vector<StudentDay> students(N, {K, {}});
    
    auto distribute = [&](auto self, int taskIdx, bool dayHasActivity) {
        if (taskIdx == numAvailable) {
            if (!dayHasActivity && !available.empty()) return; 

            vector<vector<int>> dayData(N);
            vector<int> finishedToday;
            for(int i=0; i<N; ++i) {
                dayData[i] = students[i].tasks;
                for(int id : students[i].tasks) finishedToday.push_back(id);
            }

            for(int id : finishedToday) {
                assignments[id].completed = true;
                completedCount++;
                for(int dep : assignments[id].dependents) assignments[dep].remainingDeps--;
            }
            currentSchedule.push_back(dayData);

            vector<int> nextAvailable;
            for(auto const& [id, a] : assignments) {
                if(!a.completed && a.remainingDeps == 0) nextAvailable.push_back(id);
            }
            sort(nextAvailable.begin(), nextAvailable.end());
            
            solve(day + 1, nextAvailable);

            currentSchedule.pop_back();
            for(int id : finishedToday) {
                assignments[id].completed = false;
                completedCount--;
                for(int dep : assignments[id].dependents) assignments[dep].remainingDeps++;
            }
            return;
        }

        int aid = available[taskIdx];
        int cost = assignments[aid].promptCount;

        set<int> seenPrompts; 
        for (int i = 0; i < N; ++i) {
            if (students[i].promptsLeft >= cost && seenPrompts.find(students[i].promptsLeft) == seenPrompts.end()) {
                seenPrompts.insert(students[i].promptsLeft);
                students[i].promptsLeft -= cost;
                students[i].tasks.push_back(aid);
                self(self, taskIdx + 1, true);
                students[i].tasks.pop_back();
                students[i].promptsLeft += cost;
            }
        }
        self(self, taskIdx + 1, dayHasActivity);
    };

    distribute(distribute, 0, false);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: <exe> <input_file> <num_days>" << endl;
        return 1;
    }
    M = stoi(argv[2]);
    ifstream fin(argv[1]);
    if (!fin) {
        cout << "Error opening file." << endl;
        return 1;
    }

    string line;
    while (getline(fin, line)) {
        if (line.empty() || line[0] == '%') continue;
        stringstream ss(line);
        char type; ss >> type;
        if (type == 'N') ss >> N;
        else if (type == 'K') ss >> K;
        else if (type == 'A') {
            int id, pc, dep;
            ss >> id >> pc;
            Assignment a; a.id = id; a.promptCount = pc; a.completed = false;
            while (ss >> dep && dep != 0) a.dependencies.push_back(dep);
            assignments[id] = a;
        }
    }

    for (auto &p : assignments) {
        for (int d : p.second.dependencies) assignments[d].dependents.push_back(p.first);
        p.second.remainingDeps = p.second.dependencies.size();
    }
    totalAssignments = assignments.size();

    vector<int> initial;
    for (auto const& [id, a] : assignments) if (a.remainingDeps == 0) initial.push_back(id);
    sort(initial.begin(), initial.end());

    solve(1, initial);

    if (totalValidSchedules == 0) {
        cout << "No schedule possible" << endl;
    }
    cout << "Total number of valid schedules: " << totalValidSchedules << endl;

    return 0;
}