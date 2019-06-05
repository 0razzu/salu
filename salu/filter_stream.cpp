#include "filter_stream.hpp"


FilterStream::FilterStream() {
    state = NO_FILE;
    piece[0] = '\0';
    place = PIECE_SIZE;
    add_space = 0;
}


FilterStream::FilterStream(const char file_path[]) {
    open(file_path);
}


FilterStream::~FilterStream() {
    if (fin.is_open())
        fin.close();
}


void FilterStream::get_piece() {
    char c, next;
    ushort i = 0;
    
    if (isOK()) {
        place = 0;
        
        next = fin.peek();
        
        while ((i < PIECE_SIZE - 1) && (!fin.fail()) && (next != EOF)) {
            if ((next == ' ') || (next == '\t') || (next == '\n')) {
                fin.get(c);
                
                if (add_space) {
                    piece[i] = ' ';
                    add_space = 0;
                    i++;
                }
                    
            }
            
            else {
                fin.get(c);
                piece[i] = next;
                add_space = 1;
                i++;
            }
            
            if (!fin.fail())
                next = fin.peek();
            
            else
                state |= READ_ERR;
        }
        
        piece[i] = '\0';
        
        if ((i < PIECE_SIZE - 1) && (state == OK)) {
            fin.close();
            state |= END;
        }
    }
}


void FilterStream::open(const char file_path[]) {
    fin.open(file_path);
    
    if (fin.is_open()) {
        state = OK;
        add_space = 0;
        get_piece();
    }
    
    else
        state = NO_FILE;
}


char FilterStream::peek() {
    char c = '\0';
    
    if (place >= PIECE_SIZE - 1)
        get_piece();
    
    if (isOK())
        c = piece[place];
    
    return c;
}


char FilterStream::get() {
    char c = '\0';
    
    if (place >= PIECE_SIZE - 1)
        get_piece();
    
    if (isOK()) {
        c = piece[place];
        place++;
    }
    
    return c;
}


void FilterStream::getline(char line[], ushort k, char terminator) {
    ushort i = 0;
    char c;
    
    if (!line)
        state |= MEM_ERR;
    
    else if (isOK()) {
        do {
            c = peek();
            
            if ((c != terminator) && (!isRead())) {
                c = get();
                line[i] = c;
                i++;
            }
        } while ((i < k - 1) && (c != terminator) && (isOK()));
        
        line[i] = '\0';
    }
}


bool FilterStream::isRead() {
    return (state & END) && (piece[place] == '\0');
}


bool FilterStream::isOK() {
    return ((state == OK) || (state & END));
}


uint8_t FilterStream::getState() {
    return state;
}
