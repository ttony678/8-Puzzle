# 8-Puzzle

Must compile with C++11. 

Options to insert own 8-puzzle or use a default.

Options to choose between using Manhattan Distance, Misplaced Tile, or No Heuristic.

Example of output:

Welcome to Luis Lopez's 8-Puzzle Solver.
Type '1' to use a default puzzle, or '2' to  enter your own puzzle.

2
        Enter your puzzle, use a zero to represent the blank
        Enter the first row, use space or tabs between numbers  	1 2 3
        Enter the second row, use space or tabs between numbers 	4 0 6
        Enter the third row, use space or tabs between numbers  	7 5 8


        Enter your choice of algorithm
           1. Uniform Cost Search
           2. A* with the Misplaced Tile Heuristic
           3. A* with the Manhattan Distance Heuristic

           3

Expanding state:
1 2 3
4 ^ 6
7 5 8

Best state to expand with g(n) = 1 and h(n) = 1 is...
1 2 3
4 5 6
7 ^ 8
Expanding this node...

Goal!!

To solve this problem, the search expanded a total of 2 nodes.
The maximum number of nodes in the queue at any one time was 6.
The depth of the goal was 2.

