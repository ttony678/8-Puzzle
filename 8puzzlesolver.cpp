/*
 * Luis Lopez
 * SID 861-236-618
 * 8-Puzzle Project
 *
 * CS 170: Intro to Artificial Intelligence
 * Instructor: Eamonn Keogh
 * DUE: November 3rd, 2017
 */

#include <unordered_map>
#include <iostream>
#include <functional>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <cmath>
using namespace std;

// Abstraction
const int SIZE = 3;

// Ending Condition
const int goal[3][3]   = { { 1, 2, 3 }, 
                           { 4, 5, 6 }, 
                           { 7, 8, 0 } };

// Test Cases
int vEasy[3][3]        = { { 1, 2, 3 }, 
                           { 4, 5, 6 }, 
                           { 7, 0, 8 } };

int easy[3][3]         = { { 1, 2, 0 }, 
                           { 4, 5, 3 }, 
                           { 7, 8, 6 } };

int doable[3][3]       = { { 0, 1, 2 }, 
                           { 4, 5, 3 }, 
                           { 7, 8, 6 } };

int ohboy[3][3]        = { { 8, 7, 1 }, 
                           { 6, 0, 2 }, 
                           { 5, 4, 3 } };

int impossible[3][3]   = { { 1, 2, 3 }, 
                           { 4, 5, 6 }, 
                           { 8, 7, 0 } };


struct Node {
    int g;                  // Holds g(n) for uniform function
    int h;                  // Holds h(n) for heuristic function
    int puzzle[SIZE][SIZE]; // Points to puzzle 
    Node* parent;           // Points to parent for backtrace

    Node() {
        g = 0;
        h = 0;
        parent = NULL;

        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                puzzle[i][j] = 0;
            }
        }
    }

    // Used to create the first Node.
    Node(int g, int h, int newPuzzle[][SIZE], Node* p) 
            : g(g), h(h), parent(p) {
 
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                puzzle[i][j] = newPuzzle[i][j];
            }
        }
    }

    // Used to create more Nodes easily
    Node(Node* newNode) : g(newNode->g), h(newNode->h), parent(newNode){
  
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                puzzle[i][j] = newNode->puzzle[i][j];
            }
        }
    }

    bool CheckGoal() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j< SIZE; j++) {
                if (puzzle[i][j] != goal[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    ~Node() { }
};

// Comparison function to set priority queue ordering.
struct Least {
    bool operator()(Node *lhs, Node *rhs) {
        return (lhs->g + lhs->h) > (rhs->g + rhs->h);
    }
};

// Overriding the hash function by creating a unique string
// for each Node depending on the puzzle.
struct KeyHasher {
    size_t operator() (const Node& n) const  {
        string temp;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                string s = to_string(n.puzzle[i][j]);
                temp += s;
            }
        }
        return hash<string>{}(temp);
    }
};

// Used to test equivalence of Node*'s for the hash table.
struct Equal {
    bool operator()(Node* lhs, Node* rhs) const {
        return ( lhs->puzzle[0][0] == rhs->puzzle[0][0] &&
                 lhs->puzzle[0][1] == rhs->puzzle[0][1] &&
                 lhs->puzzle[0][2] == rhs->puzzle[0][2] &&
                 lhs->puzzle[1][0] == rhs->puzzle[1][0] &&
                 lhs->puzzle[1][1] == rhs->puzzle[1][1] &&
                 lhs->puzzle[1][2] == rhs->puzzle[1][2] &&
                 lhs->puzzle[2][0] == rhs->puzzle[2][0] &&
                 lhs->puzzle[2][1] == rhs->puzzle[2][1] &&
                 lhs->puzzle[2][2] == rhs->puzzle[2][2]    );
    }
};

// Taken from 
// https://stackoverflow.com/questions/6339970/c-using-function-as-parameter
typedef int (*FunctionName)(int args[][SIZE]); 

char DisplayPuzzleTypeOptions();
char DisplayAlgorithmOptions();
void PopulateUserPuzzle(int[][SIZE]);
void PrintPuzzle(int[][SIZE]);
void PrintNode(Node*);
void PrintGoal(Node*);
void Expand(Node*, FunctionName f);
int MissPlacedTile(int[][SIZE]);
int ManhattanDistance(int[][SIZE]);
int NoHeuristic(int[][SIZE]);
Node* GeneralSearch(int[][SIZE], FunctionName f);

priority_queue<Node*, vector<Node*>, Least> pq; 
unordered_map<Node*, bool, KeyHasher, Equal> visited; 
int nodes_dequeued = 0;
int max_queue = 0;

int main() {
    Node* n;
    int p[SIZE][SIZE];
    char algorithmType;
    char puzzleType;

    // Default Puzzle
    p[0][0] = 1;
    p[0][1] = 2;
    p[0][2] = 0;
    p[1][0] = 4;
    p[1][1] = 5;
    p[1][2] = 3;
    p[2][0] = 7;
    p[2][1] = 8;
    p[2][2] = 6;

    // Menu and Display Options
    puzzleType = DisplayPuzzleTypeOptions();
    switch (puzzleType) {
        case '1':     
            cin.ignore();
            algorithmType = DisplayAlgorithmOptions();
            break;
        case '2':
            cin.ignore();
            PopulateUserPuzzle(p);
            algorithmType = DisplayAlgorithmOptions();
            break;
        default: 
            cout << "Error in Puzzle Choice: Exiting Program" << endl;
    }

    switch(algorithmType) {
        case '1':
            cin.ignore();
            n = GeneralSearch(p, (FunctionName)NoHeuristic); 
            break;
        case '2':
            cin.ignore();
            n = GeneralSearch(p, (FunctionName)MissPlacedTile); 
            break;
        case '3':
            cin.ignore();
            n = GeneralSearch(p, (FunctionName)ManhattanDistance); 
            break;
        default:
            cout << "Error in Algorithm Choice: Exiting Program" << endl;
    }

    PrintGoal(n);
   
    delete n;
    return 0;
}

Node* GeneralSearch(int p[][SIZE], FunctionName f) {

    // Making an initial node with: 
    // - Uniform cost set to 0.
    // - Heuristic set to an integer returned by the function pointer;
    //     - Function pointer can be ManhattanDistance(),
    //       MissPlacedTile(), or NoHeuristic().
    // - The initial puzzle (2D Array) from main.
    // - It's parent attribute set to NULL to signify root of tree.
    Node* initial = new Node(0, f(p), p, NULL);
    pq.push(initial);
    pq.push(initial);
    
    while (!pq.empty()) {
        // Checking maximum # of Nodes in the queue at anyone time.
        if (pq.size() > max_queue) { max_queue = pq.size(); }

        pq.pop();
        if ( visited[pq.top()] ) { 
            // DO NOTHING, WE'VE SEEN THIS NODE'S PUZZLE STATE
        }
        else {

            cout << "Best state to expand with g(n) = " << pq.top()->g;
            cout << " and h(n) = " << pq.top()->h << " is..." << endl;
            PrintNode(pq.top());
            cout << "Expanding this node..." << endl << endl;
            
            if (pq.top()->CheckGoal()) { return pq.top(); }
            Expand(pq.top(), f);        // Expanding if not goal state.
            ++nodes_dequeued;           // Incrementing Nodes Dequeued.
            visited[pq.top()] = true;   // Hashing the already expanded Node*.
        }
    }
    return NULL;
}

// This function checks if the current Node is at the goal state. If not,
// it finds the position of the blank. Then it creates new Node's for each
// puzzle move and updates the attributes accordingly before pushing
// into the global priority queue. 'f' is a function pointer which 
// holds the heuristic function. 
void Expand(Node* n, FunctionName f) {
    int xpos, ypos;

    // Get x-axis and y-axis positions of blank tile
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (n->puzzle[i][j] == 0) {
                xpos = i;
                ypos = j;
            }
        }
    }

    // Moving Tile UP
    if (xpos != 0) {
        Node* newNode = new Node(n);
        int temp = newNode->puzzle[xpos-1][ypos];
        newNode->puzzle[xpos][ypos] = temp;
        newNode->puzzle[xpos-1][ypos] = 0;
        newNode->g++;
        newNode->h = f(newNode->puzzle);
        pq.push(newNode);
    }      

    // Moving Tile DOWN
    if (xpos != SIZE-1) {
        Node* newNode = new Node(n);
        int temp = newNode->puzzle[xpos+1][ypos];
        newNode->puzzle[xpos][ypos] = temp;
        newNode->puzzle[xpos+1][ypos] = 0;
        newNode->g++;
        newNode->h = f(newNode->puzzle);
        pq.push(newNode);
    }
    
    
    // Moving Tile LEFT
    if (ypos != 0) {
        Node *newNode = new Node(n);
        int temp = newNode->puzzle[xpos][ypos-1];
        newNode->puzzle[xpos][ypos] = temp;
        newNode->puzzle[xpos][ypos-1] = 0;
        newNode->g++;
        newNode->h = f(newNode->puzzle);
        pq.push(newNode);
    }

    
    // Moving Tile RIGHT
    if (ypos != SIZE-1) {
        Node *newNode = new Node(n);
        int temp = newNode->puzzle[xpos][ypos+1];
        newNode->puzzle[xpos][ypos] = temp;
        newNode->puzzle[xpos][ypos+1] = 0;
        newNode->g++;
        newNode->h = f(newNode->puzzle);
        pq.push(newNode);
    }
}

char DisplayPuzzleTypeOptions() {
    char puzzleType;

    do {
        cout << "Welcome to Luis Lopez's 8-Puzzle Solver." << endl;
        cout << "Type '1' to use a default puzzle, or '2' to " <<
                " enter your own puzzle." << endl << endl;

        cin >> puzzleType;
        if (puzzleType != '1' && puzzleType != '2') {
            cout << "\nError: Wrong Input" << endl << endl;
        }

    } while (puzzleType != '1' && puzzleType != '2');
    
    return puzzleType;
}

char DisplayAlgorithmOptions() {
    char algorithmType;

    do {
        cout << "\tEnter your choice of algorithm" << endl;
        cout << "\t   1. Uniform Cost Search" << endl;
        cout << "\t   2. A* with the Misplaced Tile Heuristic" << endl;
        cout << "\t   3. A* with the Manhattan Distance Heuristic" << endl;
        cout << endl << "\t   ";

        cin >> algorithmType;
        if ( algorithmType != '1' && 
             algorithmType != '2' && 
             algorithmType != '3') {

                 cout << "\nError: Wrong Input" << endl << endl;
             }

    } while ( algorithmType != '1' && 
              algorithmType != '2' && 
              algorithmType != '3');
    
    return algorithmType;
}

void PopulateUserPuzzle(int p[][SIZE]) {
    string row0;
    string row1;
    string row2;

    cout << "\tEnter your puzzle, use a zero to represent the blank" << endl;
    cout << "\tEnter the first row, use space or tabs between numbers\t";
    getline(cin, row0);
    cout << "\tEnter the second row, use space or tabs between numbers\t";
    getline(cin, row1);
    cout << "\tEnter the third row, use space or tabs between numbers\t";
    getline(cin, row2);

    cout << row0 << " " << row1 << " " << row2;

    // Got the idea to subtract the character '0' from:
    // https://stackoverflow.com/questions/439573/
    //     how-to-convert-a-single-char-into-an-int
    p[0][0] = row0[0] - '0';
    p[0][1] = row0[2] - '0';
    p[0][2] = row0[4] - '0';
    p[1][0] = row1[0] - '0';
    p[1][1] = row1[2] - '0';
    p[1][2] = row1[4] - '0';
    p[2][0] = row2[0] - '0';
    p[2][1] = row2[2] - '0';
    p[2][2] = row2[4] - '0';

    cout << endl<< endl;
}

void PrintPuzzle(int p[][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (p[i][j] != 0) {
                cout << p[i][j] << " ";
            }
            else {
                cout << "^ ";
            }
        }
        cout << endl;
    }
}

void PrintNode(Node* n) {
    PrintPuzzle(n->puzzle);
}

void PrintGoal(Node* n) {
    if (n != NULL) {
        cout << "Goal!!" << endl << endl;
        cout << "To solve this problem, the search expanded a total of " <<
                nodes_dequeued << " nodes." << endl;
        cout << "The maximum number of nodes in the queue at any one time " <<
                "was " << max_queue << "." << endl;
        cout << "The depth of the goal was " << n->g << "." << endl;
    }
    else {
        cout << "No solution" << endl;
        cout << "The search expanded a total of " << nodes_dequeued << 
                " nodes." << endl;
        cout << "Also, the max # of nodes in the queue at any one time " <<
                "was " << max_queue << "." << endl;
    }
}

int MissPlacedTile(int p[][SIZE]) {
    int temp = 0;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if ((p[i][j] != 0) && (p[i][j] != goal[i][j])) {
                temp++;
            }
        }
    }
    return temp;
}

int ManhattanDistance(int p[][SIZE]) {
    int temp = 0;
    int xpos = 0;
    int ypos = 0;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if ((goal[i][j] != 0) && (p[i][j] != goal[i][j])) {
                for (int k = 0; k < SIZE; k++) {
                    for (int m = 0; m < SIZE; m++) {
                        if (p[k][m] == goal[i][j]) {
                            xpos = abs(k - i);
                            ypos = abs(m - j);
                            temp += xpos + ypos;
                        }
                    }
                }
            }
        }
    }
    return temp;
}

int NoHeuristic(int p[][SIZE]) {
    return 0;
}

