#ifndef globals_hpp
#define globals_hpp


#include <iostream>


// universal
const uint8_t OK = 0x0, MEM_ERR = 0x1, EMPTY = 0x2;

// stack
const ushort StackBlockSize = 8;

// dictionary
const uint8_t NO_ELEM = 0x4, ALREADY_EXISTS = 0x8;
const ushort DictionaryKeySize = 16;


#endif
