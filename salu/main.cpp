#include "globals.hpp"
#include "stack.cpp"
#include "dictionary.cpp"
#include "filter_stream.hpp"
#include <iostream>
#include <fstream>
#include <cmath>


bool read_num(FilterStream &in, double &num) {
    bool ok = 0, frac = 0, first_dot = 1;
    short frac_digits = 0;
    char c = '\0', next = '0';
    unsigned frac_part = 0;
    
    num = 0;
    
    next = in.peek();
    ok = in.isOK();
    
    if (!(((next >= '0') && (next <= '9')) || (next == '.') || (next == ',')))
        ok = 0;
    
    while ((ok) && (((next >= '0') && (next <= '9')) || (next == '.') || (next == ','))) {
        c = in.get();
        ok = in.isOK();
        
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


uint8_t fillDictionaries(Dictionary<double> &r_vars, Dictionary<bool> &b_vars) {
    uint8_t state;
    FilterStream vars("vars.txt");
    char name[16], type, TF, space;
    double num;
    bool ok;
    
    if (vars.isOK())
        state = OK;
    
    else
        state = NO_FILE;
        
    while ((state == OK) && (!vars.isRead())) {
        type = vars.peek();
        state |= vars.getState() & (MEM_ERR | READ_ERR);
        
        if (state == OK) {
            if ((type == 'R') || (type == 'B')) {
                type = vars.get();
                vars.getline(name, 16, ' ');
                space = vars.peek();
                state |= vars.getState() & (MEM_ERR | READ_ERR);
                
                if (state == OK) {
                    if (type == 'R') {
                        if (space == ' ') {
                            space = vars.get();
                            ok = read_num(vars, num);
                            
                            if (ok) {
                                r_vars.add(name, num);
                                state |= r_vars.getState() & (MEM_ERR | REDEFINITION);
                            }
                        }
                        
                        else
                            state |= UNEXPECTED;
                    }
                        
                    else if ((type == 'B') && (space == ' ')) {
                        space = vars.get();
                        TF = vars.peek();
                        state |= vars.getState() & (MEM_ERR | READ_ERR);
                        
                        if ((TF == 'T') || (TF == '1')) {
                            TF = vars.get();
                            b_vars.add(name, 1);
                            state |= b_vars.getState() & (MEM_ERR | REDEFINITION);
                        }
                        
                        else if ((TF == 'F') || (TF == '0')) {
                            TF = vars.get();
                            b_vars.add(name, 0);
                            state |= b_vars.getState() & (MEM_ERR | REDEFINITION);
                        }
                        
                        else
                            state |= UNEXPECTED;
                    }
                }
            }
            
            else
                state |= UNEXPECTED;
        }
    }
    
    return state;
}


uint8_t salu(FilterStream &code, char &wrong_symbol) {
    uint8_t state = OK;
    Stack<double> r_operands;
    Stack<bool> b_operands, is_real;
    Dictionary<double> r_vars;
    Dictionary<bool> b_vars;
    char c = '\0', next, command[4], name[16];
    bool read_ok, TF;
    double num;
    
    state |= fillDictionaries(r_vars, b_vars);
    
    while ((c != '{') && (c != EOF) && (state == OK)) {
        c = code.get();
        state |= code.getState() & (MEM_ERR | READ_ERR);
    }
    
    if ((c == '{') && (state == OK)) {
        state |= WORKING;
        
        next = code.peek();
        state |= code.getState() & (MEM_ERR | READ_ERR);
        
        while ((state == WORKING) && (code.isOK()) && (next != EOF)) {
            if (next == ' ') {
                c = code.get();
                next = code.peek();
                state |= code.getState() & (MEM_ERR | READ_ERR);
            }
            
            else
                state |= UNEXPECTED;
            
            if (((next >= 'A') && (next <= 'Z')) || ((next >= 'a') && (next <= 'z'))) {
                code.getline(command, 4, '\0');
                
                if (code.isOK()) {
                    if (strcmp("PSH", command) == 0) {
                        next = code.peek();
                        
                        if (code.isOK()) {
                            if (next == ' ') {
                                c = code.get();
                                next = code.peek();
                                state |= code.getState() & (MEM_ERR | READ_ERR);
                            }
                            
                            else
                                state |= UNEXPECTED;
                            
                            if (next == 'T') {
                                c = code.get();
                                b_operands.push(1);
                                is_real.push(0);
                            }
                            
                            else if (next == 'F') {
                                c = code.get();
                                b_operands.push(0);
                                is_real.push(0);
                            }
                            
                            else if (((next >= '0') && (next <= '9')) || (next == '-') || (next == '.') || (next == ',')) {
                                read_ok = read_num(code, num);
                                
                                if (read_ok) {
                                    r_operands.push(num);
                                    is_real.push(1);
                                }
                            }
                            
                            else if (next == 'R') {
                                c = code.get();
                                code.getline(name, 16, ' ');
                                num = r_vars.getValue(name);
                                
                                if (r_vars.getState() & NO_ELEM)
                                    state |= UNEXPECTED;
                                
                                else {
                                    r_operands.push(num);
                                    is_real.push(1);
                                }
                            }
                            
                            else if (next == 'B') {
                                c = code.get();
                                code.getline(name, 16, ' ');
                                TF = b_vars.getValue(name);
                                
                                if (b_vars.getState() & NO_ELEM)
                                    state |= UNEXPECTED;
                                
                                else {
                                    b_operands.push(TF);
                                    is_real.push(0);
                                }
                            }
                            
                            else
                                state |= UNEXPECTED;
                            
                            state |= (r_operands.getState() | b_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                        
                        else
                            state |= READ_ERR;
                    }
                }
                
                else
                    state |= READ_ERR;
            }
            
            else if (next == '}') {
                c = code.get();
                state &= ~WORKING;
            }
            
            next = code.peek();
            state |= code.getState() & (MEM_ERR | READ_ERR);
        }
    }
    
    return state;
}


int main(int argc, const char * argv[]) {
    FilterStream code;
    char file_path[1024] = "", wrong_symbol = '\0';
    
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
            salu(code, wrong_symbol);
        
        else
            std::cerr << "Ошибка при открытии файла «" << file_path << "»" << std::endl;
    }
    
    return 0;
}
