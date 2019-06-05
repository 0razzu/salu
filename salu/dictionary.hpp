#ifndef dictionary_hpp
#define dictionary_hpp


#include "globals.hpp"
#include <iostream>


template <typename DictionaryData>
struct DictionaryBlock {
    char key[DictionaryKeySize];
    DictionaryData value;
    DictionaryBlock *next;
};


template <typename DictionaryData>
class Dictionary {
private:
    DictionaryBlock<DictionaryData> *first_block, *last_block;
    uint8_t state;
    
public:
    Dictionary();
    ~Dictionary();
    
    void add(const char[DictionaryKeySize], DictionaryData);
    DictionaryData getValue(const char[DictionaryKeySize]);
    void del(const char[DictionaryKeySize]);
    
    bool isEmpty();
    uint8_t getState();
};


#endif
