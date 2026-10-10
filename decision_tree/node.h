#ifndef NODE_H
#define NODE_H
#include <string>
#include <vector>
using namespace std;

/*
Represents one question/decision in the tree. Each node stores:
- question/decision
- the position in file
- its parent and children
*/
class Node {
public:
    int level;              // depth in the tree
    int position;           // unique position ID
    string edgeLabel;       // label from parent
    string content;         // question/decision

    Node* parent;           // pointer to parent node
    vector<Node*> children; // list of child nodes

    Node(int l, int p, string e, string c);
};

#endif