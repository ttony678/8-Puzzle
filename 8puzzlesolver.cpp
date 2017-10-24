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
#include <sstream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <cmath>
using namespace std;

const int SIZE = 3;

const int goal[3][3]   = { { 1, 2, 3 }, 
                           { 4, 5, 6 }, 
                           { 7, 8, 0 } };

const int vEasy[3][3]  = { { 1, 2, 3 }, 
                           { 4, 5, 6 }, 
                           { 7, 0, 8 } };

const int easy[3][3]   = { { 1, 2, 0 }, 
                           { 4, 5, 3 }, 
                           { 7, 8, 6 } };

const int doable[3][3] = { { 0, 1, 2 }, 
                           { 4, 5, 3 }, 
                           { 7, 8, 6 } };


struct Node {
    int g;                  // Holds g(n) for uniform function
    int h;                  // Holds h(n) for heuristic function
    int puzzle[SIZE][SIZE];           // Points to puzzle 
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
    Node(Node* newNode) : g(newNode->g), h(newNode->h), parent(newNode->parent){
  
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

    bool operator==(const Node& i) const {
        return ( puzzle[0][0] == i.puzzle[0][0] &&
                 puzzle[0][1] == i.puzzle[0][1] &&
                 puzzle[0][2] == i.puzzle[0][2] &&
                 puzzle[1][0] == i.puzzle[1][0] &&
                 puzzle[1][1] == i.puzzle[1][1] &&
                 puzzle[1][2] == i.puzzle[1][2] &&
                 puzzle[2][0] == i.puzzle[2][0] &&
                 puzzle[2][1] == i.puzzle[2][1] &&
                 puzzle[2][2] == i.puzzle[2][2]    );
    }

    ~Node() {
        delete parent;
    }
};

// Comparison function to set priority queue ordering.
struct Least {
    bool operator()(Node *lhs, Node *rhs) {
        return (lhs->g + lhs->h) > (rhs->g + rhs->h);
    }
};

struct KeyHasher {
    std::size_t operator() (const Node& n) const  {
        string temp;
        temp += to_string(n.g);
        temp += to_string(n.h);
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                string s = to_string(n.puzzle[i][j]);
                temp += s;
            }
        }
        return hash<string>{}(temp);
    }
};

// Taken from 
// https://stackoverflow.com/questions/6339970/c-using-function-as-parameter
typedef int (* vFunctionCall)(int args[][SIZE]); 

char DisplayPuzzleTypeOptions();
void PrintPuzzle(int[][SIZE]);
void PrintNode(Node*);
void Expand(Node*, vFunctionCall f);
int MissPlacedTile(int[][SIZE]);
int ManhattanDistance(int[][SIZE]);
int NoHeuristic(int[][SIZE]);
Node* GeneralSearch(int[][SIZE], vFunctionCall f);

priority_queue<Node*, vector<Node*>, Least> pq;  
int nodes_dequeued = 0;

int main() {
    int p[SIZE][SIZE];

    p[0][0] = 1;
    p[0][1] = 2;
    p[0][2] = 3;
    p[1][0] = 4;
    p[1][1] = 5;
    p[1][2] = 6;
    p[2][0] = 7;
    p[2][1] = 8;
    p[2][2] = 0;

    int t[SIZE][SIZE];

    t[0][0] = 1;
    t[0][1] = 2;
    t[0][2] = 3;
    t[1][0] = 4;
    t[1][1] = 5;
    t[1][2] = 6;
    t[2][0] = 7;
    t[2][1] = 8;
    t[2][2] = 0;


    Node* a = new Node(0, 0, p, NULL);
    Node* b = new Node(0, 0, t, NULL); 

    cout << "Hash of a: " << KeyHasher{}(a) << endl;
    cout << "Hash of b: " << KeyHasher{}(b) << endl;

    unordered_map<Node, bool, KeyHasher> mp;
    mp[*a] = true;

    cout << "Hash table lookup of a: " << mp[*a] << endl;
    cout << "Hash table lookup of b: " << mp[*b] << endl;



    Node* n = GeneralSearch(p, (vFunctionCall)MissPlacedTile); 

    if (n != NULL) {
        cout << "Goal!!" << endl << endl;
        cout << "To solve this problem, the search expanded a total of " <<
                (nodes_dequeued - 1) << " nodes." << endl;
        cout << "The depth of the goal was " << n->g << "." << endl;
    }
    else {
        cout << "No solution" << endl;
    }

    // char puzzleType = DisplayPuzzleTypeOptions();

   
    delete n;
    return 0;
}


Node* GeneralSearch(int p[][SIZE], vFunctionCall f) {

    // Making a initial node with: 
    // - Uniform cost set to 0.
    // - Heuristic set to an integer returned by the function pointer;
    //     - Function pointer can be ManhattanDistance(),
    //       MissPlacedTile(), or NoHeuristic().
    // - A copy of the initial puzzle from main.
    // - It's parent attribute set to NULL to signify root of tree.
    Node* initial = new Node(0, f(p), p, NULL);
    pq.push(initial);
    pq.push(initial);
    
    while (!pq.empty()) {
        if (pq.empty()) { return NULL; }
        pq.pop();
        ++nodes_dequeued;
        PrintNode(pq.top());
        if (pq.top()->CheckGoal()) { return pq.top(); }
        Expand(pq.top(), f);
    }
}

// This function checks if the current Node is at the goal state. If not,
// it finds the position of the blank. Then it creates new Node's for each
// puzzle move and updates the attributes accordingly before pushing
// into the global priority queue. 'f' is a function pointer which 
// holds the heuristic function. 
void Expand(Node* n, vFunctionCall f) {
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
        cout << "Type '1' to use a default puzzle, or \"2\" to " <<
                " enter your own puzzle." << endl;

        cin >> puzzleType;
        if (puzzleType != '1' && puzzleType != '2') {
            cout << "\nError: Wrong Input" << endl << endl;
        }

    } while (puzzleType != '1' && puzzleType != '2');
    
    return puzzleType;
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
    cout << endl;
}

void PrintNode(Node* n) {
    cout << "\tUniform cost: " << n->g << endl;
    cout << "\tHeuristic Value: " << n->h << endl;
    PrintPuzzle(n->puzzle);
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

