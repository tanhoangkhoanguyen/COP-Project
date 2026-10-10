#include "node.h"

// Simple constructor to stores raw information from file
Node::Node(int l, int p, string e, string c) {
    level = l;
    position = p;
    edgeLabel = e;
    content = c;
    parent = nullptr;
}