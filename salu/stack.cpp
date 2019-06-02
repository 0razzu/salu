#ifndef stack_cpp
#define stack_cpp


#include "stack.hpp"


template <typename StackBlockData>
Stack<StackBlockData>::Stack() {
    prev = nullptr;
    top = nullptr;
    error_code = OK;
    k = 0;
}


template <typename StackBlockData>
Stack<StackBlockData>::Stack(const Stack<StackBlockData> &Stack0) {
    StackBlock<StackBlockData> *curr = Stack0.top, *temp, *curr_last;
    ushort i;
    
    top = nullptr;
    error_code = Stack0.error_code;
    k = Stack0.k;
    
    if (!(error_code & MEM_ERR)) {
        if (Stack0.prev)
            prev = new(std::nothrow) StackBlock<StackBlockData>;
        
        else
            prev = nullptr;
        
        if ((!Stack0.prev) || (prev)) {
            if (curr) {
                top = new(std::nothrow) StackBlock<StackBlockData>;
                
                if (top) {
                    for (i = 0; i < StackBlockSize; i++)
                        top->data[i] = curr->data[i];
                    
                    top->next = nullptr;
                    
                    curr_last = top;
                    
                    curr = curr->next;
                }
                
                else
                    error_code |= MEM_ERR;
            }
            
            while ((curr) && (!(error_code & MEM_ERR))) {
                temp = new(std::nothrow) StackBlock<StackBlockData>;
                
                if (temp) {
                    for (i = 0; i < StackBlockSize; i++)
                        temp->data[i] = curr->data[i];
                    
                    temp->next = nullptr;
                    
                    curr_last->next = temp;
                    curr_last = curr_last->next;
                    
                    curr = curr->next;
                }
                
                else
                    error_code |= MEM_ERR;
            }
        }
        
        else
            error_code |= MEM_ERR;
    }
}


template <typename StackBlockData>
Stack<StackBlockData>::~Stack() {
    clear();
}


template <typename StackBlockData>
void Stack<StackBlockData>::operator =(const Stack<StackBlockData> &Stack0) {
    StackBlock<StackBlockData> *curr = Stack0.top, *temp, *curr_last;
    ushort i;
    
    clear();
    
    top = nullptr;
    error_code = Stack0.error_code;
    k = Stack0.k;
    
    if (!(error_code & MEM_ERR)) {
        if (Stack0.prev)
            prev = new(std::nothrow) StackBlock<StackBlockData>;
        
        if ((!Stack0.prev) || (prev)) {
            if (curr) {
                top = new(std::nothrow) StackBlock<StackBlockData>;
                
                if (top) {
                    for (i = 0; i < StackBlockSize; i++)
                        top->data[i] = curr->data[i];
                    
                    top->next = nullptr;
                    
                    curr_last = top;
                    
                    curr = curr->next;
                }
                
                else
                    error_code |= MEM_ERR;
            }
            
            while ((curr) && (!(error_code & MEM_ERR))) {
                temp = new(std::nothrow) StackBlock<StackBlockData>;
                
                if (temp) {
                    for (i = 0; i < StackBlockSize; i++)
                        temp->data[i] = curr->data[i];
                    
                    temp->next = nullptr;
                    
                    curr_last->next = temp;
                    curr_last = curr_last->next;
                    
                    curr = curr->next;
                }
                
                else
                    error_code |= MEM_ERR;
            }
        }
        
        else
            error_code |= MEM_ERR;
    }
}


template <typename StackBlockData>
void Stack<StackBlockData>::push(StackBlockData data) {
    StackBlock<StackBlockData> *temp;
    
    if (!(error_code & MEM_ERR)) {
        if (k % StackBlockSize != 0) {
            top->data[k % StackBlockSize] = data;
            k++;
        }
        
        else if (prev) {
            top = prev;
            prev = nullptr;
            top->data[0] = data;
            k++;
        }
        
        else {
            temp = new(std::nothrow) StackBlock<StackBlockData>;
            
            if (temp) {
                temp->data[0] = data;
                
                temp->next = top;
                top = temp;
                
                k++;
            }
            
            else
                error_code |= MEM_ERR;
        }
    }
}


template <typename StackBlockData>
StackBlockData Stack<StackBlockData>::pop() {
    StackBlockData data = 0;
    
    if (top) {
        data = top->data[(k - 1) % StackBlockSize];
        k--;
        
        if (k % StackBlockSize == 0) {
            if (prev)
                delete prev;
            
            prev = top;
            top = top->next;
        }
        
        else if ((k % StackBlockSize < 0.75 * StackBlockSize) && (prev)) {
            delete prev;
            prev = nullptr;
        }
    }
    
    else
        error_code |= EMPTY;
    
    return data;
}


template <typename StackBlockData>
void Stack<StackBlockData>::change(StackBlockData data) {
    if (top)
        top->data[(k - 1) % StackBlockSize] = data;
    
    else
        error_code |= EMPTY;
}


template <typename StackBlockData>
StackBlockData Stack<StackBlockData>::peek() {
    StackBlockData data = 0;
    
    if (top)
        data = top->data[(k - 1) % StackBlockSize];
    
    else
        error_code |= EMPTY;
    
    return data;
}


template <typename StackBlockData>
void Stack<StackBlockData>::del() {
    if (top) {
        k--;
        
        if (k % StackBlockSize == 0) {
            if (prev)
                delete prev;
            
            prev = top;
            top = top->next;
        }
        
        else if ((k % StackBlockSize < 0.75 * StackBlockSize) && (prev)) {
            delete prev;
            prev = nullptr;
        }
    }
    
    else
        error_code |= EMPTY;
}


template <typename StackBlockData>
void Stack<StackBlockData>::clear() {
    StackBlock<StackBlockData> *temp;
    
    while (top) {
        temp = top;
        top = top->next;
        
        delete temp;
    }
    
    if (prev)
        delete prev;
    
    k = 0;
}


template <typename StackBlockData>
unsigned long long Stack<StackBlockData>::getK() {
    return k;
}


template <typename StackBlockData>
bool Stack<StackBlockData>::isEmpty() {
    return k == 0;
}


template <typename StackBlockData>
uint8_t Stack<StackBlockData>::error() {
    uint8_t error_code_r = error_code;
    
    error_code &= MEM_ERR;
    
    return error_code_r;
}


#endif
