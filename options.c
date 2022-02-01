#define EXTERN
#include"ext.h"


//draws the rows to the terminal
void Refresh_screen(){
    editor_scroll();
    buffer ab;
    init_abuf(&ab);

    append_buff(&ab, "\x1b[2J", 4);
    append_buff(&ab, "\x1b[H", 3);
    //append_buff(&ab, "\x1b[45m", 5);

    print_rows(&ab);
    print_status_bar(&ab);
    print_mssg_bar(&ab);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", win.sy - win.row_offset + 1, win.rx - win.col_offset + 1);
    append_buff(&ab, buf, strlen(buf));

    write(STDOUT_FILENO, ab.b, ab.len);
    free_buff(&ab);
    return;
}


//changes cursor position depending on keypress
void move_cursor(int key){
    ROW *row = (win.sy >= win.numrows) ? NULL : &win.row[win.sy];
    switch(key){
        case ARROW_LEFT:
            if(win.sx != 0)
                win.sx--;
            else if(win.sy > 0){
                win.sy--;
                win.sx = win.row[win.sy].size;
            }
            break;
        case ARROW_RIGHT:
            if(row && win.sx < row->size)
                win.sx++;
            else if(row && win.sx == row->size){
                win.sy++;
                win.sx = 0;
            }
            break;
        case ARROW_UP:
            if(win.sy !=0)
                win.sy--;
            break;
        case ARROW_DOWN:
            if(win.sy < win.numrows)
                win.sy++;
            break;
    }
    row = (win.sy >= win.numrows) ? NULL : &win.row[win.sy];
    int rowlen = row ? row->size : 0;
    if(win.sx > rowlen){
        win.sx = rowlen;
    }
}

//scrolls the terminal
void editor_scroll(){
    win.rx = win.sx;

    if(win.sy < win.numrows){
        win.rx = editor_sxtorx(&win.row[win.sy], win.sx);
    }

    if(win.sy<win.row_offset){
        win.row_offset = win.sy;
    }
    if(win.sy >= win.row_offset+win.scrows){
        win.row_offset = win.sy - win.scrows + 1;
    }
    if(win.sx < win.col_offset){
        win.col_offset = win.rx;
    }
    if(win.sx >= win.col_offset+win.sccols){
        win.col_offset = win.rx - win.sccols + 1;
    }
}


//opens the editor with filename 
void open_editor(char *filename){
    free(win.filename);
    win.filename = strdup(filename);

    select_syntax();

    FILE *fp = fopen(filename, "rw");
    if(!fp)
        die("fopen");
    
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while((linelen = getline(&line, &linecap, fp))!=-1){
        while(linelen > 0 && (line[linelen-1]=='\n' || line[linelen-1] == '\r'))
            linelen--;
        editor_insert_row(win.numrows,line, linelen);
    }

    free(line);
    fclose(fp);
    win.changed = 0;
}

//saves current edited file
void save_editor() {
    if (win.filename == NULL){
        win.filename = editor_prompt("Save as: %s ");
        if (win.filename == NULL){
            StatusMessage("Cancelled");
            return;
        }
        select_syntax();
    }

    int len;
    char *buf = rows_to_string(&len);
    int fd = open(win.filename, O_RDWR | O_CREAT, 0644);
    if(fd != -1){
        if(ftruncate(fd, len) != -1){
            if(write(fd, buf, len) == len){
                close(fd);
                free(buf);
                win.changed = 0;
                StatusMessage("File saved successfully");
                return;
            }
        }
        close(fd);
    }
    free(buf);
    StatusMessage("Cannot save file! I/O error: %s", strerror(errno));
}

//writes to the prompt
char *editor_prompt(char *prompt){
    size_t bufsize = 64;
    char *buf = (char *)malloc(bufsize);

    size_t buflen = 0;
    buf[0] = '\0';

    while(1){
        StatusMessage(prompt, buf);
        Refresh_screen();

        int c = read_key();
        if (c == DELETE_KEY || c == CTRL_KEY('h') || c == BACKSPACE){
            if(buflen !=0)
                buf[--buflen] = '\0';
        }
        else if (c == '\x1b'){
                StatusMessage("");
                free(buf);
                return NULL;
            }
            else if (c == '\r'){
                if (buflen != 0){
                    StatusMessage("");
                    return buf;
                }
            }
            else if (!iscntrl(c) && c < 128){
                if (buflen == bufsize - 1){
                    bufsize *= 2;
                    buf = realloc(buf, bufsize);
                }
                buf[buflen++] = c;
                buf[buflen] = '\0';
            }
    }
}

//finds a string
void editor_find(){
    int sx_orig = win.sx;
    int sy_orig = win.sy;
    int coloff_orig = win.col_offset;
    int rowoff_orig = win.row_offset;

    char *query = editor_prompt("Search: %s ");
    if(query == NULL)
        return;
    int i;
    for (i = 0; i < win.numrows; i++){
        ROW *row = &win.row[i];
        char *found = strstr(row->render, query);
        if(found){
            win.sy = i;
            win.sx = row_rx_to_sx(row, found - row->render);
            win.row_offset = win.numrows;
            break;
        }
    }
    if(query){
        free(query);
    }else{
        win.sx = sx_orig;
        win.sy = sy_orig;
        win.col_offset = coloff_orig;
        win.row_offset = rowoff_orig;
    }
    
}

void editor_undo(){
    node *current = pop(st);
    node *un = pop(st);
    if(un == NULL)
        return;
    win = un->w;
    free(un);
    free(current);
    return;
}



