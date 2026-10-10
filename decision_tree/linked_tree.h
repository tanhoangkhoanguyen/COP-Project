#ifndef LINKED_TREE_H
#define LINKED_TREE_H

#include "node.h"
#include <unordered_map>
#include <fstream>

/*
Tree functions:
- Building the tree from file
- Storing node references for fast lookup
- Performing tree analysis
- Supporting interactive exploration
*/
class LinkedTree {
public:
    Node* root;

    // Quick access: position -> node pointer
    unordered_map<int, Node*> positionMap;

    LinkedTree();

    // Build tree from input file (core requirement)
    void buildTree(string filename);

    // Print tree in required visual format
    void printTree(Node* node, ofstream& out);

    // Tree property calculations
    int countInternal(Node* node);
    int countLeaves(Node* node);
    int height(Node* node);

    // Preorder-based outputs
    void internalPreorder(Node* node, ofstream& out);
    void leafPreorder(Node* node, ofstream& out);

    // Tree type checks
    bool isBinary(Node* node);
    bool isProper(Node* node);
    bool isBalanced(Node* node);

    // Interactive user exploration
    void explore();
};

#endif