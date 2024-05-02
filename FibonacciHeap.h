#ifndef FIBHEAP_FIBONACCIHEAP_H
#define FIBHEAP_FIBONACCIHEAP_H


#include <iostream>
#include <vector>
#include <algorithm>

///                       -----
///     FIBONACCI HEAP de MAXIM !
///                       -----


template <class V> class FibonacciHeap;

const double INF_fibonacci_heap = 2000000001;

template <class V> struct node {
    node<V>* left;
    node<V>* right;
    node<V>* child;
    node<V>* parent;
    V val;
    bool marked;
    int degree;
};

template <class V> class FibonacciHeap{
private:
    node<V>* maxNode;
    node<V>* rootList;


    node<V>* constructNode(V value){
        auto* newNode = new node<V>;
        newNode->left = newNode;
        newNode->right = newNode;
        newNode->child = nullptr;
        newNode->parent = nullptr;
        newNode->degree = 0;
        newNode->val = value;
        newNode->marked = false;
        return newNode;
    }
    void mergeWithRoot(node<V>* mergedNode){
        if (rootList == nullptr)
            rootList = mergedNode;
        else {
            mergedNode->right = rootList;
            mergedNode->left = rootList->left;
            rootList->left->right = mergedNode;
            rootList->left = mergedNode;
        }
    }

    void removeFromRoot(node<V>* removedNode){
        if (removedNode == rootList)
            rootList = removedNode->right;
        removedNode->left->right = removedNode->right;
        removedNode->right->left = removedNode->left;
    }

    void removeFromChildren(node<V>* removedChild, node<V>* parent){
        if (parent->child == parent->child->right)
            parent->child = nullptr;
        else if (parent->child == removedChild) {
            parent->child = removedChild->right;
            removedChild->right->parent = parent;
        }
        removedChild->left->right = removedChild->right;
        removedChild->right->left = removedChild->left;

    }

    void mergeWithChild(node<V>* newChild, node<V>* parent){

        if (parent->child == nullptr)
            parent->child = newChild;
        else{
            // Inserez mereu la dreapta primului copil
            newChild->right = parent->child->right;
            newChild->left = parent->child;
            parent->child->right->left = newChild;
            parent->child->right = newChild;
        }
    }

    void heapLink(node<V>* child, node<V>* parent){
        removeFromRoot(child);
        child->left = child->right = child;
        parent->degree++;
        mergeWithChild(child, parent);
        child->parent = parent;


    }

    void cleanUp(){
        // magic number 128 = 64 bits x 2
        // 64 seems to be working just fine tho
        // increase to 128 for bigger values?
        std::vector< node<V>* > degreeTable = {64, nullptr};
        std::vector< node<V>* > rootNodes = {rootList};

        node<V>* p = rootList->right;
        while (p != rootList) {
            rootNodes.push_back(p);
            p = p->right;
        }

        for (auto rootNode : rootNodes){
            int deg = rootNode->degree;
            while(degreeTable[deg] != nullptr){
                node<V>* degNode = degreeTable[deg];
                if(rootNode->val < degNode->val)
                    std::swap(rootNode, degNode);
                heapLink(degNode, rootNode);
                degreeTable[deg] = nullptr;
                deg++;
            }
            degreeTable[deg] = rootNode;
        }
        for(int i = 0; i < 64; i++)
            if (degreeTable[i] != nullptr)
                if( degreeTable[i]->val > maxNode->val)
                    maxNode = degreeTable[i];
    }

    void cut(node<V>* removedChild, node<V>* parent){
        removeFromChildren(removedChild, parent);
        parent->degree -= 1;
        mergeWithRoot(removedChild);
        removedChild->parent = NULL;
        removedChild->marked = false;
    }

    void cascadingCut(node<V>* currentNode){
        node<V>* currentParent = currentNode->parent;
        if (currentParent != NULL) {
            if (!currentNode->marked)
                currentNode->marked = true;
            else {
                cut(currentNode, currentParent);
                cascadingCut(currentParent);
            }
        }
    }

public:
    FibonacciHeap(){
        maxNode = nullptr;
        rootList = nullptr;
    }
    ~FibonacciHeap() = default;

    void insert(V insertedValue){
        node<V>* insertedNode = constructNode(insertedValue);

        mergeWithRoot(insertedNode);

        if (maxNode == nullptr || maxNode->val < insertedValue)
            maxNode = insertedNode;
    }

    void merge(FibonacciHeap<V>* other) {

        if (rootList == nullptr){
            rootList = other->rootList;
            maxNode = other->maxNode;
        }
        else if(other->rootList != nullptr) {

            node<V>* last = other->rootList->left;   // ultimul nod dupa merge
            other->rootList->left = rootList->left;  // rootList->left = ultimul din primul heap
            rootList->left->right = other->rootList; // ult din primul heap ->left = other.rootList
            rootList->left = last;                   // rootList->left = ultimul nod dupa merge
            rootList->left->right = rootList;        // ultimul nod dupam merge ->right = rootList

            // maxNode = max(minNode, other.minNode)
            if (maxNode->val < other->maxNode->val)
                maxNode = other->maxNode;
        }
    }

    V getMaximum(){
        return maxNode->val;
    };

    V extractMax(){
        node<V>* z = maxNode;
        V maxVal = 0;

        if (z != nullptr){
            if (z->child != nullptr) {
                node<V>* p = rootList->right;

                // AICI PT UN CAZ AM Z->DEGREE MAI MARE CU 1 DECAT AR TREBUI SA FIE
                // NU-MI DAU SEAMA DE CE
                std::vector<node<V> *> children = {};
                node<V> *currentChild = z->child;
                do{
                    auto temp = currentChild;
                    // std::cout << currentChild->val << "-> child. \n";
                    children.push_back(temp);
                    currentChild = currentChild->right;
                }while(currentChild != z->child);

                for (auto child: children) {
                    mergeWithRoot(child);
                    child->parent = nullptr;
                }
            }

            removeFromRoot(z);

            if (z == z->right){
                // Am extras dintr-un heap cu un singur element (care era si minim)
                maxNode = nullptr;
                rootList = nullptr;
            }
            else{
                maxNode = z->right;
                cleanUp();
            }

            maxVal = z->val;
            delete z;
        }
        return maxVal;
    }

    void increaseValue(node<V>* incNode, V incValue){
        if (incValue < incNode->val)
            return;
        incNode->val = incValue;
        node<V>* parentNode = incNode->parent;
        if (parentNode != NULL && incNode->val > parentNode->val){
            cut(incNode, parentNode);
            cascadingCut(parentNode);
        }
        if (incValue > maxNode->val)
            maxNode = incNode;

    }

    node<V>* deleteNode(node<V>* delNode){
        decreaseValue(delNode, INF_fibonacci_heap);
        return extractMax();
    }

};
#endif //FIBHEAP_FIBONACCIHEAP_H
