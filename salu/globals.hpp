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

// filter_stream
const uint8_t NO_FILE = 0x4, END = 0x10;
const ushort PIECE_SIZE = 25;

// filter_stream & salu
const uint8_t READ_ERR = 0x8;

// salu
const uint8_t WORKING = 0x2, UNEXPECTED = 0x4, INCOMPATIBLE = 0x10;


#endif
