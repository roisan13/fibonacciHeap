#include <fstream>
#include <vector>
#include <iostream>
#include "FibonacciHeap.h"


std::ifstream f("mergeheap.in");
std::ofstream g("mergeheap.out");


int main() {
    int N,Q;
    std::vector< FibonacciHeap<int>* > heapArray;
    f >> N >> Q;
    for(int i = 0; i < N + 1; ++i)
    {
        heapArray.push_back(new FibonacciHeap<int>());
    }
    for(int i = 0; i < Q; ++i)
    {
        //std::cout << i << ".\n";
        int tipOperatie;
        f >> tipOperatie;
        if (tipOperatie == 1){
            int multime, val;
            f >> multime >> val;
            heapArray[multime]->insert(val);

        }
        else if (tipOperatie == 2){
            int multime;
            f >> multime;
            g << heapArray[multime]->extractMax() << "\n";
        }
        else if (tipOperatie == 3){
            int mult1, mult2;
            f >> mult1 >> mult2;
            heapArray[mult1]->merge(heapArray[mult2]);
            heapArray[mult2] = new FibonacciHeap<int>();

        }
    }
    return 0;
}
