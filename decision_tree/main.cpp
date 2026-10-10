#include <iostream>
#include <fstream>
#include "linked_tree.h"
using namespace std;

int main() {
    LinkedTree tree;
    string filename;
    cout << "Enter file name: ";
    cin >> filename;

    tree.buildTree(filename);

    ofstream out("about_tree.txt");

    out << "TREE VISUALIZATION\n";
    tree.printTree(tree.root, out);

    out << "\nRoot: " << tree.root->content << "\n";
    out << "Internal Nodes: " << tree.countInternal(tree.root) << "\n";
    out << "External Nodes: " << tree.countLeaves(tree.root) << "\n";
    out << "Height: " << tree.height(tree.root) << "\n";

    out << "\nInternal Preorder:\n";
    tree.internalPreorder(tree.root, out);

    out << "\nExternal Preorder:\n";
    tree.leafPreorder(tree.root, out);

    bool binary = tree.isBinary(tree.root);
    out << "\nBinary Tree: " << (binary ? "Yes" : "No") << "\n";

    if (binary) {
        out << "Proper Tree: " << (tree.isProper(tree.root) ? "Yes" : "No") << "\n";
        out << "Balanced Tree: " << (tree.isBalanced(tree.root) ? "Yes" : "No") << "\n";
    }

    out.close();
    tree.explore();

    return 0;
}