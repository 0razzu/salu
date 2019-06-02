#ifndef dictionary_cpp
#define dictionary_cpp


#include "dictionary.hpp"
#include "globals.hpp"


template <typename DictionaryData>
Dictionary<DictionaryData>::Dictionary() {
    first_block = NULL;
    last_block = NULL;
    error_code = OK;
}


template <typename DictionaryData>
Dictionary<DictionaryData>::~Dictionary() {
    DictionaryBlock<DictionaryData> *temp, *next = first_block;
    
    while (next) {
        temp = next;
        next = next->next;
        
        delete temp;
    }
}


template <typename DictionaryData>
void Dictionary<DictionaryData>::add(const char key_i[DictionaryKeySize], DictionaryData value_i) {
    DictionaryBlock<DictionaryData> *temp = first_block;
    
    error_code &= !(MEM_ERR | ALREADY_EXISTS);
    
    while ((temp) && (!(error_code & ALREADY_EXISTS))) {
        if (strcmp(temp->key, key_i) == 0)
            error_code |= ALREADY_EXISTS;
        
        temp = temp->next;
    }
    
    if (!(error_code & ALREADY_EXISTS)) {
        temp = new(std::nothrow) DictionaryBlock<DictionaryData>;
            
        if (temp) {
            temp->key[0] = '\0';
            strcat(temp->key, key_i);
            temp->value = value_i;
            temp->next = NULL;
            
            if (first_block) {
                last_block->next = temp;
                last_block = last_block->next;
            }
                    
            else {
                first_block = temp;
                last_block = temp;
            }
        }
            
        else
            error_code |= MEM_ERR;
    }
}


template <typename DictionaryData>
DictionaryData Dictionary<DictionaryData>::getValue(const char key_i[DictionaryKeySize]) {
    DictionaryData value_r = 0;
    DictionaryBlock<DictionaryData> *temp = first_block;
    bool found = 0;
    
    if (isEmpty())
        error_code |= (EMPTY | NO_ELEM);
    
    else {
        error_code &= !NO_ELEM;
        
        while ((temp) && (!found)) {
            if (strcmp(temp->key, key_i) == 0) {
                value_r = temp->value;
                found = 1;
            }
            
            temp = temp->next;
        }
        
        if (!found)
            error_code |= NO_ELEM;
    }
    
    return value_r;
}


template <typename DictionaryData>
void Dictionary<DictionaryData>::del(const char key_i[DictionaryKeySize]) {
    DictionaryBlock<DictionaryData> *prev = NULL, *temp = first_block;
    bool found = 0;
    
    if (isEmpty())
        error_code |= (EMPTY | NO_ELEM);
    
    else {
        error_code &= !NO_ELEM;
        
        while ((temp) && (!found)) {
            if (strcmp(temp->key, key_i) == 0)
                found = 1;
            
            else {
                prev = temp;
                temp = temp->next;
            }
        }
        
        if (found) {
            if (prev)
                prev->next = temp->next;
            
            else {
                first_block = first_block->next;
                
                if (!first_block)
                    last_block = NULL;
            }
            
            delete temp;
        }
        
        else
            error_code |= NO_ELEM;
    }
}


template <typename DictionaryData>
bool Dictionary<DictionaryData>::isEmpty() {
    return first_block == NULL;
}


template <typename DictionaryData>
uint8_t Dictionary<DictionaryData>::error() {
    uint8_t error_code_r = error_code;
    
    error_code = OK;
    
    return error_code_r;
}


#endif
