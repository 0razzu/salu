#ifndef globals_hpp
#define globals_hpp


#include <iostream>


// universal
const uint8_t OK = 0x0, MEM_ERR = 0x1;

// stack
const uint8_t EMPTY = 0x40;
const ushort StackBlockSize = 8;

// dictionary
const uint8_t NO_ELEM = 0x40;
const ushort DictionaryKeySize = 16;

// filter_stream
const uint8_t END = 0x40;
const ushort PIECE_SIZE = 25;

// filter_stream & salu & fill_dictionaries
const uint8_t READ_ERR = 0x8, NO_FILE = 0x20;

// salu
const uint8_t INCOMPATIBLE = 0x10, OPERANDS = 0x40, WORKING = 0x80;

// salu & fill_dictionaries
const uint8_t UNEXPECTED = 0x4;

// dictionary & salu & fill_dictionaries
const uint8_t REDEFINITION = 0x2;


#endif
