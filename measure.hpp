#include <chrono>

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