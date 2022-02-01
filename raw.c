#include"ext.h"

//initialize buffer
void init_abuf(buffer * p){
    p->b = NULL;
    p->len = 0;
}

//append to buffer
void append_buff(buffer *ab, const char *s, int l){
    char *n = realloc(ab->b, ab->len + l);

    if(n == NULL)
        return;
    memcpy(&n[ab->len], s, l);
    ab->b = n;
    ab->len += l;
    return;
}

//free buffer
void free_buff(buffer* ab){
    free(ab->b);
    return;
}

//exit and display error
void die(const char *s){
    perror(s);
    exit(1);
}

//sets terminal to default mode
void end_raw_mode(){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &win.orig_termios) == -1){
        die("tcsetattr");
    }
}

//sets terminal to raw mode
void set_raw_mode(){
    if(tcgetattr(STDIN_FILENO, &win.orig_termios) == -1){
        die("tcgetattr");
    }
    atexit(end_raw_mode);

    struct termios raw = win.orig_termios;
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1){
        die("tcsetattr");
    }
}

//reads and returns keypress
int read_key(){
    char c;
    int read_c;
    while((read_c = read(STDIN_FILENO, &c, 1)) != 1){
        if(read_c == -1 && errno != EAGAIN){
            die("read");
        }
    }

    if( c== '\x1b'){
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {
            if(seq[1] >= '0' && seq[1] <= '9'){
                if(read(STDIN_FILENO, &seq[2], 1) != 1)
                    return '\x1b';
                if(seq[2] == '~'){
                    switch(seq[1]){
                        case '1':
                            return HOME_KEY;
                        case '4':
                            return END_KEY;
                        case '3':
                            return DELETE_KEY;
                        case '5':
                            return PAGE_UP;
                        case '6':
                            return PAGE_DOWN;
                        case '7':
                            return HOME_KEY;
                        case '8':
                            return END_KEY;
                    }
                }
            }else{
                switch (seq[1]) {
                    case 'A':
                        return ARROW_UP;
                    case 'B':
                        return ARROW_DOWN;
                    case 'C':
                        return ARROW_RIGHT;
                    case 'D':
                        return ARROW_LEFT;
                    case 'H':
                        return HOME_KEY;
                    case 'F':
                        return END_KEY;
                }
            }
        }
        else if (seq[0] == 'O') {
            switch (seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
    return '\x1b';
    }
    return c;
}

//check keypress and map
void check_keypress(){
    static int quit = QUIT;
    int c = read_key();

    switch(c){
        case '\r':
            insert_new_line();
            break;
        case CTRL_KEY('q'):
            if(win.changed && quit > 0){
                StatusMessage("WARNING!! File has unsaved changes. Press Ctrl-Q %d more times to quit.", quit);
                quit--;
                return;
            }
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
        case CTRL_KEY('s'):
            save_editor();
            break;
        case HOME_KEY:
            win.sx = 0;
            break;
        case END_KEY:
            if(win.sy < win.numrows)
                win.sx = win.row[win.sy].size;
            break;
        // case CTRL_KEY('z'):
        //     editor_undo();
        //     break;
        case CTRL_KEY('f'):
            editor_find();
            break;
        case BACKSPACE:
        case CTRL_KEY('h'):
        case DELETE_KEY:
            if(c == DELETE_KEY)
                move_cursor(ARROW_RIGHT);
            delete_char();
            break;
        case PAGE_DOWN:
        case PAGE_UP:
            {
            if(c == PAGE_UP){
                win.sy = win.row_offset;
            }
            else if(c== PAGE_DOWN){
                win.sy = win.row_offset + win.scrows - 1;
                if(win.sy > win.numrows)
                    win.sy = win.numrows;
            }
            int count = win.scrows;
            while(count--)
                move_cursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
            }
            break;
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            move_cursor(c);
            break;
        case CTRL_KEY('l'):
        case '\x1b':
            break;
        default:
            insert_char(c);
            break;
    }
    quit = QUIT;
}