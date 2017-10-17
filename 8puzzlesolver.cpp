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
#include <queue>
#include <cstdlib>
#include <cmath>
using namespace std;


/*
 * Global Variables
 */
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
    int g;                  // Holds g(n) for uniform fxn
    int h;                  // Holds h(n) for heuristic fxn
    int** puzzle;           // Points to puzzle 
    Node* parent;           // Points to parent for backtrace


    Node() {
        g = 0;
        h = 0;
        parent = NULL;

        puzzle = new int*[3];
        for (int i = 0; i < 3; i++) {
            puzzle[i] = new int[3];
        }
    }


    Node(int g, int h, int** newPuzzle, Node* p) 
            : g(g), h(h), parent(p) {
        
        puzzle = new int*[3];
        for (int i = 0; i < 3; i++) {
            puzzle[i] = new int[3];
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                puzzle[i][j] = newPuzzle[i][j];
            }
        }
    }


    Node(Node* newNode) : g(newNode->g), h(newNode->h), parent(newNode->parent){
        
        puzzle = new int*[3];
        for (int i = 0; i < 3; i++) {
            puzzle[i] = new int[3];
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                puzzle[i][j] = newNode->puzzle[i][j];
            }
        }
    }


    bool CheckGoal() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j< 3; j++) {
                if (puzzle[i][j] != goal[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }


    void Expand() {
        int xpos, ypos;

        if ( CheckGoal() ) {
            cout << "Goal State Reached After Expanding" << endl;
            exit(0);
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (puzzle[i][j] == 0) {
                    xpos = i;
                    ypos = j;
                }
            }
        }
        

        /*
        // Moving Tile UP
        if (xpos != 0) {
            int temp = puzzle[xpos-1][ypos];
            puzzle[xpos][ypos] = temp;
            puzzle[xpos-1][ypos] = 0;
        }
        */

        
        // Moving Tile DOWN
        if (xpos != 2) {
            int temp = puzzle[xpos+1][ypos];
            puzzle[xpos][ypos] = temp;
            puzzle[xpos+1][ypos] = 0;
        }
        

        /*
        // Moving Tile LEFT
        if (ypos != 0) {
            int temp = puzzle[xpos][ypos-1];
            puzzle[xpos][ypos] = temp;
            puzzle[xpos][ypos-1] = 0;
        }
        */

        /*
        // Moving Tile RIGHT
        if (ypos != 2) {
            int temp = puzzle[xpos][ypos+1];
            puzzle[xpos][ypos] = temp;
            puzzle[xpos][ypos+1] = 0;
        }
        */
    }


    ~Node() {
        for(int i = 0; i < 3; ++i) {
            delete [] puzzle[i];
        }
    }
};


/*
 * PROTOTYPES
 */
char DisplayPuzzleTypeOptions();
void PrintPuzzle(int**);
int MissPlacedTile(const Node*);
int ManhattanDistance(const Node*);


queue<Node*> tree;         // Holds the min of f(n) = g(n) + h(n)


/*
 * MAIN FUNCTION
 */
int main() {
    Node *init;
    init = new Node;

    init->puzzle[0][0] = 0;
    init->puzzle[0][1] = 1;
    init->puzzle[0][2] = 2;
    init->puzzle[1][0] = 4;
    init->puzzle[1][1] = 5;
    init->puzzle[1][2] = 8;
    init->puzzle[2][0] = 7;
    init->puzzle[2][1] = 3;
    init->puzzle[2][2] = 6;
    PrintPuzzle(init->puzzle);

    cout << "Manhattan Distance: ";
    int tiles = ManhattanDistance(init);
    cout << tiles << endl << endl;

    // char puzzleType = DisplayPuzzleTypeOptions();

    delete init;
    return 0;
}


/*
 * HELPER FUNCTIONS
 */
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
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (p[i][j] != 0) {
                cout << p[i][j] << " ";
            }
            else {
                cout << "B ";
            }
        }
        cout << endl;
    }
    cout << endl;
}


int MissPlacedTile(const Node* n) {
    int temp = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if ((n->puzzle[i][j] != 0) && (n->puzzle[i][j] != goal[i][j])) {
                temp++;
            }
        }
    }

    return temp;
}


int ManhattanDistance(const Node* n) {
    int temp = 0;
    int xpos = 0;
    int ypos = 0;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if ((goal[i][j] != 0) && (n->puzzle[i][j] != goal[i][j])) {
                for (int k = 0; k < 3; k++) {
                    for (int m = 0; m < 3; m++) {
                        if (n->puzzle[k][m] == goal[i][j]) {
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

