#include "globals.hpp"
#include "stack.hpp"
#include "stack.cpp"
#include "dictionary.hpp"
#include "dictionary.cpp"
#include <iostream>
#include <fstream>


int main(int argc, const char * argv[]) {
    std::ifstream fin;
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
        
        fin.open(file_path);
        
        if (fin.is_open()) {
            //вызов САЛУ
            
            fin.close();
        }
        
        else
            std::cerr << "Ошибка при открытии файла «" << file_path << "»" << std::endl;
    }
    
    return 0;
}
