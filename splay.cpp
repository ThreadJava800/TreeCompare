#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include <chrono>

struct Node_t {
    int key;

    struct Node_t *left;
    struct Node_t *right;
    struct Node_t *parent;
};

struct Splay_t {
    struct Node_t *root;
};

//=========SPLAY STAFF=========
struct Splay_t *treeCtor();
struct Node_t  *nodeCtor(int key, struct Node_t *left, struct Node_t *right, struct Node_t *parent);
void nodeDtor(struct Node_t  *root);
void treeDtor(struct Splay_t *tree);

void _rotateLeft (struct Node_t *node);
void _rotateRight(struct Node_t *node);
Node_t *_merge(struct Node_t *left, struct Node_t *right);
void _splay      (struct Node_t *node);

struct Node_t *_add(struct Node_t  *node, struct Node_t *prev, int side,  int key);
void            add(struct Splay_t *splay, int key);

struct Node_t *_maxNode(struct Node_t *node);
struct Node_t *_find(struct Node_t *root, int key);
void _remove(struct Node_t  **root, int key);
void            remove(struct Splay_t *splay, int key);

void graphNode(struct Node_t *node, FILE *tempFile);
void drawConnections(struct Node_t *node, FILE *tempFile);
void graphDump(struct Node_t *node, int id);

//=========CONTEST STAFF=============
#define INPUT_CHECK(readFunc, res) {        \
    int factRes = readFunc;                  \
    if (factRes != res) abort();              \
}                                              \


#define ON_ERROR(expr, errStr, retVal) {                 \
    if (expr) {                                           \
        fprintf(stderr, "FATAL ERROR: %s\n", errStr);      \
        return retVal;                                      \
    }                                                        \
}                                                             \
//===========================================

struct Splay_t *treeCtor() {
    struct Splay_t *tree = (struct Splay_t*) calloc(1, sizeof(struct Splay_t));
    ON_ERROR(!tree, "Unable to alloc mem", NULL);

    tree->root = NULL;

    return tree;
}

struct Node_t *nodeCtor(int key, struct Node_t *left, struct Node_t *right, struct Node_t *parent) {
    struct Node_t *leaf = (struct Node_t*) calloc(1, sizeof(struct Node_t));
    if(!leaf) return NULL;

    leaf->key     = key;

    leaf->left    = left;
    leaf->right   = right;
    leaf->parent  = parent;

    return leaf;
}

void nodeDtor(struct Node_t *root) {
    if(!root) return;

    if (root->left)  nodeDtor(root->left);
    if (root->right) nodeDtor(root->right);

    free(root);
}

void treeDtor(struct Splay_t *tree) {
    ON_ERROR(!tree, "Nullptr", );

    nodeDtor(tree->root);

    free(tree);
}

#define P(node) (node->parent)
// grand parent
#define GP(node) (P(P(node)))

void _rotateLeft(struct Node_t *node) {
    // one of sons of node is x

    struct Node_t *grandParent = P(node);     // grandparent of x
    struct Node_t *x = node->right; // x

    // update child of grandparent if he exists
    if (grandParent) {
        if (grandParent->left == node) {
            grandParent->left  = x;
        } else {
            grandParent->right = x;
        }
    }

    // setting parent as left child of x and then parent right = x left
    struct Node_t *xLeft = x->left;
    x->left              = node;
    node->right          = xLeft;

    P(node) = x;
    P(x)    = grandParent;

    if (node->right) {
        P(node->right) = node;
    }
}

void _rotateRight(struct Node_t *node) {
    // one of sons of node is x

    struct Node_t *grandParent = P(node);     // grandparent of x
    struct Node_t *x = node->left; // x

    // update child of grandparent if he exists
    if (grandParent) {
        if (grandParent->left == node) {
            grandParent->left  = x;
        } else {
            grandParent->right = x;
        }
    }

    // setting parent as right child of x and then parent left = x right
    struct Node_t *xRight = x->right;
    x->right              = node;
    node->left            = xRight;

    P(node) = x;
    P(x)    = grandParent;

    if (node->left) {
        P(node->left) = node;
    }
}

struct Node_t *_maxNode(struct Node_t *node) {
    ON_ERROR(!node, "Nullptr", nullptr);

    if (!node->right) return node;
    return _maxNode(node->right);
}

Node_t *_merge(struct Node_t *left, struct Node_t *right) {
    if (!left) return right;
    if (!right) return left;

    Node_t *maxi = _maxNode(left);
    _splay(maxi);
    maxi->right = right;

    P(right) = maxi;
    return maxi;
}

void _splay (struct Node_t *node) {
    if (!node) return;

    while (P(node))
    {
        if (node == P(node)->left) {
            if (!GP(node)) _rotateRight(P(node));
            else if (GP(node)->left == P(node)) {
                _rotateRight(GP(node));
                _rotateRight(P(node));                 // zig-zig
            } else {
                _rotateRight(P(node));
                _rotateLeft (P(node));                 // zig-zag
            }
        } else {
            if (!GP(node)) _rotateLeft(P(node));
            else if (GP(node)->left == P(node)) {
                _rotateLeft (P(node));
                _rotateRight(P(node));                 // zig-zag
            } else {
                _rotateLeft(GP(node));
                _rotateLeft(P(node));                  // zig-zig
            }
        }
    }
}

struct Node_t *_add(struct Node_t  *node, struct Node_t *prev, int side,  int key) {
    if (!node) {
        struct Node_t *newNode = nodeCtor(key, NULL, NULL, prev);

        // set left or right for parent (needed for splay)
        if (prev) {
            if (side) prev->right = newNode;
            else      prev->left  = newNode;
        }

        return newNode;
    }

    if (node->key > key) {
        // 0 = came from left
        return _add(node->left, node, 0, key);
    }

    // else
    // 1 = came from right
    return _add(node->right, node, 1, key); 
}

void add(struct Splay_t *splay, int key) {
    ON_ERROR(!splay, "Nullptr", );

    struct Node_t *addedNode = _add(splay->root, NULL, 0, key);

    _splay(addedNode);

    splay->root = addedNode;
}

struct Node_t *_find(struct Node_t *root, int key) {
    ON_ERROR(!root, "Nullptr", nullptr);

    if (root->key == key) return root;

    if (root->key > key) return _find(root->left, key);
    return _find(root->right, key);
}

void _remove(struct Node_t  **root, int key) {
    ON_ERROR(!root, "Nullptr", );

    struct Node_t *needed = _find(*root, key);
    _splay(needed);

    if (!needed) return;

    if (needed->left)  needed->left->parent  = nullptr;
    if (needed->right) needed->right->parent = nullptr;

    Node_t *tree = _merge(needed->left, needed->right);
    *root = tree;

    if (tree) tree->parent = nullptr;
}

void remove(struct Splay_t *splay, int key) {
    ON_ERROR(!splay, "Nullptr",);

    _remove(&(splay->root), key);
}

void graphNode(struct Node_t *node, FILE *tempFile) {
    fprintf(
                tempFile, 
                "\tlabel%p[shape=record, style=\"rounded, filled\", fillcolor=red, label=\"{ {val: %d ",
                node,
                node->key
            );  

    fprintf(
                tempFile, 
                " | {left: %p | right: %p | parent: %p | cur: %p} }}\"];\n", 
                node->left,
                node->right,
                node->parent,
                node
            );

    if (node->parent) {
        fprintf(tempFile, "\tlabel%p->label%p [color=\"blue\", style=\"dashed\",arrowhead=\"none\"]", node->parent, node);
    }

    if (node->left)  graphNode(node->left, tempFile);
    if (node->right) graphNode(node->right, tempFile);
}

void drawConnections(struct Node_t *node, FILE *tempFile) {
    if (!node) return;

    if (node->left) {
        fprintf(tempFile, "\tlabel%p->label%p [color=\"green\", style=\"dashed\",arrowhead=\"none\"]", node, node->left);
    }
    if (node->right) {
        fprintf(tempFile, "\tlabel%p->label%p [color=\"red\", style=\"dashed\",arrowhead=\"none\"]", node, node->right);
    }

    if (node->left)  drawConnections(node->left, tempFile);
    if (node->right) drawConnections(node->right, tempFile);
}

void graphDump(struct Node_t *node, int id) {
    if (!node) return;

    FILE *tempFile = fopen("temp.dot", "w");
    fprintf(tempFile, "digraph tree {\n");
    fprintf(tempFile, "\trankdir=HR;\n");
    if (!tempFile) return;

    graphNode(node, tempFile);
    drawConnections(node, tempFile);

    fprintf(tempFile, "}");
    fclose(tempFile);

    char command[2096] = "";
    sprintf(command, "dot -Tsvg temp.dot > graph_%d.png && xdg-open graph_%d.png", id, id);
    system(command);
}

int main() {
    srand((unsigned) time(NULL));

    for (int i = 100000; i <= 1000000; i+= 100000) {
        printf("IN %d\n", i);

        Splay_t *tree = treeCtor();
        int measureData[1000000] = {};
 
        for (int j = 0; j < i; j++) {
            measureData[j] = rand();
        }

        double allTime = 0;

        auto start = std::chrono::high_resolution_clock::now(); 
        for (int j = 0; j < 5; j++) {
            for (int m = 0; m < i; m++) {
                add(tree, measureData[m]);
            }
        }
        auto end   = std::chrono::high_resolution_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        allTime += duration.count();
        printf("ADD %lf\n", allTime / 5);

        allTime = 0;
        start = std::chrono::high_resolution_clock::now(); 
        for (int j = 0; j < 5; j++) {
            for (int m = 0; m < i; m++) {
                remove(tree, measureData[m]);
            }
        }
        end   = std::chrono::high_resolution_clock::now(); 
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        allTime += duration.count();
        printf("DEL %lf\n", allTime / 5);
    }

    return 0;
}