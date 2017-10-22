/*
 * Luis Lopez
 * SID 861-236-618
 * 8-Puzzle Project
 *
 * CS 170: Intro to Artificial Intelligence
 * Instructor: Eamonn Keogh
 * DUE: November 3rd, 2017
 */

#include <iostream>
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
    int** puzzle;           // Points to puzzle 
    Node* parent;           // Points to parent for backtrace

    Node() {
        g = 0;
        h = 0;
        parent = NULL;

        puzzle = new int*[SIZE];
        for (int i = 0; i < SIZE; i++) {
            puzzle[i] = new int[SIZE];
        }
    }

    // Used to create the first Node.
    Node(int g, int h, int** newPuzzle, Node* p) 
            : g(g), h(h), parent(p) {
        
        puzzle = new int*[SIZE];
        for (int i = 0; i < SIZE; i++) {
            puzzle[i] = new int[SIZE];
        }

        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                puzzle[i][j] = newPuzzle[i][j];
            }
        }
    }

    // Used to create more Nodes easily
    Node(Node* newNode) : g(newNode->g), h(newNode->h), parent(newNode->parent){
        
        puzzle = new int*[SIZE];
        for (int i = 0; i < SIZE; i++) {
            puzzle[i] = new int[SIZE];
        }

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

    ~Node() {
        for(int i = 0; i < SIZE; ++i) {
            delete [] puzzle[i];
        }
        delete [] puzzle;
        delete parent;
    }
};

struct CompareAge {
    bool operator()(Node *lhs, Node *rhs) {
        return (lhs->g + lhs->h) > (rhs->g + rhs->h);
    }
};


// Taken from 
// https://stackoverflow.com/questions/6339970/c-using-function-as-parameter
typedef int (* vFunctionCall)(int** args); 

char DisplayPuzzleTypeOptions();
void PrintPuzzle(int**);
void PrintNode(const Node*);
void Expand(Node*, vFunctionCall f);
int MissPlacedTile(int**);
int ManhattanDistance(int**);
int NoHeuristic(int**);
Node* GeneralSearch(int**, vFunctionCall f);

// Holds the min of f(n) = g(n) + h(n)
priority_queue<Node*, vector<Node*>, CompareAge> tree;  


int main() {
    int** p;
    p = new int*[SIZE];
    for (int i = 0; i < SIZE; i++) {
        p[i] = new int[SIZE];
    }

    p[0][0] = 1;
    p[0][1] = 2;
    p[0][2] = 3;
    p[1][0] = 4;
    p[1][1] = 5;
    p[1][2] = 6;
    p[2][0] = 8;
    p[2][1] = 7;
    p[2][2] = 0;

    Node* n = GeneralSearch(p, (vFunctionCall)MissPlacedTile); 

    if (n == NULL) {
        cout << "No solution" << endl;
    }
    // char puzzleType = DisplayPuzzleTypeOptions();

    for(int i = 0; i < SIZE; ++i) {
            delete [] p[i];
        }
    delete [] p;
    delete n;
    return 0;
}

Node* GeneralSearch(int** p, vFunctionCall f) {

    // Making a initial node with: 
    // - Uniform cost set to 0.
    // - Heuristic set to an integer returned by the function pointer;
    //     - Function pointer can be ManhattanDistance(),
    //       MissPlacedTile(), or NoHeuristic().
    // - A copy of the initial puzzle from main.
    // - It's parent attribute set to NULL to signify root of tree.
    Node* initial = new Node(0, f(p), p, NULL);
    tree.push(initial);
    tree.push(initial);
    
    /*
    tree.pop();
    cout << "Tree size: " << tree.size();
    cout << "\t Tree empty(): " << tree.empty() << endl;
    cout << "Accessing empty tree: " << tree.top()->puzzle[0][0] << endl;;
    */

    while (!tree.empty()) {
        if (tree.empty()) { return NULL; }
        tree.pop();
        // PrintNode(tree.top());
        if (tree.top()->CheckGoal()) { return tree.top(); }
        Expand(tree.top(), f);
    }

    return initial;
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
        tree.push(newNode);
    }      

    // Moving Tile DOWN
    if (xpos != SIZE-1) {
        Node* newNode = new Node(n);
        int temp = newNode->puzzle[xpos+1][ypos];
        newNode->puzzle[xpos][ypos] = temp;
        newNode->puzzle[xpos+1][ypos] = 0;
        newNode->g++;
        newNode->h = f(newNode->puzzle);
        tree.push(newNode);
    }
    
    
    // Moving Tile LEFT
    if (ypos != 0) {
        Node *newNode = new Node(n);
        int temp = newNode->puzzle[xpos][ypos-1];
        newNode->puzzle[xpos][ypos] = temp;
        newNode->puzzle[xpos][ypos-1] = 0;
        newNode->g++;
        newNode->h = f(newNode->puzzle);
        tree.push(newNode);
    }

    
    // Moving Tile RIGHT
    if (ypos != SIZE-1) {
        Node *newNode = new Node(n);
        int temp = newNode->puzzle[xpos][ypos+1];
        newNode->puzzle[xpos][ypos] = temp;
        newNode->puzzle[xpos][ypos+1] = 0;
        newNode->g++;
        newNode->h = f(newNode->puzzle);
        tree.push(newNode);
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

void PrintPuzzle(int** p) {
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

void PrintNode(const Node* n) {
    cout << "\tUniform cost: " << n->g << endl;
    cout << "\tHeuristic Value: " << n->h << endl;
    PrintPuzzle(n->puzzle);
}

int MissPlacedTile(int** p) {
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

int ManhattanDistance(int** p) {
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

int NoHeuristic(int** p) {
    return 0;
}

