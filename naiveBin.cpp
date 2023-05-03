#include "naiveBin.h"
#include "measure.hpp"

TreeElem_t *nodeCtor(Elem_t value) {
    TreeElem_t *node = (TreeElem_t *) calloc(1, sizeof(TreeElem_t));
    ON_ERROR(!node, "Nullptr", nullptr);

    node->left  = nullptr;
    node->right = nullptr;
    node->value = value;

    return node;
}

NaiveBin_t *NaiveBin::ctor() {
    NaiveBin_t *tree = (NaiveBin_t *) calloc(1, sizeof(NaiveBin_t));
    ON_ERROR(!tree, "Nullptr", nullptr);

    tree->root = nullptr;

    return tree;
}

TreeElem_t *add(TreeElem_t *root, Elem_t value) {
    ON_ERROR(!root, "Nullptr", nullptr);

    if (!root) return nodeCtor(value);

    if (root->value > value) {
        root->left = add(root->left, value);
    } else {
        root->right = add(root->right, value);
    }

    return root;
}

void NaiveBin::add(NaiveBin_t *tree, Elem_t value) {
    ON_ERROR(!tree, "Nullptr", );

    tree->root = add(tree->root, value);
}

TreeElem_t *NaiveBin::search(TreeElem_t *tree, Elem_t value) {
    if (!tree) return nullptr;

    if (tree->value == value) return tree;
    if (tree->value > value)  return search(tree->left, value);
    return search(tree->right, value);
}

Elem_t getMinNode(TreeElem_t *node) {
    if (!node) return TMAX;

    if (!node->left) return node->value;

    return getMinNode(node->left);
}

TreeElem_t *deleteRecursive(TreeElem_t *node, Elem_t value) {
    if (!node) return nullptr;

    if      (value < node->value) node->left  = deleteRecursive(node->left,  value);
    else if (value > node->value) node->right = deleteRecursive(node->right, value);
    else if (node->left && node->right) {
        node->value = getMinNode(node->right);
        node->right = deleteRecursive(node->right, node->value);
    } else {
        if      (node->left)  node = node->left;
        else if (node->right) node = node->right;
        else                  node = nullptr;
    }
    return node;
}

void NaiveBin::del(NaiveBin_t *tree, Elem_t value) {
    ON_ERROR(!tree, "Nullptr", -1);

    deleteRecursive(tree->root, value);
}

void graphNode(struct TreeElem_t *node, FILE *tempFile) {
    fprintf(
                tempFile, 
                "\tlabel%p[shape=record, style=\"rounded, filled\", fillcolor=red, label=\"{ {val: %d ",
                node,
                node->value
            );  

    fprintf(
                tempFile, 
                " | {left: %p | right: %p} }}\"];\n", 
                node->left,
                node->right
            );

    if (node->left)  graphNode(node->left, tempFile);
    if (node->right) graphNode(node->right, tempFile);
}

void drawConnections(struct TreeElem_t *node, FILE *tempFile) {
    if (!node) return;

    if (node->left) {
        fprintf(tempFile, "\tlabel%p->label%p [color=\"red\", style=\"dashed\",arrowhead=\"none\"]", node, node->left);
    }
    if (node->right) {
        fprintf(tempFile, "\tlabel%p->label%p [color=\"red\", style=\"dashed\",arrowhead=\"none\"]", node, node->right);
    }

    if (node->left)  drawConnections(node->left, tempFile);
    if (node->right) drawConnections(node->right, tempFile);
}

void graphDump(struct TreeElem_t *node) {
    if (!node) return;

    FILE *tempFile = fopen("temp.dot", "w");
    fprintf(tempFile, "digraph tree {\n");
    fprintf(tempFile, "\trankdir=HR;\n");
    if (!tempFile) return;

    graphNode(node, tempFile);
    drawConnections(node, tempFile);

    fprintf(tempFile, "}");
    fclose(tempFile);

    system("dot -Tsvg temp.dot > graph.png && xdg-open graph.png");
}

void oneAddFunc(void *tree, int *measureData) {
    NaiveBin_t *workTree = (NaiveBin_t *)tree;

    for (int i = 0; i < 100000; i++) {
        NaiveBin::add(workTree, measureData[i]);
    }
}

void oneDelFunc(void *tree, int *measureData) {
    NaiveBin_t *workTree = (NaiveBin_t *)tree;

    for (int i = 0; i < 50000; i++) {
        NaiveBin::del(workTree, measureData[i]);
    }
}

void test() {
    int measureData[100000] = {};

    for (int i = 0;  i < 100000; i++) {
        measureData[i] = rand();
    }

    NaiveBin_t *tree = NaiveBin::ctor();
    printf("ADDS: %lf\n", medianTime(oneAddFunc, tree, measureData, 5));
    printf("DELETES: %lf\n", medianTime(oneDelFunc, tree, measureData, 5));

    tree = NaiveBin::ctor();
    for (int i = 0; i < 100000; i++) {
        measureData[i] = i;
    }

    printf("ADDS: %lf\n", medianTime(oneAddFunc, tree, measureData, 5));
    printf("DELETES: %lf\n", medianTime(oneDelFunc, tree, measureData, 5));
}