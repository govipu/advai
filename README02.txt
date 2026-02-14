Compile using g++:

g++ assg02.cpp -o assg02

Part 1: Earliest Completion Day

Find the minimum number of days required to finish all assignments.

./assg02 <input_file> earliest <N> <K>


N → number of students

K → prompts available per student per day

Example:
./assg02 input.txt earliest 3 4


Part 2: Minimum Subscription (Best K)

Find the minimum prompts per student per day required to finish within given days.

./assg02 <input_file> bestK <N> <DAYS>


N → number of students

DAYS → maximum allowed days

Example:
./assg02 input.txt bestK 3 5