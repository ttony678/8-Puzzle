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
int goal[SIZE][SIZE];

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
        for (unsigned i = 0; i < SIZE; i++) {
            for (unsigned j = 0; j < SIZE; j++) {
                if (lhs->puzzle[i][j] != rhs->puzzle[i][j]) {
                    return 0;
                }
            }
        }
        return 1;
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
void BackTrace(Node*);
void createGoal();
int MissPlacedTile(int[][SIZE]);
int ManhattanDistance(int[][SIZE]);
int NoHeuristic(int[][SIZE]);
Node* GeneralSearch(int[][SIZE], FunctionName f);

priority_queue<Node*, vector<Node*>, Least> pq; 
unordered_map<Node*, bool, KeyHasher, Equal> visited; 
int nodes_dequeued = 0;
unsigned max_queue = 0;

int main() {
    Node* n;
    int p[SIZE][SIZE];
    char algorithmType;
    char puzzleType;

    // Default Puzzle 4x4
    // p[0][0] = 1;
    // p[0][1] = 2;
    // p[0][2] = 3;
    // p[0][3] = 4;
    // p[1][0] = 5;
    // p[1][1] = 6;
    // p[1][2] = 7;
    // p[1][3] = 0;
    // p[2][0] = 9;
    // p[2][1] = 10;
    // p[2][2] = 11;
    // p[2][3] = 8;
    // p[3][0] = 13;
    // p[3][1] = 14;
    // p[3][2] = 15;
    // p[3][3] = 12;

    // Default Puzzle 3x3
    p[0][0] = 8; 
    p[0][1] = 7; 
    p[0][2] = 1; 
    p[1][0] = 6; 
    p[1][1] = 0; 
    p[1][2] = 2; 
    p[2][0] = 5; 
    p[2][1] = 4; 
    p[2][2] = 3; 

    // Creating Goal State
    createGoal();

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
	
    cout << endl;
    BackTrace(n);
   
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

    if (initial->CheckGoal()) { return initial; }

    // cout << endl << "Expanding state: " << endl;
    // PrintNode(initial);
    // cout << endl;

    Expand(initial, f);
    ++nodes_dequeued;           // Incrementing Node's expanded
    visited[initial] = true;
    
    while (!pq.empty()) {
        // Checking maximum # of Nodes in the queue at anyone time.
        if (pq.size() > max_queue) { max_queue = pq.size(); }

        if ( visited[pq.top()] ) { 
            // DO NOTHING, WE'VE SEEN THIS NODE'S PUZZLE STATE
        }
        else {
                        
            if (pq.top()->CheckGoal()) { return pq.top(); }

            // cout << "Best state to expand with g(n) = " << pq.top()->g;
            // cout << " and h(n) = " << pq.top()->h << " is..." << endl;
            // PrintNode(pq.top());
            // cout << "Expanding this node..." << endl << endl;

            Expand(pq.top(), f);        // Expanding if not goal state.
            ++nodes_dequeued;           // Incrementing Node's expanded
            visited[pq.top()] = true;   // Hashing the already expanded Node*.
        }
        pq.pop();
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
    string row;
    vector<int> userInput;
    vector< vector<int> > totalUserInput;

    cout << "\tEnter your puzzle, use a zero to represent the blank" << endl;

    for (unsigned i = 0; i < SIZE; i++) {
        cout << "\tEnter the " << i+1 << " row, use space or tabs between numbers\t";
        getline(cin, row);
        stringstream ss(row);
        int x = 0;
        while (ss >> x) {
            userInput.push_back(x);
        }
        totalUserInput.push_back(userInput);
        userInput.clear();
    }
    
    for (unsigned i = 0; i < SIZE; i++) {
        for (unsigned j = 0; j < SIZE; j++) {
            p[i][j] = totalUserInput.at(i).at(j); 
        }
    }

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

void BackTrace(Node* n) {
	vector<Node*> list;
	
	if (n != NULL ) {
		
		cout << "Solved the puzzle with these moves: " << endl;
		
		while (n != NULL) {
			list.insert(list.begin(), n);
			n = n->parent;
		}
		
		PrintNode(list.at(0));		
		for (unsigned i = 1; i < list.size(); i++) {
			cout << endl << "  | " << endl;
			cout << "  V " << endl << endl;
			PrintNode(list.at(i));
		}
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

void createGoal() {
    int temp = 1;
    for (unsigned i = 0; i < SIZE; i++) {
        for (unsigned j = 0; j < SIZE; j++) {
            if (!(i == SIZE-1 && j == SIZE-1))
                goal[i][j] = temp++; 
            else 
                goal[i][j] = 0;
        }
    }
}
