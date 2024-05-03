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

template <class V> class node {
private:
    node<V>* left;
    node<V>* right;
    node<V>* child;
    node<V>* parent;
    V val;
    bool marked;
    int degree;
public:
    friend class FibonacciHeap<V>;
    explicit node(V value){
        left = this;
        right = this;
        child = nullptr;
        parent = nullptr;
        degree = 0;
        val = value;
        marked = false;
    }

    ~node() = default;
};

template <class V> class FibonacciHeap{
private:
    node<V>* maxNode;
    node<V>* rootList;

    void mergeWithRoot(node<V>* mergedNode){
        mergedNode->parent = nullptr;
        if (rootList != nullptr){
            mergedNode->right = rootList;
            mergedNode->left = rootList->left;
            rootList->left->right = mergedNode;
            rootList->left = mergedNode;
        }
        else{
            mergedNode->right = mergedNode;
            mergedNode->left = mergedNode;
            rootList = mergedNode;
        }
    }

    void removeFromRoot(node<V>* removedNode){
        if (removedNode == rootList)
            rootList = removedNode->right;
        if (removedNode->right != removedNode){
            removedNode->left->right = removedNode->right;
            removedNode->right->left = removedNode->left;
        }
        if (removedNode->parent != nullptr) {
            if (removedNode->parent->degree == 1)
                removedNode->parent->child = nullptr;
            else removedNode->parent->child = removedNode->right;
            removedNode->parent->degree--;
        }

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
        if(parent->degree == 0){
            parent->child = newChild;
            newChild->right = newChild;
            newChild->left = newChild;
            newChild->parent = parent;
        }
        else{
            node<V>* firstChild = parent->child;
            node<V>* firstChildsLeft = firstChild->left;
            firstChild->left = newChild;
            newChild->right = firstChild;
            newChild->left = firstChildsLeft;
            firstChildsLeft->right = newChild;
        }
        newChild->parent = parent;
        parent->degree++;
    }

    void heapLink(node<V>* child, node<V>* parent){
        removeFromRoot(child);
        mergeWithChild(child, parent);
        child->marked = false;
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
        removedChild->parent = nullptr;
        removedChild->marked = false;
    }

    void cascadingCut(node<V>* currentNode){
        node<V>* currentParent = currentNode->parent;
        if (currentParent != nullptr) {
            if (!currentNode->marked)
                currentNode->marked = true;
            else {
                cut(currentNode, currentParent);
                cascadingCut(currentParent);
            }
        }
    }

    void freeMemory(node<V>* x){
        if ( x != nullptr )
        {
            node<V>* t1 = x;
            do{
                node<V>* t2 = t1;
                t1 = t1->right;
                freeMemory(t2->child);
                delete t2;
            } while(t1 != x);
        }
    }

public:
    FibonacciHeap(){
        maxNode = nullptr;
        rootList = nullptr;
    }

    ~FibonacciHeap(){
        freeMemory(rootList);
        rootList = nullptr;
        maxNode = nullptr;
    }

    void insert(V insertedValue){
        node<V>* insertedNode = new node<V>(insertedValue);

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

        if (maxNode == nullptr) {
            // throw (runtime_error("Can't get maximum! Heap is empty!"));
        }
        return maxNode->val;

    };

    V extractMax(){
        node<V>* z = maxNode;
        V maxVal;
        if (z != nullptr){
            if (z->child != nullptr) {

                node<V>* currentChild = z->child;
                do{
                    auto temp = currentChild;
                    currentChild = currentChild->right;
                    mergeWithRoot(temp);
                }while(currentChild != z->child);
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
            return maxVal;

        }
        else {
            //throw(runtime_error("Can't pop! Heap is empty!"));
        }
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
