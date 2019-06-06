#ifndef salu_hpp
#define salu_hpp


#include "globals.hpp"
#include "stack.cpp"
#include "dictionary.cpp"
#include "filter_stream.hpp"
#include <iostream>
#include <fstream>
#include <cmath>


uint8_t salu(FilterStream &code, bool &res_is_real, double &Rres, bool &Bres);


#endif
