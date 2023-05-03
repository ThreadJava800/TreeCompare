#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include <chrono>

const int  PLUS_INF = INT_MAX;
const int MINUS_INF = INT_MIN;

#define ON_ERROR(expr, errStr, retVal) {                 \
    if (expr) {                                           \
        fprintf(stderr, "FATAL ERROR: %s\n", errStr);      \
        return retVal;                                      \
    }                                                        \
}                                                             \

struct Node_t {
    int key;

    struct Node_t *next = nullptr;
    struct Node_t *down = nullptr;
};

struct SkipList_t {
    struct Node_t *head = nullptr;
    struct Node_t *tail = nullptr;
};

struct SkipList_t *treeCtor();
struct Node_t  *nodeCtor(int key, struct Node_t *next, struct Node_t *down);

struct Node_t *_insert(Node_t *res, int key);
void insertElement(SkipList_t **list, int key);

void remove(Node_t *node, int key);

struct SkipList_t *treeCtor() {
    SkipList_t *list = (SkipList_t *) calloc(1, sizeof(SkipList_t));
    ON_ERROR(!list, "Nullptr", nullptr);

    list->head = nodeCtor(MINUS_INF, nullptr, nullptr);
    list->tail = nodeCtor(PLUS_INF, nullptr, nullptr);

    return list;
}

struct Node_t  *nodeCtor(int key, struct Node_t *next, struct Node_t *down) {
    Node_t *node = (Node_t *) calloc(1, sizeof(Node_t));
    ON_ERROR(!node, "Nullptr", nullptr);

    node->key  = key;
    node->down = down;
    node->next = next;

    return node;
}

struct Node_t *_insert(Node_t *res, int key) {
    while (res->next && res->next->key < key)
    {
        res = res->next;
    }
    
    Node_t *down = nullptr;
    if (!res->down) down = nullptr;
    else down = _insert(res->down, key);

    if (down || !res->down) {       // first level or add
        res->next = nodeCtor(key, res->next, down);
        if (rand() % 2) return res->next;
        return nullptr;
    }
    return nullptr;
}

void insertElement(SkipList_t **list, int key) {
    Node_t *res = _insert((*list)->head, key);
    if (res) {
        SkipList_t *newLvl = treeCtor();
        newLvl->head->next = nodeCtor(key, newLvl->tail, res);
        *list = newLvl;
    }
}

void remove(Node_t *node, int key) {
    while (node->next && node->next->key < key)
    {
        node = node->next;
    }
    
    if (node->down) remove(node->down, key);
    if (node->next && node->next->key == key) node->next = node->next->next;
}

int main() {
    srand((unsigned) time(NULL));

    for (int i = 100000; i <= 1000000; i+= 100000) {
        printf("IN %d\n", i);

        SkipList_t *tree = treeCtor();
        int measureData[1000000] = {};
 
        for (int j = 0; j < i; j++) {
            measureData[j] = rand();
        }

        double allTime = 0;

        auto start = std::chrono::high_resolution_clock::now(); 
        for (int j = 0; j < 5; j++) {
            for (int m = 0; m < i; m++) {
                insertElement(&tree, measureData[m]);
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
                remove(tree->head, measureData[m]);
            }
        }
        end   = std::chrono::high_resolution_clock::now(); 
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        allTime += duration.count();
        printf("DEL %lf\n", allTime / 5);
    }

    return 0;
}