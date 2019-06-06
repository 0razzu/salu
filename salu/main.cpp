#include "globals.hpp"
#include "filter_stream.hpp"
#include "salu.hpp"
#include <iostream>


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
                    std::cout << std::endl << "Введите путь к файлу с кодом для САЛУ" << std::endl;
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
                        std::cerr << errors[0] << std::endl;
                    
                    if (salu_res & OPERANDS)
                        std::cerr << errors[1] << std::endl;
                    
                    if (salu_res & NO_FILE)
                        std::cerr << errors[2] << std::endl;
                    
                    if (salu_res & INCOMPATIBLE)
                        std::cerr << errors[3] << std::endl;
                    
                    if (salu_res & READ_ERR)
                        std::cerr << errors[4] << std::endl;
                    
                    if (salu_res & UNEXPECTED)
                        std::cerr << errors[5] << std::endl;
                    
                    if (salu_res & REDEFINITION)
                        std::cerr << errors[6] << std::endl;
                    
                    if (salu_res & MEM_ERR)
                        std::cerr << errors[7] << std::endl;
                    
                    std::cerr << std::endl;
                }
            }
            
            else
                std::cerr << std::endl << "Ошибка при открытии файла «" << file_path << "»" << std::endl << std::endl;
        }
    }
    
    return 0;
}
