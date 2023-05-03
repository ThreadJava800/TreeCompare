#include "avl.cpp"

int main() {
    // srand((unsigned) time(NULL));

    AVL_t *tree = treeCtor();

    insertSM(tree, 15);
    insertSM(tree, 189);
    insertSM(tree, 12);
    insertSM(tree, -100);
    insertSM(tree, 89);
    insertSM(tree, 56);

    graphDump(tree->root);

    return 0;
}