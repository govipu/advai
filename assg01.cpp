#include<bits/stdc++.h>

using namespace std;

struct Assignment {
    int id;
    int promptCount;
    vector<int> dependencies;
    int level;
};

typedef vector<vector<vector<int>>> Schedule;

int N, K, numDays;
vector<Assignment> assignments;
vector<Schedule> allSchedules;
set<string> uniqueSchedules;

long long totalScheduleCount = 0;
const int MAX_SCHEDULES_TO_STORE = 20;
long long nodeVisits = 0;


void calculateLevels() {
    vector<int> levels(assignments.size(), 0);

    for (int iter = 0; iter < assignments.size(); iter++) {
        for (int i = 0; i < assignments.size(); i++) {
            for (int dep : assignments[i].dependencies) {
                levels[i] = max(levels[i], levels[dep] + 1);
            }
        }
    }

    for (int i = 0; i < assignments.size(); i++) {
        assignments[i].level = levels[i];
    }
}

string scheduleToString(const Schedule& schedule) {
    string result;
    for (size_t day = 0; day < schedule.size(); day++) {
        for (size_t student = 0; student < schedule[day].size(); student++) {
            vector<int> sorted = schedule[day][student];
            sort(sorted.begin(), sorted.end());
            for (int assign : sorted) {
                result += to_string(day) + "," +
                          to_string(student) + "," +
                          to_string(assign) + ";";
            }
        }
    }
    return result;
}

bool canDoAssignment(int assignmentIdx, const vector<bool>& completed) {
    for (int dep : assignments[assignmentIdx].dependencies) {
        if (!completed[dep]) return false;
    }
    return true;
}

vector<int> getAvailableAssignments(const vector<bool>& completed) {
    vector<int> available;

    for (size_t i = 0; i < assignments.size(); i++) {
        if (!completed[i] && canDoAssignment(i, completed)) {
            available.push_back(i);
        }
    }

    sort(available.begin(), available.end(), [](int a, int b) {
        if (assignments[a].level != assignments[b].level)
            return assignments[a].level < assignments[b].level;
        return assignments[a].promptCount < assignments[b].promptCount;
    });

    return available;
}

bool allCompleted(const vector<bool>& completed) {
    for (bool c : completed)
        if (!c) return false;
    return true;
}

bool isPossible(const vector<bool>& completed, int currentDay, int assignmentsLeft) {
    if (assignmentsLeft == 0) return true;

    int daysLeft = numDays - currentDay;
    if (daysLeft <= 0) return false;

    int totalPromptsNeeded = 0;
    for (size_t i = 0; i < completed.size(); i++) {
        if (!completed[i]) {
            totalPromptsNeeded += assignments[i].promptCount;
        }
    }

    int totalCapacity = daysLeft * N * K;
    return totalPromptsNeeded <= totalCapacity;
}

void findSchedulesOptimized(vector<bool> completed, int day,
                            Schedule schedule,
                            vector<int> remainingPrompts,
                            int assignmentsLeft) {



    nodeVisits++;

    if (allCompleted(completed)) {
        string schedStr = scheduleToString(schedule);
        if (uniqueSchedules.find(schedStr) == uniqueSchedules.end()) {
            uniqueSchedules.insert(schedStr);
            totalScheduleCount++;

            if (allSchedules.size() < MAX_SCHEDULES_TO_STORE) {
                allSchedules.push_back(schedule);
            }
        }
        return;
    }

    if (day >= numDays) return;

    if (!isPossible(completed, day, assignmentsLeft)) return;

    vector<int> available = getAvailableAssignments(completed);

    if (available.empty()) {
        vector<int> newPrompts(N, K);
        findSchedulesOptimized(completed, day + 1, schedule, newPrompts, assignmentsLeft);
        return;
    }

    int maxToConsider = min((int)available.size(), 2);

    for (int idx = 0; idx < maxToConsider; idx++) {
        int assignIdx = available[idx];
        int promptsNeeded = assignments[assignIdx].promptCount;

        for (int student = 0; student < N; student++) {
            if (remainingPrompts[student] >= promptsNeeded) {

                vector<bool> newCompleted = completed;
                newCompleted[assignIdx] = true;

                vector<int> newPrompts = remainingPrompts;
                newPrompts[student] -= promptsNeeded;

                Schedule newSchedule = schedule;
                while (newSchedule.size() <= (size_t)day) {
                    newSchedule.push_back(vector<vector<int>>(N));
                }

                newSchedule[day][student].push_back(assignIdx);

                findSchedulesOptimized(newCompleted, day,
                                       newSchedule, newPrompts,
                                       assignmentsLeft - 1);
            }
        }
    }

    bool workDone = false;
    for (int p : remainingPrompts) {
        if (p < K) {
            workDone = true;
            break;
        }
    }

    if (workDone) {
        vector<int> newPrompts(N, K);
        findSchedulesOptimized(completed, day + 1, schedule, newPrompts, assignmentsLeft);
    }
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cerr << "Usage: " << argv[0]
             << " <input-filename> <number-of-days>" << endl;
        return 1;
    }

    string filename = argv[1];
    numDays = stoi(argv[2]);

    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Error opening file" << endl;
        return 1;
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty() || line[0] == '%') continue;

        istringstream iss(line);
        char type;
        iss >> type;

        if (type == 'N') {
            iss >> N;
        }
        else if (type == 'K') {
            iss >> K;
        }
        else if (type == 'A') {
            Assignment a;
            iss >> a.id >> a.promptCount;

            int dep;
            while (iss >> dep && dep != 0) {
                a.dependencies.push_back(dep - 1);
            }

            a.level = 0;
            assignments.push_back(a);
        }
    }

    calculateLevels();

    vector<bool> completed(assignments.size(), false);
    vector<int> remainingPrompts(N, K);
    Schedule schedule;
    schedule.push_back(vector<vector<int>>(N));

    findSchedulesOptimized(completed, 0, schedule,
                           remainingPrompts, assignments.size());

    cout << "Total valid schedules found: "
         << totalScheduleCount << endl;
    
    if(totalScheduleCount>0)
    cout << "\nShowing first " << allSchedules.size() << " schedules:\n\n";

    int schedNum = 1;
    for (const auto& sched : allSchedules) {
        cout << "Schedule " << schedNum++ << ":\n";

        for (size_t day = 0; day < sched.size(); day++) {
            bool dayHasWork = false;

            for (size_t student = 0; student < sched[day].size(); student++) {
                if (!sched[day][student].empty()) {
                    dayHasWork = true;
                    break;
                }
            }

            if (!dayHasWork) continue;

            cout << "  Day " << day + 1 << ":\n";
            for (size_t student = 0; student < sched[day].size(); student++) {
                if (!sched[day][student].empty()) {
                    cout << "    Student " << student + 1 << ": ";
                    for (int idx : sched[day][student]) {
                        cout << "A" << assignments[idx].id << " ";
                    }
                    cout << "\n";
                }
            }
        }
        cout << "\n";
    }

    return 0;
}
