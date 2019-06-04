#include "globals.hpp"
#include "stack.cpp"
#include "dictionary.cpp"
#include "filter_stream.hpp"
#include <iostream>
#include <fstream>
#include <cmath>


bool read_num(std::istream &in, u_long &curr, double &num) {
    bool ok = 0, frac = 0, first_dot = 1;
    short frac_digits = 0;
    char c = '\0', next = '0';
    unsigned frac_part = 0;
    
    num = 0;
    
    next = in.peek();
    
    if (((next >= '0') && (next <= '9')) || (next == '.') || (next == ','))
        ok = 1;
    
    while ((ok) && (((next >= '0') && (next <= '9')) || (next == '.') || (next == ','))) {
        in.get(c);
        curr++;
        
        if ((first_dot) && ((c == '.') || (c == ','))) {
            first_dot = 0;
            frac = 1;
        }
        
        else if ((c >= '0') && (c <= '9')) {
            if (!frac)
                num = 10*num + c - '0';
            
            else {
                frac_digits++;
                frac_part = 10*frac_part + c - '0';
            }
        }
        
        else
            ok = 0;
        
        if (ok)
            next = in.peek();
    }
    
    if ((frac) && (num == 0) && (frac_digits == 0))
        ok = 0;
    
    if (ok)
        num += frac_part * pow(10, -frac_digits);
    
    return ok;
}


uint8_t salu(FilterStream &code) {
    uint8_t state = OK;
    Stack<double> r_operands;
    Stack<bool> b_operands, is_r;
    char c = '\0', next, command[4], wrong_symbol;
    u_long curr = 0, curr_line = 1;
    
    while ((c != '{') && (c != EOF) && (code.isOK())) {
        c = code.get();
        
        if ((c == '\n') && (code.isOK())) {
            curr = 0;
            curr_line++;
        }
        
        else
            curr++;
    }
    
    if (c == '{') {
        state |= WORKING;
    
        next = code.peek();
        
        while ((state == WORKING) && (code.isOK())) {
            if (((next >= 'A') && (next <= 'Z')) || ((next >= 'a') && (next <= 'z'))) {
                code.getline(command, 4, '\0');
                
                // обработка команд
            }
            
            else if (next == '}') {
                c = code.get();
                state &= ~WORKING;
            }
            
            next = code.peek();
        }
    }
    
    if (code.getState() & READ_ERR)
        state |= READ_ERR;
    
    return state;
}


int main(int argc, const char * argv[]) {
    FilterStream code;
    char file_path[1024] = "";
    
    if (argc > 2)
        std::cerr << "Некорректное число входных аргументов" << std::endl;
    
    else {
        if (argc == 1) {
            while (file_path[0] == '\0') {
                std::cout << "Введите путь к файлу с кодом для САЛУ" << std::endl;
                rewind(stdin);
                std::cin.getline(file_path, 1024, '\n');
            }
        }
        
        else
            strcat(file_path, argv[1]);
        
        code.open(file_path);
        
        if (code.isOK())
            salu(code);
        
        else
            std::cerr << "Ошибка при открытии файла «" << file_path << "»" << std::endl;
    }
    
    return 0;
}
