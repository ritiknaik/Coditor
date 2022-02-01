#include"ext.h"


//---------------------Row on Editor-----------------//

//inserts new line
void insert_new_line(){
    if (win.sx == 0) {
        editor_insert_row(win.sy, "", 0);
    } else {
        ROW *row = &win.row[win.sy];
        editor_insert_row(win.sy + 1, &row->line[win.sx], row->size - win.sx);
        row = &win.row[win.sy];
        row->size = win.sx;
        row->line[row->size] = '\0';
        editor_update_row(row);
    }
    win.sy++;
    win.sx = 0;
}

//delete char from row
void delete_char(){
    if(win.sy == win.numrows)
        return;
    if(win.sx == 0 && win.sy == 0)
        return;

    ROW *row = &win.row[win.sy];
    if(win.sx > 0){
        row_delete_char(row, win.sx - 1);
        win.sx--;
    }else{
        win.sx = win.row[win.sy - 1].size;
        append_string_to_row(&win.row[win.sy - 1], row->line, row->size);
        del_row(win.sy);
        win.sy--;
    }
}


//inserts a new row
void editor_insert_row(int at, char *s, size_t len){
    if(at < 0 || at > win.numrows)
        return;
    win.row = realloc(win.row, sizeof(ROW) * (win.numrows + 1));
    memmove(&win.row[at + 1], &win.row[at], sizeof(ROW) * (win.numrows - at));
        
    win.row[at].size = len;
    win.row[at].line = malloc(len + 1);
    memcpy(win.row[at].line, s, len);
    win.row[at].line[len] = '\0';

    win.row[at].rsize = 0;
    win.row[at].render = NULL;
    win.row[at].h = NULL;
    editor_update_row(&win.row[at]);

    win.numrows++;
    win.changed++;
    return;
}

//deletes a row
void del_row(int at){
    if(at < 0 || at >= win.numrows)
        return;
    free_row(&win.row[at]);
    memmove(&win.row[at], &win.row[at + 1], sizeof(ROW) * (win.numrows - at - 1));
    win.numrows--;
    win.changed++;
}

//converts rows to string to display
char *rows_to_string(int *buflen){
    int tlen = 0;
    int j;
    for (j = 0; j < win.numrows; j++){
        tlen += win.row[j].size + 1;
    }
    *buflen = tlen;
    char *buf = (char*)malloc(tlen);
    char *p = buf;
    for (j = 0; j < win.numrows; j++){
        memcpy(p, win.row[j].line, win.row[j].size);
        p += win.row[j].size;
        *p = '\n';
        p++;
    }
    return buf;
}


//------------------- Row Manipulation -------------------//


//insert char to row
void row_insert_char(ROW *row, int at, int c) {
    if (at < 0 || at > row->size) at = row->size;
    row->line = realloc(row->line, row->size + 2);
    memmove(&row->line[at + 1], &row->line[at], row->size - at + 1);
    row->size++;
    row->line[at] = c;
    editor_update_row(row);
    win.changed++;
}

//appends string to row
void append_string_to_row(ROW *row, char *s, size_t len){
    row->line = realloc(row->line, row->size + len + 1);
    memcpy(&row->line[row->size], s, len);
    row->size += len;
    row->line[row->size] = '\0';
    editor_update_row(row);
    win.changed++;
}

//deletes char from row
void row_delete_char(ROW *row, int at){
    if(at < 0 || at >= row->size)
        return;
    memmove(&row->line[at], &row->line[at + 1], row->size - at);
    row->size--;
    editor_update_row(row);
    win.changed++;
}

//insert a char
void insert_char(int c){
    if(win.sy == win.numrows){
        editor_insert_row(win.numrows, "", 0);
    }
    row_insert_char(&win.row[win.sy], win.sx, c);
    win.sx++;
}

//updates the editor rows
void editor_update_row(ROW *row){
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++)
        if(row->line[j] == '\t')
            tabs++;

    free(row->render);
    row->render = malloc(row->size + tabs*(TAB -1) + 1);

    
    int idx = 0;
    for (j = 0; j < row->size; j++){
        if(row->line[j] == '\t'){
            row->render[idx++] = ' ';
            while(idx%TAB !=0)
                row->render[idx++] = ' ';
        }else{
            row->render[idx++] = row->line[j];
        }
    }
    row->render[idx] = '\0';
    row->rsize = idx;

    update_syntax(row);
    return;
}

//free row
void free_row(ROW *row){
    free(row->render);
    free(row->line);
    free(row->h);
}

//converts sx to rx
int editor_sxtorx(ROW *row, int sx){
    int rx = 0;
    int j;
    for (j = 0; j < sx; j++) {
        if (row->line[j] == '\t')
            rx += (TAB - 1) - (rx % TAB);
        rx++;
    }
    return rx;
}


//converts rx to sx
int row_rx_to_sx(ROW *row, int rx){
    int crx = 0;
    int cx;
    for (cx = 0; cx < row->size; cx++){
        if(row->line[cx] == '\t')
            crx += TAB - 1 - (crx % TAB);
        crx++;
        if(crx > rx)
            return cx;
    }
    return cx;
}
