#include "globals.hpp"
#include "stack.cpp"
#include "dictionary.cpp"
#include "filter_stream.hpp"
#include <iostream>
#include <fstream>
#include <cmath>


bool read_num(FilterStream &in, double &num) {
    bool ok = 0, negative = 0, frac = 0, first_dot = 1;
    short frac_digits = 0;
    char c = '\0', next = '0';
    unsigned frac_part = 0;
    
    num = 0;
    
    next = in.peek();
    ok = in.isOK();
    
    if (!(((next >= '0') && (next <= '9')) || (next == '.') || (next == ',') || (next == '+') || (next == '-')))
        ok = 0;
    
    else if ((next == '+') || (next == '-')) {
        c = in.get();
        next = in.peek();
        ok = in.isOK();
        
        if (c == '-')
            negative = 1;
    }
    
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
    
    if (ok) {
        num += frac_part * pow(10, -frac_digits);
        
        if (negative)
            num = -num;
    }
    
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
                
                space = vars.peek();
                if (space == ' ') {
                    space = vars.get();
                    state |= vars.getState() & (MEM_ERR | READ_ERR);
                }
            }
            
            else
                state |= UNEXPECTED;
        }
    }
    
    return (state & ~END);
}


uint8_t salu(FilterStream &code, bool &res_is_real, double &Rres, bool &Bres) {
    uint8_t state = OK;
    Stack<double> r_operands;
    Stack<bool> b_operands, is_real;
    Dictionary<double> r_vars;
    Dictionary<bool> b_vars;
    char c = '\0', next, command[4], name[16];
    bool read_ok, TF, Ba, Bb, a_is_real, b_is_real;
    double num, Ra, Rb;
    
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
                    
                    else if (strcmp("POW", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && ((!a_is_real) || (!b_is_real)))
                            state |= INCOMPATIBLE;
                        
                        Rb = r_operands.pop();
                        Ra = r_operands.pop();
                        state |= r_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            Ra = pow(Ra, Rb);
                            r_operands.push(Ra);
                            is_real.push(1);
                            state |= (r_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("NOT", command) == 0) {
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && (a_is_real))
                            state |= INCOMPATIBLE;
                        
                        Ba = b_operands.pop();
                        state |= b_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            Ba = !Ba;
                            b_operands.push(Ba);
                            is_real.push(0);
                            state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("POS", command) == 0) {
                        a_is_real = is_real.peek();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && (!a_is_real))
                            state |= INCOMPATIBLE;
                        
                        Ra = r_operands.peek();
                        state |= r_operands.getState() & (MEM_ERR | EMPTY);
                    }
                    
                    else if (strcmp("NEG", command) == 0) {
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && (!a_is_real))
                            state |= INCOMPATIBLE;
                        
                        Ra = r_operands.pop();
                        state |= r_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            Ra = -Ra;
                            r_operands.push(Ra);
                            is_real.push(1);
                            state |= (r_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("MUL", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && ((!a_is_real) || (!b_is_real)))
                            state |= INCOMPATIBLE;
                        
                        Rb = r_operands.pop();
                        Ra = r_operands.pop();
                        state |= r_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            Ra *= Rb;
                            r_operands.push(Ra);
                            is_real.push(1);
                            state |= (r_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("DIV", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && ((!a_is_real) || (!b_is_real)))
                            state |= INCOMPATIBLE;
                        
                        Rb = r_operands.pop();
                        Ra = r_operands.pop();
                        state |= r_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            Ra /= Rb;
                            r_operands.push(Ra);
                            is_real.push(1);
                            state |= (r_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("AND", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && ((a_is_real) || (b_is_real)))
                            state |= INCOMPATIBLE;
                        
                        Bb = b_operands.pop();
                        Ba = b_operands.pop();
                        state |= b_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            Ba = Ba && Bb;
                            b_operands.push(Ba);
                            is_real.push(0);
                            state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("SUM", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && ((!a_is_real) || (!b_is_real)))
                            state |= INCOMPATIBLE;
                        
                        Rb = r_operands.pop();
                        Ra = r_operands.pop();
                        state |= r_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            Ra += Rb;
                            r_operands.push(Ra);
                            is_real.push(1);
                            state |= (r_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("SUB", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && ((!a_is_real) || (!b_is_real)))
                            state |= INCOMPATIBLE;
                        
                        Rb = r_operands.pop();
                        Ra = r_operands.pop();
                        state |= r_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            Ra -= Rb;
                            r_operands.push(Ra);
                            is_real.push(1);
                            state |= (r_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("LES", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            if ((a_is_real) && (b_is_real)) {
                                Rb = r_operands.pop();
                                Ra = r_operands.pop();
                                state |= r_operands.getState() & (MEM_ERR | EMPTY);
                                
                                if (state == WORKING) {
                                    Ba = Ra < Rb;
                                    b_operands.push(Ba);
                                    is_real.push(0);
                                    state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                                }
                            }
                            
                            else if ((!a_is_real) && (!b_is_real)) {
                                Bb = b_operands.pop();
                                Ba = b_operands.pop();
                                state |= b_operands.getState() & (MEM_ERR | EMPTY);
                                
                                if (state == WORKING) {
                                    Ba = Ba < Bb;
                                    b_operands.push(Ba);
                                    is_real.push(0);
                                    state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                                }
                            }
                            
                            else
                                state |= INCOMPATIBLE;
                        }
                    }
                    
                    else if (strcmp("MOR", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            if ((a_is_real) && (b_is_real)) {
                                Rb = r_operands.pop();
                                Ra = r_operands.pop();
                                state |= r_operands.getState() & (MEM_ERR | EMPTY);
                                
                                if (state == WORKING) {
                                    Ba = Ra > Rb;
                                    b_operands.push(Ba);
                                    is_real.push(0);
                                    state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                                }
                            }
                            
                            else if ((!a_is_real) && (!b_is_real)) {
                                Bb = b_operands.pop();
                                Ba = b_operands.pop();
                                state |= b_operands.getState() & (MEM_ERR | EMPTY);
                                
                                if (state == WORKING) {
                                    Ba = Ba > Bb;
                                    b_operands.push(Ba);
                                    is_real.push(0);
                                    state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                                }
                            }
                            
                            else
                                state |= INCOMPATIBLE;
                        }
                    }
                    
                    else if (strcmp("EQL", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            if ((a_is_real) && (b_is_real)) {
                                Rb = r_operands.pop();
                                Ra = r_operands.pop();
                                state |= r_operands.getState() & (MEM_ERR | EMPTY);
                                
                                if (state == WORKING) {
                                    Ba = Ra == Rb;
                                    b_operands.push(Ba);
                                    is_real.push(0);
                                    state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                                }
                            }
                            
                            else if ((!a_is_real) && (!b_is_real)) {
                                Bb = b_operands.pop();
                                Ba = b_operands.pop();
                                state |= b_operands.getState() & (MEM_ERR | EMPTY);
                                
                                if (state == WORKING) {
                                    Ba = Ba == Bb;
                                    b_operands.push(Ba);
                                    is_real.push(0);
                                    state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                                }
                            }
                            
                            else
                                state |= INCOMPATIBLE;
                        }
                    }
                    
                    else if (strcmp("NEQ", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            if ((a_is_real) && (b_is_real)) {
                                Rb = r_operands.pop();
                                Ra = r_operands.pop();
                                state |= r_operands.getState() & (MEM_ERR | EMPTY);
                                
                                if (state == WORKING) {
                                    Ba = Ra != Rb;
                                    b_operands.push(Ba);
                                    is_real.push(0);
                                    state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                                }
                            }
                            
                            else if ((!a_is_real) && (!b_is_real)) {
                                Bb = b_operands.pop();
                                Ba = b_operands.pop();
                                state |= b_operands.getState() & (MEM_ERR | EMPTY);
                                
                                if (state == WORKING) {
                                    Ba = Ba != Bb;
                                    b_operands.push(Ba);
                                    is_real.push(0);
                                    state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                                }
                            }
                            
                            else
                                state |= INCOMPATIBLE;
                        }
                    }
                    
                    else if (strcmp("OR_", command) == 0) {
                        b_is_real = is_real.pop();
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && ((a_is_real) || (b_is_real)))
                            state |= INCOMPATIBLE;
                        
                        Bb = b_operands.pop();
                        Ba = b_operands.pop();
                        state |= b_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            Ba = Ba || Bb;
                            b_operands.push(Ba);
                            is_real.push(0);
                            state |= (b_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("FRC", command) == 0) {
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && (!a_is_real))
                            state |= INCOMPATIBLE;
                        
                        Ra = r_operands.pop();
                        state |= r_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            if (Ra < 0)
                                Ra -= (int)Ra - 1;
                            
                            else
                                Ra -= (int)Ra;
                            
                            r_operands.push(Ra);
                            is_real.push(1);
                            state |= (r_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("ENT", command) == 0) {
                        a_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if ((state == WORKING) && (!a_is_real))
                            state |= INCOMPATIBLE;
                        
                        Ra = r_operands.pop();
                        state |= r_operands.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            if (Ra < 0)
                                Ra = (int)Ra - 1;
                            
                            else
                                Ra = (int)Ra;
                            
                            r_operands.push(Ra);
                            is_real.push(1);
                            state |= (r_operands.getState() | is_real.getState()) & MEM_ERR;
                        }
                    }
                    
                    else if (strcmp("RES", command) == 0) {
                        res_is_real = is_real.pop();
                        state |= is_real.getState() & (MEM_ERR | EMPTY);
                        
                        if (state == WORKING) {
                            if (res_is_real)
                                Rres = r_operands.pop();
                            
                            else
                                Bres = b_operands.pop();
                            
                            state |= (r_operands.getState() | b_operands.getState()) & (MEM_ERR | EMPTY);
                        }
                    }
                    
                    else
                        state |= UNEXPECTED;
                }
                
                else
                    state |= READ_ERR;
            }
            
            else if (next == '}') {
                c = code.get();
                state &= ~WORKING;
            }
            
            else
                state |= UNEXPECTED;
            
            if (state == WORKING) {
                next = code.peek();
                state |= code.getState() & (MEM_ERR | READ_ERR);
            }
        }
    }
    
    if ((state & ~WORKING) != OK)
        state &= ~WORKING;
    
    if ((state == OK) && ((!r_operands.isEmpty()) || (!b_operands.isEmpty()) || (!is_real.isEmpty())))
        state |= OPERANDS;
    
    return state;
}


ushort read_errors(char errors[8][256]) {
    FilterStream errors_file("salu.errors");
    char curr = '\0', next;
    ushort res = 0, curr_error_code;
    
    if (errors_file.isOK()) {
        for (ushort i = 0; i < 8; i++)
            errors[i][0] = '\0';
            
        while ((!errors_file.isRead()) && (res == 0)) {
            curr_error_code = 0;
            next = errors_file.peek();
            if (!errors_file.isOK())
                res = 5;
                
            while ((next >= '0') && (next <= '9') && (res == 0) && (!errors_file.isRead())) {
                curr = errors_file.get();
                    
                if (errors_file.isOK())
                    curr_error_code = curr_error_code * 10 + curr - '0';
                    
                else
                    res = 5;
                
                next = errors_file.peek();
            }
                
            curr = errors_file.get();
            if (!errors_file.isOK())
                res = 5;
            
            else if (curr != ' ')
                res = 2;
            
            else if ((curr_error_code >= 0) && (curr_error_code <= 7)) {
                if (errors[curr_error_code][0] == '\0') {
                    errors_file.getline(errors[curr_error_code], 256, ';');
                    curr = errors_file.get();
                    if (!errors_file.isOK())
                        res = 5;
                }
                    
                else
                    res = 4;
            }
            
            next = errors_file.peek();
            
            if (next == ' ')
                curr = errors_file.get();
            
            if (!errors_file.isOK())
                res = 5;
        }
            
        if (res == 0)
            for (unsigned short i = 0; i < 8; i++)
                if (errors[i][0] == '\0')
                    res = 3;
    }
    
    else
        res = 1;
    
    return res;
}


int main(int argc, const char *argv[]) {
    FilterStream code;
    char file_path[1024] = "";
    uint8_t salu_res;
    bool res_is_real, Bres;
    double Rres;
    ushort read_errors_res;
    char errors[8][256];
    
    read_errors_res = read_errors(errors);
    
    if (read_errors_res != 0) {
        std::cerr << std::endl;
        
        if (read_errors_res == 1)
            std::cerr << "Ошибка при открытии файла определения ошибок";
        
        else if (read_errors_res == 2)
            std::cerr << "Неожиданный символ в файле определения ошибок";
        
        else if (read_errors_res == 3)
            std::cerr << "В файле определения ошибок описаны не все типы ошибок";
        
        else if (read_errors_res == 4)
            std::cerr << "Попытка переопределения кода ошибки в файле определения ошибок";
        
        else if (read_errors_res == 5)
            std::cerr << "Ошибка чтения из файла определения ошибок";
        
        std::cerr << std::endl << std::endl;
    }
    
    else {
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
            
            if (code.isOK()) {
                salu_res = salu(code, res_is_real, Rres, Bres);
                
                if (salu_res == OK) {
                    std::cout << std::endl << "Результат: ";
                    
                    if (res_is_real)
                        std::cout << Rres;
                    
                    else
                        std::cout << std::boolalpha << Bres;
                    
                    std::cout << std::endl << std::endl;
                }
                
                else {
                    std::cerr << std::endl;
                    
                    if (salu_res & WORKING)
                        std::cerr << errors[0];
                    
                    if (salu_res & OPERANDS)
                        std::cerr << errors[1];
                    
                    if (salu_res & NO_FILE)
                        std::cerr << errors[2];
                    
                    if (salu_res & INCOMPATIBLE)
                        std::cerr << errors[3];
                    
                    if (salu_res & READ_ERR)
                        std::cerr << errors[4];
                    
                    if (salu_res & UNEXPECTED)
                        std::cerr << errors[5];
                    
                    if (salu_res & REDEFINITION)
                        std::cerr << errors[6];
                    
                    if (salu_res & MEM_ERR)
                        std::cerr << errors[7];
                    
                    std::cerr << std::endl << std::endl;
                }
            }
            
            else
                std::cerr << std::endl << "Ошибка при открытии файла «" << file_path << "»" << std::endl << std::endl;
        }
    }
    
    return 0;
}
