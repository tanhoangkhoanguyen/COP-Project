#include "linked_tree.h"
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

LinkedTree::LinkedTree() {
    root = nullptr;
}

// Code for tree building
// Here, we use a stack to find parent instantly
void LinkedTree::buildTree(string filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Error: cannot open file.\n";
        return;
    }

    vector<Node*> levelStack(100, nullptr);
    int level, position;
    string edgeLabel;
    string content;

    while (file >> level >> position >> edgeLabel) {
        // read full sentence content (may contain spaces)
        getline(file, content);
        if (!content.empty() && content[0] == ' ')
            content.erase(0, 1);

        Node* node = new Node(level, position, edgeLabel, content);
        positionMap[position] = node; // store for quick lookup

        if (level == 0) { // root case
            root = node;
            levelStack[0] = node;
            continue;
        }

        Node* parent = levelStack[level - 1];
        node->parent = parent;
        parent->children.push_back(node);

        levelStack[level] = node; // update stack
    }

    file.close();
}

// Print the tree in the required format
void LinkedTree::printTree(Node* node, ofstream& out) {
    if (!node) return;

    for (int i = 0; i < node->level * 2; i++)
        out << "-";

    if (node->parent != nullptr)
        out << "[" << node->edgeLabel << "] ";

    out << node->content << "\n";

    for (Node* child : node->children)
        printTree(child, out);
}

// Count the internal nodes in the subtree rooted at node
int LinkedTree::countInternal(Node* node) {
    if (!node) return 0;

    int count = (!node->children.empty()) ? 1 : 0;
    for (Node* child : node->children)
        count += countInternal(child);

    return count;
}

// Count the leaf nodes in the subtree rooted at node
int LinkedTree::countLeaves(Node* node) {
    if (!node) return 0;
    if (node->children.empty())
        return 1;

    int count = 0;
    for (Node* child : node->children)
        count += countLeaves(child);

    return count;
}

// Return the height of the subtree rooted at node
int LinkedTree::height(Node* node) {
    if (!node) return -1;

    int maxH = -1;
    for (Node* child : node->children)
        maxH = max(maxH, height(child));

    return maxH + 1;
}

// Print internal nodes in preorder traversal
void LinkedTree::internalPreorder(Node* node, ofstream& out) {
    if (!node) return;
    if (!node->children.empty())
        out << node->content << "\n";

    for (Node* child : node->children)
        internalPreorder(child, out);
}

// Print leaf nodes in preorder traversal
void LinkedTree::leafPreorder(Node* node, ofstream& out) {
    if (!node) return;
    if (node->children.empty())
        out << node->content << "\n";

    for (Node* child : node->children)
        leafPreorder(child, out);
}

// Check if the tree rooted at node is a binary tree
bool LinkedTree::isBinary(Node* node) {
    if (!node) return true;
    if (node->children.size() > 2)
        return false;

    for (Node* child : node->children)
        if (!isBinary(child))
            return false;

    return true;
}

// Check if the tree rooted at node is a proper binary tree
bool LinkedTree::isProper(Node* node) {
    if (!node) return true;
    if (!node->children.empty() && node->children.size() != 2)
        return false;

    for (Node* child : node->children)
        if (!isProper(child))
            return false;

    return true;
}

// Check if the tree rooted at node is balanced
bool LinkedTree::isBalanced(Node* node) {
    if (!node) return true;
    if (node->children.size() <= 1)
        return true;

    int h1 = height(node->children[0]);
    int h2 = height(node->children[1]);

    if (abs(h1 - h2) > 1)
        return false;

    for (Node* child : node->children)
        if (!isBalanced(child))
            return false;

    return true;
}

// Interactive exploration function
void LinkedTree::explore() {
    string input;
    while (true) {
        cout << "\nEnter position or 'exit': ";
        cin >> input;

        if (input == "exit") {
            cout << "Goodbye!\n";
            break;
        }

        int pos;
        try {
            pos = stoi(input);
        }
        catch (...) {
            cout << "Invalid input. Please try again.\n";
            continue;
        }

        if (positionMap.find(pos) == positionMap.end()) {
            cout << "Invalid input. Please try again.\n";
            continue;
        }

        Node* node = positionMap[pos];
        cout << "Node's content: " << node->content << "\n";
        cout << "Ancestor: "
             << (node->parent ? node->parent->content : "None") << "\n";
        cout << "Descendant: "
             << (!node->children.empty() ? node->children[0]->content : "None") << "\n";

        if (node->parent)
            for (Node* sib : node->parent->children)
                if (sib != node) {
                    cout << "Sibling: " << sib->content << "\n";
                    break;
                }
        else cout << "Sibling: None\n";
    }
}