#ifndef stack_hpp
#define stack_hpp


#include "globals.hpp"
#include <iostream>


template <typename StackBlockData>
struct StackBlock {
    StackBlockData data[StackBlockSize];
    StackBlock *next;
};


template <typename StackBlockData>
class Stack {
private:
    StackBlock<StackBlockData> *prev, *top;
    uint8_t state;
    unsigned long long k;
    

public:
    Stack();
    Stack(const Stack&);
    ~Stack();
    
    void operator =(const Stack&);
    
    void push(StackBlockData);
    StackBlockData pop();
    
    void change(StackBlockData);
    StackBlockData peek();
    void del();
    
    void clear();
    
    unsigned long long getK();
    bool isEmpty();
    
    uint8_t getState();
};


#endif
