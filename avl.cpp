#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <chrono>


typedef int Elem_t;

Elem_t DEFAULT_MAX = INT_MAX;

struct Node_t {
    Elem_t key;
    int height;
    
    struct Node_t* left;
    struct Node_t* right;
};

struct AVL_t {
    struct Node_t* root;
};

//==========AVL_TREE_METHODS============
struct AVL_t  *treeCtor();
struct Node_t *nodeCtor(Elem_t key, Node_t *left, Node_t *right);
void           nodeDtor(struct Node_t*root);
void           treeDtor(struct AVL_t *tree);

void _recountHeight(struct Node_t *node);
int _getHeight(struct Node_t *node);
int  _balanceFactor(struct Node_t *node);

struct Node_t *balance(struct Node_t *node);
struct Node_t *rotateLeft(struct Node_t *node);
struct Node_t *rotateRight(struct Node_t *node);


struct Node_t *_merge(struct Node_t *left, struct Node_t *right);
void   _split(Elem_t key, struct Node_t *current, struct Node_t **outLeft, struct Node_t **outRight);

struct Node_t *_insertSM(struct Node_t *node, Elem_t key, int priority);
void   insertSM(struct AVL_t *treap, Elem_t key);

void _removeSM(struct Node_t **node,  Elem_t key);
void  removeSM(struct AVL_t *treap, Elem_t key);

struct Node_t *_addKey(struct Node_t *node, Elem_t key);
void            addKey(struct AVL_t *tree, Elem_t key);

struct Node_t *_removeMin(struct Node_t *node);
struct Node_t *_removeKey(struct Node_t *node, Elem_t key);
void            removeKey(struct AVL_t *tree, Elem_t key);

void graphNode(struct Node_t *node, FILE *tempFile);
void drawConnections(struct Node_t *node, FILE *tempFile);
void graphDump(struct Node_t *node);


#define ON_ERROR(expr, errStr, retVal) {                 \
                                                          \
}                                                          \
//===========================================

struct AVL_t  *treeCtor() {
    struct AVL_t *tree = (struct AVL_t*) calloc(1, sizeof(struct AVL_t));
    ON_ERROR(!tree, "Unable to alloc mem", NULL);

    tree->root = NULL;

    return tree;
}

struct Node_t *nodeCtor(Elem_t key, Node_t *left, Node_t *right) {
    struct Node_t *leaf = (struct Node_t*) calloc(1, sizeof(struct Node_t));
    if(!leaf) return NULL;

    leaf->height = 0;
    leaf->key    = key;
    leaf->left   = left;
    leaf->right  = right;

    return leaf;
}

void nodeDtor(struct Node_t *root) {
    if(!root) return;

    if (root->left)  nodeDtor(root->left);
    if (root->right) nodeDtor(root->right);

    free(root);
}

void treeDtor(struct AVL_t *tree) {
    ON_ERROR(!tree, "Nullptr",);

    nodeDtor(tree->root);

    free(tree);
}

void _recountHeight(struct Node_t *node) {
    int lH = _getHeight(node->left);
    int rH = _getHeight(node->right);

    node->height = (lH <= rH ? 
                    (rH + 1) : (lH + 1));
}

int _getHeight(struct Node_t *node) {
    if (!node) return 0;

    return node->height;
}

int _balanceFactor(struct Node_t *node) {
    if (!node) return 0;

    return _getHeight(node->left) - _getHeight(node->right);
}

struct Node_t *balance(struct Node_t *node) {
    _recountHeight(node);

    if (_balanceFactor(node) == -2) {
        if (_balanceFactor(node->right) == 1) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    }

    if (_balanceFactor(node) == 2) {
        if (_balanceFactor(node->left) == -1) {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }

    return node;
}

struct Node_t *rotateLeft(struct Node_t *node) {
    struct Node_t *newRoot = node   ->right;
    node   ->right         = newRoot->left;
    newRoot->left          = node;

    _recountHeight(node);
    _recountHeight(newRoot);

    return newRoot;
}

struct Node_t *rotateRight(struct Node_t *node) {
    struct Node_t *newRoot = node   ->left;
    node   ->left          = newRoot->right;
    newRoot->right         = node;

    _recountHeight(node);
    _recountHeight(newRoot);

    return newRoot;
}

struct Node_t *_merge(struct Node_t *left, struct Node_t *right) {
    if (!left)  return right;
    if (!right) return left;

    if (left->key > right->key) {
        left->left = _merge(left, right->left);
        return right;
    }

    // else
    right->right = _merge(left->right, right);
    return left;
}

void _split(Elem_t key, struct Node_t *current, struct Node_t **outLeft, struct Node_t **outRight) {
    struct Node_t *newNode = NULL;

    if (key >= current->key) {

        if (!(current->right)) *outRight = NULL;
        else _split(key, current->right, &newNode, outRight);

        current->right = newNode;
        *outLeft = current;
    } else {
        if (!(current->left)) *outLeft = NULL;
        else _split(key, current->left, outLeft, &newNode);

        current->left = newNode;
        *outRight = current;
    }
}


struct Node_t *_insertSM(struct Node_t *node, Elem_t key, int priority) {
    if (!node) return nodeCtor(key, NULL, NULL);

    struct Node_t *outLeft  = NULL;
    struct Node_t *outRight = NULL;

    _split(key, node, &outLeft, &outRight);

    return nodeCtor(key, outLeft, outRight);
}

void insertSM(struct AVL_t *treap, Elem_t key) {
    ON_ERROR(!treap, "Nullptr", );

    treap->root = _insertSM(treap->root, key, rand());
}

// REMOVE SECTION

void _removeSM(struct Node_t **node,  Elem_t key) {
    if (!node)    return;      // not found
    if (!(*node)) return;

    if ((*node)->key < key) {
        _removeSM(&((*node)->right), key);
    }
    else if ((*node)->key > key) {
        _removeSM(&((*node)->left),  key);
    }
    else     *node = _merge((*node)->left, (*node)->right);
}

void removeSM(struct AVL_t *treap, Elem_t key) {
    ON_ERROR(!treap, "Nullptr",);

    _removeSM(&(treap->root), key);
}

struct Node_t *_addKey(struct Node_t *node, Elem_t key) {
    // leaf case
    if (!node) return nodeCtor(key, NULL, NULL);

    if (key < node->key) {
        node->left  = _addKey(node->left,  key);
    }
    else if (key == node->key) {
        return node;
    }
    else {
        node->right = _addKey(node->right, key);
    }

    return balance(node);
}

void addKey (struct AVL_t *tree, Elem_t key) {
    if(!tree) return;

    tree->root = _addKey(tree->root, key);
}

struct Node_t *_removeMin(struct Node_t *node) {
    ON_ERROR(!node, "Nullptr", nullptr);

    if (!node->left) return node->right;
    node->left = _removeMin(node->left);
    return balance(node);
}

struct Node_t *_findMin(struct Node_t *node) {
    ON_ERROR(!node, "Nullptr", nullptr);

    if (node->left) return _findMin(node->left);
    return node;
}

struct Node_t *_removeKey(struct Node_t *node, Elem_t key) {
    if (!node) return nullptr;

    if      (key < node->key) node->left  = _removeKey(node->left,  key);
    else if (key > node->key) node->right = _removeKey(node->right, key);
    else {
        Node_t *left  = node->left;
        Node_t *right = node->right;

        // nodeDtor(node);

        if (!right) return left;
        Node_t *mini = _findMin(right);
        mini->right  = _removeMin(right);
        mini->left   = left;
        return balance(mini);
    }

    return node;
}

void            removeKey(struct AVL_t *tree, Elem_t key) {
    ON_ERROR(!tree, "Nullptr", );

    _removeKey(tree->root, key);
}

void graphNode(struct Node_t *node, FILE *tempFile) {
    fprintf(
                tempFile, 
                "\tlabel%p[shape=record, style=\"rounded, filled\", fillcolor=red, label=\"{ {val: %d | height: %d ",
                node,
                node->key,
                node->height
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

void drawConnections(struct Node_t *node, FILE *tempFile) {
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

void graphDump(struct Node_t *node) {
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

typedef void (*TestFunc_t)(void *tree, int *measureData);

double medianTime(TestFunc_t func, void *tree, int *measureData, int repeatCount) {
    double allTime = 0;
    for (int i = 0; i < repeatCount; i++) {
        auto start = std::chrono::high_resolution_clock::now(); 
        func(tree, measureData);
        auto end   = std::chrono::high_resolution_clock::now(); 

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        allTime += duration.count();
    }

    return allTime / repeatCount;
}

int main() {
    srand((unsigned) time(NULL));

    for (int i = 100000; i <= 1000000; i+= 100000) {
        printf("IN %d\n", i);

        AVL_t *tree = treeCtor();
        int measureData[1000000] = {};
 
        for (int j = 0; j < i; j++) {
            measureData[j] = j;
        }

        double allTime = 0;

        auto start = std::chrono::high_resolution_clock::now(); 
        for (int j = 0; j < 5; j++) {
            for (int m = 0; m < i; m++) {
                insertSM(tree, measureData[m]);
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
                removeSM(tree, measureData[m]);
            }
        }
        end   = std::chrono::high_resolution_clock::now(); 
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        allTime += duration.count();
        printf("DEL %lf\n", allTime / 5);
    }

    return 0;
}