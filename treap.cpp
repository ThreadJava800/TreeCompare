#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#include <chrono>

typedef int Elem_t;

const int MAX_COM_LENGTH = 10;

const char INSERT_COM[] = "insert";
const char DELETE_COM[] = "delete";
const char EXISTS_COM[] = "exists";
const char NEXT_COM[]   = "next";
const char PREV_COM[]   = "prev";
const char KTH_COM[]    = "kth";

#define true 1;
#define false 0;

const Elem_t DEFAULT_NONE = INT_MAX;

struct Node_t
{
    Elem_t key;
    int prior;

    size_t childrenCount; 

    struct Node_t *left;
    struct Node_t *right;
};


struct Treap_t {
    struct Node_t *root;
};

//========TREAP FUNCTIONS===========
struct Treap_t *treapCtor();
struct Node_t  *nodeCtor (Elem_t key, int prior, struct Node_t *left, struct Node_t *right);
void   nodeDtor (struct Node_t  *node);
void   treapDtor(struct Treap_t *treap);

size_t getNodeSize(struct Node_t *node);
void   updNodeSize(struct Node_t *node);

struct Node_t *_merge(struct Node_t *left, struct Node_t *right);
void   _split(Elem_t key, struct Node_t *current, struct Node_t **outLeft, struct Node_t **outRight);

struct Node_t *_insert(struct Node_t *node, Elem_t key, int priority);
void   insert (struct Treap_t *treap, Elem_t key);

void _remove(struct Node_t **node,  Elem_t key);
void  removeTreap(struct Treap_t *treap, Elem_t key);

int _exists(struct Node_t  *node,  Elem_t key);
int  exists(struct Treap_t *treap, Elem_t key);

Elem_t _next(struct Node_t  *node,  Elem_t cmpEl);
Elem_t  next(struct Treap_t *treap, Elem_t cmpEl);

Elem_t _prev(struct Node_t  *node,  Elem_t cmpEl);
Elem_t  prev(struct Treap_t *treap, Elem_t cmpEl);

Elem_t _kth(struct Node_t  *node,  size_t k);
Elem_t  kth(struct Treap_t *treap, size_t k);

// DEBUG
void graphNode(struct Node_t *node, FILE *tempFile);
void drawConnections(struct Node_t *node, FILE *tempFile);
void graphDump(struct Node_t *node, int id);
void printInOrder(struct Node_t* node);

//=========CONTEST STAFF=============
void parser();

#define INPUT_CHECK(readFunc, res) {        \
    int factRes = readFunc;                  \
    if (factRes != res) abort();              \
}                                              \


#define ON_ERROR(expr, errStr, retVal) {                                  \
    if (expr) {                                                            \
        fprintf(stderr, "FATAL ERROR: %s in %d\n", errStr, __LINE__);       \
        return retVal;                                                       \
    }                                                                         \
}                                                                              \
//=========================================== 

struct Treap_t *treapCtor() {
    struct Treap_t *treap = (struct Treap_t *) calloc(1, sizeof(struct Treap_t));
    ON_ERROR(!treap, "Unable to alloc mem", NULL);

    treap->root = NULL;

    return treap;
}

struct Node_t  *nodeCtor (Elem_t key, int prior, struct Node_t *left, struct Node_t *right) {
    struct Node_t *node = (struct Node_t *) calloc(1, sizeof(struct Node_t));
    ON_ERROR(!node, "Unable to alloc mem", NULL);

    node->key           = key;
    node->prior         = prior;

    node->left  = left;
    node->right = right;

    updNodeSize(node);

    return node;
}

void nodeDtor (struct Node_t *node) {
    if(!node) return;

    if (node->left)  nodeDtor(node->left);
    if (node->right) nodeDtor(node->right);

    free(node);
}

void treapDtor(struct Treap_t *treap) {
    ON_ERROR(!treap, "Nullptr",);

    nodeDtor(treap->root);
    free(treap);
}

// MERGE & SPLIT

size_t getNodeSize(struct Node_t *node) {
    return (node == NULL) ? 0 : node->childrenCount;
}

void updNodeSize(struct Node_t *node) {
    if (!node) return;

    node->childrenCount = getNodeSize(node->left) + getNodeSize(node->right) + 1;
}

struct Node_t *_merge(struct Node_t *left, struct Node_t *right) {
    if (!left)  return right;
    if (!right) return left;

    if (left->prior > right->prior) {
        left->right = _merge(left->right, right);
        updNodeSize(left);
        return left;
    }

    // else
    right->left = _merge(left, right->left);
    updNodeSize(right);
    return right;
}

void _split(Elem_t key, struct Node_t *current, struct Node_t **outLeft, struct Node_t **outRight) {
    struct Node_t *newNode = NULL;

    if (key >= current->key) {

        if (!(current->right)) *outRight = NULL;
        else _split(key, current->right, &newNode, outRight);

        current->right = newNode;
        *outLeft = current;

        updNodeSize(current);
    } else {
        if (!(current->left)) *outLeft = NULL;
        else _split(key, current->left, outLeft, &newNode);

        current->left = newNode;
        *outRight = current;

        updNodeSize(current);
    }
}

// INSERT SECTION

struct Node_t *_insert(struct Node_t *node, Elem_t key, int priority) {
    if (!node) return nodeCtor(key, priority, NULL, NULL);

    if (node->prior >= priority) {
        if (key < node->key) {
            node->left  = _insert(node->left, key, priority);
        } else {
            node->right = _insert(node->right, key, priority);
        }

        updNodeSize(node);

        return node;
    }

    struct Node_t *outLeft  = NULL;
    struct Node_t *outRight = NULL;

    _split(key, node, &outLeft, &outRight);

    return nodeCtor(key, priority, outLeft, outRight);
}

void insert(struct Treap_t *treap, Elem_t key) {
    ON_ERROR(!treap, "Nullptr", );

    if (exists(treap, key)) return;

    treap->root = _insert(treap->root, key, rand());
}

// REMOVE SECTION

void _remove(struct Node_t **node,  Elem_t key) {
    if (!node)    return;      // not found
    if (!(*node)) return;

    if ((*node)->key < key) {
        _remove(&((*node)->right), key);
        updNodeSize(*node);
    }
    else if ((*node)->key > key) {
        _remove(&((*node)->left),  key);
        updNodeSize(*node);
    }
    else     *node = _merge((*node)->left, (*node)->right);
    // upd everywhere
    updNodeSize(*node);
}

void removeTreap(struct Treap_t *treap, Elem_t key) {
    ON_ERROR(!treap, "Nullptr",);

    _remove(&(treap->root), key);
}

// EXISTS SECTION
int _exists(struct Node_t *node,  Elem_t key) {
    struct Node_t *cur = node;

    while (cur)
    {
        if (cur->key == key) return true;

        if (key < cur->key) cur = cur->left;
        else                cur = cur->right;
    }
    

    return false;
}

int exists(struct Treap_t *treap, Elem_t key) {
    ON_ERROR(!treap, "Nullptr", -1);

    return _exists(treap->root, key);
}

// NEXT SECTION

Elem_t _next(struct Node_t  *node,  Elem_t cmpEl) {
    struct Node_t *cur = node;
    struct Node_t *res = NULL;

    while (cur)
    {
        if(cur->key > cmpEl) {
            res = cur;
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }

    if (res) return res->key;
    
    return DEFAULT_NONE;
}

Elem_t next(struct Treap_t *treap, Elem_t cmpEl) {
    ON_ERROR(!treap, "Nullptr", DEFAULT_NONE);

    return _next(treap->root, cmpEl);
}

// PREV SECTION

Elem_t _prev(struct Node_t  *node,  Elem_t cmpEl) {
    struct Node_t *cur = node;
    struct Node_t *res = NULL;

    while (cur)
    {
        if(cur->key >= cmpEl) {
            cur = cur->left;
        } else {
            res = cur;
            cur = cur->right;
        }
    }

    if (res) return res->key;
    
    return DEFAULT_NONE;
}

Elem_t  prev(struct Treap_t *treap, Elem_t cmpEl) {
    ON_ERROR(!treap, "Nullptr", DEFAULT_NONE);

    return _prev(treap->root, cmpEl);
}

Elem_t _kth(struct Node_t *node,  size_t k) {
    struct Node_t *cur = node;
    
    while (cur)
    {
        size_t leftSize = getNodeSize(cur->left);
        if (leftSize == k) return cur->key;

        cur = leftSize > k ? cur->left : cur->right;
        if (leftSize < k) k -= (leftSize + 1);
    }
    

    return DEFAULT_NONE;
}

Elem_t  kth(struct Treap_t *treap, size_t k) {
    ON_ERROR(!treap, "Nullptr", DEFAULT_NONE);

    return _kth(treap->root, k);
}

// GRAPH DUMPS

void graphNode(struct Node_t *node, FILE *tempFile) {
    fprintf(
                tempFile, 
                "\tlabel%p[shape=record, style=\"rounded, filled\", fillcolor=red, label=\"{ {val: %d | prior: %d ",
                node,
                node->key,
                node->prior
            );  

    fprintf(
                tempFile, 
                " | {left: %p | right: %p | children: %lu | cur: %p} }}\"];\n", 
                node->left,
                node->right,
                node->childrenCount,
                node
            );

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
    ON_ERROR(!node, "Nullptr", );

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

void printInOrder(struct Node_t* node) { 
    if (node) { 
        printInOrder(node->left); 
        printf("key: %d priority: %d\n", node->key, node->prior); 
        printInOrder(node->right); 
    } 
} 

void parser() {
    char text[MAX_COM_LENGTH];
    int  number = 0;

    struct Treap_t *treap = treapCtor();

    while (scanf("%s %d", text, &number) == 2)
    {
        if (!strcmp(text, INSERT_COM)) {
            insert(treap, number);
        }
        if (!strcmp(text, EXISTS_COM)) {
            int res = exists(treap, number);
            if (res) printf("true\n");
            else     printf("false\n");
        }
        if (!strcmp(text, NEXT_COM)) {
            int res = next(treap, number);

            if (res == DEFAULT_NONE) printf("none\n");
            else                     printf("%d\n", res);
        }
        if (!strcmp(text, PREV_COM)) {
            int res = prev(treap, number);

            if (res == DEFAULT_NONE) printf("none\n");
            else                     printf("%d\n", res);
        }
        if (!strcmp(text, KTH_COM)) {
            int res = kth(treap, (size_t) number);

            if (res == DEFAULT_NONE) printf("none\n");
            else                     printf("%d\n", res);
        }
        if (!strcmp(text, DELETE_COM)) {
           removeTreap(treap, number);
        }
    }
    
}

int main() {
    srand((unsigned) time(NULL));

    for (int i = 100000; i <= 1000000; i+= 100000) {
        printf("IN %d\n", i);

        Treap_t *tree = treapCtor();
        int measureData[1000000] = {};
 
        for (int j = 0; j < i; j++) {
            measureData[j] = rand();
        }

        double allTime = 0;

        auto start = std::chrono::high_resolution_clock::now(); 
        for (int j = 0; j < 5; j++) {
            for (int m = 0; m < i; m++) {
                insert(tree, measureData[m]);
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
                removeTreap(tree, measureData[m]);
            }
        }
        end   = std::chrono::high_resolution_clock::now(); 
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        allTime += duration.count();
        printf("DEL %lf\n", allTime / 5);
    }

    return 0;
}