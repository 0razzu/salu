#ifndef filter_stream_hpp
#define filter_stream_hpp


#include "globals.hpp"
#include <iostream>
#include <fstream>


class FilterStream {
private:
    uint8_t state;
    std::ifstream fin;
    char piece[PIECE_SIZE];
    ushort place;
    bool add_space;
    
    void get_piece();
    
public:
    FilterStream();
    FilterStream(const char[]);
    ~FilterStream();
    
    void open(const char[]);
    
    char peek();
    char get();
    void getline(char[], ushort, char);
    
    bool isRead();
    bool isOK();
    uint8_t getState();
};


#endif
