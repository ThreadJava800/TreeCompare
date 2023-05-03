#define MAKE_CHECKS 0
#include "include.h"

typedef int Elem_t;
static Elem_t TMAX = INT_MAX;

struct TreeElem_t {
    Elem_t      value = 0;

    TreeElem_t *left  = nullptr;
    TreeElem_t *right = nullptr;
};

struct NaiveBin_t
{
    TreeElem_t *root = nullptr;
};


namespace NaiveBin {
    NaiveBin_t *ctor();

    void        add(NaiveBin_t *tree, Elem_t value);
    TreeElem_t *search(TreeElem_t *tree, Elem_t value);
    void        del(NaiveBin_t *tree, Elem_t value);

    void dtor(NaiveBin_t *tree);
};

TreeElem_t *nodeCtor(Elem_t value);
TreeElem_t *add(TreeElem_t *root, Elem_t value);
Elem_t getMinNode(TreeElem_t *node);
TreeElem_t *deleteRecursive(TreeElem_t *node, Elem_t value);

void graphNode(struct TreeElem_t *node, FILE *tempFile);
void drawConnections(struct TreeElem_t *node, FILE *tempFile);
void graphDump(struct TreeElem_t *node);

void test();